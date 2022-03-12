#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "OLED.h"

#define ROW_COUNT 128
#define COL_COUNT   8

uint8_t life[ROW_COUNT][COL_COUNT];
uint8_t new_life[ROW_COUNT][COL_COUNT];

void start_life(uint32_t seed){
    srand(seed);
    for (int i = 0; i < ROW_COUNT; ++i){
        for (int j = 0; j < COL_COUNT; ++j){
            life[i][j] = rand() % 256;
        }
    }
    load_image(life);
}

// Any live cell with two or three live neighbours survives.
// Any dead cell with three live neighbours becomes a live cell.
// All other live cells die in the next generation. Similarly, all other dead cells stay dead.
uint8_t count_neighbours(uint8_t i, uint8_t j, uint8_t bit_index){
    uint8_t count = 0;
    uint8_t up_row = (i == 0) ? (ROW_COUNT - 1) : (i - 1);
    uint8_t down_row = (i == 7) ? 0 : (i + 1);
    uint8_t left_col = j;
    uint8_t left_bit_index = (bit_index == 7) ? 0 : (bit_index - 1);
    uint8_t right_col = j;
    uint8_t right_bit_index = (bit_index == 0) ? 7 : (bit_index + 1);

    //Left location
    //start of byte
    if (bit_index == 7){
        //start of row
        if (j == 0){
            //Wrap around
            left_col = COL_COUNT - 1;
        }
        else {
            //not start of row
            left_col = j - 1;
        }
    }

    //Right location
    //End of byte
    if (bit_index == 0){
        //start of row
        if (j == (ROW_COUNT - 1)){
            //Wrap around
            right_col = 0;
        }
        else {
            //not end of row
            right_col = j + 1;
        }
    } 
    
    //N
    count += (life[up_row][j] & (1 << bit_index)) ? 1 : 0;
    //NW
    count += (life[up_row][left_col] & (1 << left_bit_index)) ? 1 : 0;
    //NE
    count += (life[up_row][right_col] & (1 << right_bit_index)) ? 1 : 0;
    //W
    count += (life[i][left_col] & (1 << left_bit_index)) ? 1 : 0;
    //E
    count += (life[i][right_col] & (1 << right_bit_index)) ? 1 : 0;
    //S
    count += (life[down_row][j] & (1 << bit_index)) ? 1 : 0;
    //SW
    count += (life[down_row][left_col] & (1 << left_bit_index)) ? 1 : 0;
    //SE
    count += (life[down_row][right_col] & (1 << right_bit_index)) ? 1 : 0;

    return count;
}

uint8_t next_generation(uint8_t i, uint8_t j){
    uint8_t new_byte = 0;
    //for each bit
    for (uint8_t bit_index = 0; bit_index < 8; ++bit_index){
        //calculate number of neighbours
        uint8_t neighbours = count_neighbours( i , j , bit_index);
        //If previous generation alive
        if(life[i][j] & (1 << bit_index)){
            //..and 2 - 3 neighbours
            if ((neighbours == 2) | (neighbours == 3)){
                //live
                new_byte |= (1 << bit_index);
            } else {
                //Do nothing, already dead
            }
        //If previous generation dead
        } else {
            //And three neighbours
            if (neighbours == 3){
                //Live!!!
                new_byte |= (1 << bit_index);
            } else {
                //Do nothing, already dead
            }
        }
    }
        //Populate new_life with next generation
        return new_byte;
}

void play_god(void){
    //calculate new life 
    for ( int i = 0; i < ROW_COUNT; ++i ){
        for ( int j = 0; j < COL_COUNT; ++j ){
            new_life [ i ][ j ] = next_generation( i , j );
        }
    }
    //copy new life to life for use in next generation
    for ( int i = 0; i < ROW_COUNT; ++i ){
        for ( int j = 0; j < COL_COUNT; ++j ){
            life [ i ][ j ] = new_life [ i ][ j ];
        }
    }

}

uint32_t calculate_new_seed(){
    uint32_t _seed;
    //An extra element of randomisation - bit shift a byte using another random byte
    _seed = (new_life[1][7] << (new_life[9][7] % 21));
    //..and xor it with another byte
    _seed ^= (new_life[2][2]);
    return _seed;

}

int main() {

    init_io();
    gpio_init(25);
    gpio_init(27);
    gpio_set_dir(25, 1);
    gpio_set_dir(27, 0);
    gpio_set_input_enabled(27,1);
    gpio_pull_down(27);
    screen_init();
    
    gpio_put(COB, 1);
    for ( int i = 0; i < ROW_COUNT; ++i ){
        for ( int j = 0; j < COL_COUNT; ++j ){
            send_command(0);
        }
    }
    gpio_put(COB, 0);

    start_life(20);
    while (true) {
        sleep_ms(10);
        play_god();
        load_image(new_life);
        if(gpio_get(27)){
            start_life(calculate_new_seed());
        }
    }
    return 1;
}
