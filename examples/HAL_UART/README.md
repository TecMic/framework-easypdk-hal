# UART Example (HAL)

In this example, a interrupt based software UART is used. At startup it prints `Hello World!` via the TX pin.
If a byte is received on the RX pin, that byte will be echoed at the TX line. If a new line character (\n) is received, the length of the line is printed.

### Hardware Circuit
By default, the LED is placed on the PA4 pin with a current sink configuration.\
The TX pin is configured as PA7 and the RX pin as PA0. The UART baud is set to 9600.

The pins, baud and the used buffer size can be configured in `include/hal_config.h`


### Compatibility
<mark>This example is only tested with the PFS154 microcontroller!

>See [easypdk-hal](https://github.com/TecMic/framework-easypdk-hal) for more info about the HAL lib.