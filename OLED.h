#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "pico/stdlib.h"
#include "hardware/spi.h"

#define COB 20 //Data / Command (bar)
#define SDA 19 //Data out
#define SCK 18 //Clock
#define CSB 17 // Chip Select (bar)
#define RST 16 // Reset (bar)

#define SPI_PORT spi0

void send_command(uint8_t command);

void send_commands(const uint8_t *commands);

void set_charge_pump(bool value);

enum display_mode {
    normal = 0xA4,
    test   = 0xA5
};

void set_display_mode(enum display_mode display_mode);

void set_display_on(bool value);

void set_inversion_mode(bool value);

void set_contrast( uint8_t contrast );

void reset_contrast(void);

void set_vertical_mode();

enum memory_addressing_mode{
    horizontal,
    vertical,
    page_Addressing
};

void set_memory_addressing_mode(enum memory_addressing_mode mode);

void set_lower_page_addressing_column(uint8_t column);
void set_higher_page_addressing_column(uint8_t column);

void set_column_address(uint8_t start_address, uint8_t end_address);

void set_page_address(uint8_t start_address, uint8_t end_address);

void set_display_start_line(uint8_t start_line_register);

void remap_segment(bool value);

enum multiplex_ratio {
    M_16 = 0x0F,
    M_32 = 0x1F,
    M_64 = 0x3F
};

void set_multiplex_ratio(uint8_t ratio);

enum output_scan_direction {
    regular   = 0xC0,
    remapped = 0xC8
};

void remap_output_scan_direction(bool remap);

void set_display_offset(uint8_t offset);


void set_com_pins(bool sequential, bool lr_remap );

void set_display_clock(uint8_t divide_ratio, uint8_t frequency);

void set_pre_charge_period(uint8_t phase_1, uint8_t phase_2);

enum deselect_level{
    VCC_0_65 = 0x00,
    VCC_0_77 = 0x20,
    VCC_0_83 = 0x30
};

void set_Vcomh_deselect_level(enum deselect_level level);

void nop();

void set_scroll_status(bool value);

void clear_display_on_init();

void screen_init(void);

void init_io();

void load_image(unsigned char image[128][8]);

#ifdef __cplusplus
}
#endif