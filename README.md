# game-of-life
An implementation of Conway's Game of Life for RP Pico using a custom OLED driver

Conway's Game of Life is simulation of living organisms in an apparently endless environment. Rules are implemented to determine whether the organisms live or die on their next iteration.

The OLED display used is the SSD1306 - a 0.96 inch display using the SPI protocol.

The OLED display is initialised to draw each byte starting from the bottom left of the screen and moving up until the column is filled. The next column is then filled from bottom to top and so on.

The game of life implementation generates a seed and then pseudo-randomly fills an array of 128 by 8 bytes. This is generation 0. Generation 1 onwards follow Conway's rules and are stored in a new array which is drawn once all bytes have been calculated. The original array is then overwritten with the new array contents and the calculations begins again.

If a button press is detected on GPIO27 (with 3v3 connected to GPIO27 when button is pressed), a pseudo-random seed is generated to start the game of life again. The pseudo-random seed uses three bytes from the previous generation, which given the large combinations, is practically not-repeatable.

On turning on the MCU, the voltage on pin 26 is sampled and is used to determine the initial seed. It also is used to control the maximum population percentage of generation 0.

Possible improvements - 
The same seed is typically selected on MCU start, unless voltage is changed manually, so starting generation is also typically the same. If this voltage was unpredictable this would generate a unique generation 0 on MCU start. 
