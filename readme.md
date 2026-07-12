# (Cascaded) Lithium-Ion Battery Charger 


The goals is a modular design for charging a lithium ion battery.  The board has two parts, the actual charging circuitry and the communication peripherals.



The communication peripherals includes an electrically isolated UART interface over a pulse transformer, an I2C interface to talk to the board's DAC module, and some general purpose inputs and outputs. 

Power Supply: 12V

V1 had a few issues, one was a layout error with the PIC microcontroller (I created the symbols itself in KiCad).  The other issue, much more interesting, is an issue with stability in the constant current controller.


