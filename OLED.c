#include <stdio.h>
#include "OLED.h"

/*! \brief Write one byte to SPI device.
* Writes one byte to the SPI device on Port 0.
* This is a wrapper for the spi_write_blocking function of the Pico SPI library
* \param command A single byte command/data packet
* \return None
*/
void send_command(uint8_t command){
    spi_write_blocking(SPI_PORT, &command, 1);
}
/*! \brief Write multiple bytes to SPI device.
* Writes multiple bytes to the SPI device on Port 0.
* This is a wrapper for the spi_write_blocking function of the Pico SPI library
* \param commands An array of bytes to be sent as command/data packet
* \return None
*/
void send_commands(const uint8_t *commands){
    spi_write_blocking(SPI_PORT, commands, sizeof(commands) / 1);
}

void set_charge_pump(bool value){
    uint8_t commands[2] = {0x8D, 0x10 | ( value << 2)};
    send_commands(commands);
}
/*! \brief Sets the display mode
* Options are to read from RAM or just turn all pixels on.
* \param display_mode normal/test
* \return None
*/
void set_display_mode(enum display_mode display_mode){
    send_command(display_mode);
}
/*! \brief Sets the display on or off
* \param value 0 - off / 1 - on
* \return None
*/
void set_display_on(bool value){
    send_command(0xAE | value);
}
/*! \brief Determines if a 'set' pixel is lit or unlit.
* Inversion mode may use more power if few pixels are 'set'
* \param value 0 - off (white is set) / 1 - on (black is set)
* \return None
*/
void set_inversion_mode(bool value){
    send_command(0xA6 | value);
}

void set_contrast( uint8_t contrast ){
    uint8_t commands[2] = {0x81, contrast};
    send_commands(commands);
}

void reset_contrast(void){
    uint8_t commands[2] = {0x81, 0x7F};
    send_commands(commands);
}
void set_vertical_mode(){
    send_command(0xD3);
}

void set_memory_addressing_mode(enum memory_addressing_mode mode){
    uint8_t commands[2] = { 0x20 , mode };
    send_commands(commands);
}

void set_lower_page_addressing_column(uint8_t column){
    uint8_t lower = ( column && 0xF );
    send_command(lower);

}
void set_higher_page_addressing_column(uint8_t column){
    uint8_t higher = (( column && 0xF ) | 0x10 );
    send_command(higher);
}

void set_column_address(uint8_t start_address, uint8_t end_address){
    uint8_t commands[3] = { 0x21, start_address & 0x74, end_address & 0x7F };
    send_commands(commands);
}

void set_page_address(uint8_t start_address, uint8_t end_address){
    uint8_t commands[3] = { 0x22, start_address & 0x7, end_address & 0x7 };
    send_commands(commands);
}

void set_display_start_line(uint8_t start_line_register){
    //Max of register of 63
    uint8_t command = (( start_line_register & 0x3F ) | 0x80 );
    send_command(command);
}

void remap_segment(bool value){
    send_command(0xA0 | value);
}

void set_multiplex_ratio(uint8_t ratio){
    uint8_t commands[2] = { 0xA8 , ratio };
    send_commands(commands);
}

void remap_output_scan_direction(bool remap){    
    send_command(0xC0 | (remap ? 8 : 0));
}

void set_display_offset(uint8_t offset){
    uint8_t commands[2] = { 0xD3 , offset & 0x3F };
    send_commands(commands);
}

void set_com_pins(bool sequential, bool lr_remap ){
    uint8_t commands[2] = { 0xDA };
    commands[1] = (( sequential << 4 ) | ( lr_remap << 5 ));
    send_commands(commands);
}

void set_display_clock(uint8_t divide_ratio, uint8_t frequency){
    //Max values are 16 
    divide_ratio &= 0xF;
    frequency     = (frequency << 4);
    uint8_t commands[2] = { 0xD5 };
    commands[1] = ( divide_ratio | frequency );
    send_commands(commands);
}

void set_pre_charge_period(uint8_t phase_1, uint8_t phase_2){
    phase_1 &= 0xF;
    phase_2  = ( phase_2 << 4 );
    uint8_t commands[2] = { 0xD9 };
    commands[1] = ( phase_1 | phase_2 );
    send_commands(commands);
}


void set_Vcomh_deselect_level(enum deselect_level level){
    uint8_t commands[2] = { 0xDB , level};
    send_commands(commands);
}
/*! \brief Wastes time - N OPeration
* \return None
*/
void nop(){
    send_command(0xE3);
}

void set_scroll_status(bool value){
    if (value){
        nop();
    }
    else{
        send_command(0x2E);
    }
}
/*! \brief Wipes the screen. 
* Outcome will vary based on whether inversion mode is set.
* \return None
*/
void clear_display_on_init(){
    for (uint8_t i = 0; i < 128; ++i){
        for (uint8_t j = 0; j < 64; ++j){

            send_command(0x00);
        }
        
    }
}
/*! \brief Runs initialisation functions for screen. 
* Typical changes will be to memory addressing mode, segment remapping,
* and output scan direction. \n
* Default is to write each column from bottom to top, starting with left-most column.
* Column will be automatically incremented after each write until all columns are filled.
* \return None
*/
void screen_init(void){
    //Wait until screen is stabilised and then bring online
    gpio_put(RST, 0);
    sleep_ms(100);
    gpio_put(RST, 1);
    sleep_ms(100);

    set_display_on(0); //default
    set_display_clock(0x00, 0x80); //not default
    set_multiplex_ratio(M_64); //default
    set_display_offset(0x00); //default
    set_display_start_line(0x00); //default
    set_charge_pump(1); //not default
    
    //load in image rotates 90 clockwise to produce displayed image

    set_memory_addressing_mode(vertical); // not default
    remap_segment(1); // not default
    remap_output_scan_direction(0); // not default
    set_com_pins(1,0); //default 1,0



    set_contrast(0xCF); //not default
    set_pre_charge_period(0x1, 0xF); //not default
    set_Vcomh_deselect_level(0x30); //not default
    set_display_mode(normal); //default
    set_scroll_status(0); //default
    set_display_on(1); //not default

}
/*! \brief Runs initialisation functions for PICO. 
* Use SPI_PORT_0 and write out at 8MHz. Reset, Chip Select and DC pins will be controlled by the user, not the SPI.
* \return None
*/
void init_io(){

    //Initialise GPIOs
    gpio_init_mask( (1 << COB) | (1 << SCK) | (1 << SDA) | (1 << CSB) | (1 << RST) );
    //Set manually controlled (non-SPI) GPIOs as output
    gpio_set_dir_masked( (1 << COB) | (1 << CSB) | (1 << RST), (1 << COB) | (1 << CSB) | (1 << RST) );

    //Set all manually controlled as off - command, chip selected, reset as all are active low
    gpio_put_masked( (1 << COB) | (1 << CSB) | (1 << RST), 0 );

    //Initialise SPI at 8 mhz
    spi_init(SPI_PORT, 8000000);

    //Set serial clock and data (mosi) as SPI-controlled
    gpio_set_function(SDA, GPIO_FUNC_SPI);
    gpio_set_function(SCK, GPIO_FUNC_SPI);
    
    sleep_ms(1);
    clear_display_on_init();
}
/*! \brief Loads a 64w * 128h image to the screen.
* Image is stored as a list of hex values, 8 bytes wide, 128 lines long. This 
* means the image is flipped 90 in software for each of debugging but writes back as
* correct orientation.
* \param image Image to load as 128 x 8 2d array
* \return None
*/
void load_image(unsigned char img[128][8]){

    gpio_put(COB, 1);

    for (uint8_t i = 0; i < 128; ++i){

        for (uint8_t j = 0; j < 8; ++j){
        
            send_command(img[i][j]);

        }
    }
    gpio_put(COB, 0);

}


    

