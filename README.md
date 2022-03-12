# game-of-life
An implementation of Conway's Game of Life for RP Pico using a custom OLED driver

Conway's Game of Life is simulation of living organisms in an apparently endless environment. Rules are implemented to determine whether the organisms live or die on their next iteration.

The OLED display used is the SSD1306 - a 0.96 inch display using the SPI protocol.

The OLED display is initialised to draw each byte starting from the bottom left of the screen and moving up until the column is filled. The next column is then filled from bottom to top and so on.

The game of life implementation generates a seed (currently fixed) and then pseudo-randomly fills an array of 128 by 8 bytes. This is generation one. Generations 2 onwards follow Conway's rules and are stored in a new array which is drawn once all bytes have been calculated. The original array is then overwritten with the new array contents and the calculations begins again.

If a button press is detected on GPIO27 (with 3v3 connected to GPIO27 when button is pressed), a pseudo-random seed is generated to start the game of life again. The pseudo-random seed uses three bytes from the previous generation, which given the large combinations, is practically not-repeatable.

Possible improvements - 
The same seed is always selected on MCU start so starting generation is always the same. A possible solution to this is to perform an analogue-to-digital calculation on a pin when turned on. However, this voltage must be random too and so the question is how can it be? If a random oscillation was on the pin, this may be possible. 
