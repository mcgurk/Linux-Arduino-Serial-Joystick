# Linux-Arduino-Serial-Joystick

https://github.com/torvalds/linux/blob/master/drivers/input/joystick/stinger.c

## Orange Pi

UART0 "Debug TTL UART":

![Orange Pi PC](https://github.com/mcgurk/Linux-Arduino-Serial-Joystick/raw/master/Images/Orange_Pi_PC.jpg)

UART1, UART2, UART3:

![Orange Pi pinout](https://github.com/mcgurk/Linux-Arduino-Serial-Joystick/raw/master/Images/OrangePi-pinout.png)

Orange Pi, activate 3 serial ports in GPIO pins:
```
[uart0]
uart_used = 1
uart_port = 0
uart_type = 2
uart_tx = port:PA04<2><1><default><default>
uart_rx = port:PA05<2><1><default><default>

[uart1]
uart_used = 1
uart_port = 1
uart_type = 2
uart_tx = port:PG06<2><1><default><default>
uart_rx = port:PG07<2><1><default><default>

[uart2]
uart_used = 1
uart_port = 2
uart_type = 2
uart_tx = port:PA00<2><1><default><default>
uart_rx = port:PA01<2><1><default><default>

[uart3]
uart_used = 1
uart_port = 3
uart_type = 2
uart_tx = port:PA13<3><1><default><default>
uart_rx = port:PA14<3><1><default><default>
```
