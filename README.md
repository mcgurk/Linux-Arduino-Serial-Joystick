# Linux-Arduino-Serial-Joystick

Needs serport and stinger.ko -modules.

https://github.com/torvalds/linux/blob/master/drivers/input/joystick/stinger.c

From this github-page you can download stinger-module for RetrOrangePi 2.5.2.

Put it `/lib/modules/3.4.112-sun8i/kernel/drivers/input/joystick/stinger.ko` and do `sudo depmod -a`. inputattach-command loads it automatically, so you don't have to start it by hand.

(I have not yet tested module with clean install of ROPi)

## ESP8266

Wii Classic Controller (I²C):

ESP8266: SDA = D2, SCL = D1

`sudo inputattach --stinger /dev/ttyUSB0 &`

## Arduino
### Notice, if you use 5V Arduino
Remember, Orange Pi GPIO's are 3.3V. So if you gonna connect Arduino's TX to OPi, use 3.3V Arduino or do voltage level shifting.
### Notice, if you use Arduinos USB serial port
Many Arduinos resets when serial communication starts. When Linux driver asks for initialization, Arduino doesn't answer fast enough, because it is resetting itself. Here is some ways to overcome this: http://playground.arduino.cc/Main/DisablingAutoResetOnSerialConnection
One software solution is to use socat. It creates new serialport device, keeps original port open and makes link between virtual and real port.

`sudo socat open:/dev/ttyACM0 PTY,link=/dev/ttyV0`

`sudo inputattach --stinger /dev/ttyV0 &`

Problem is that command must be given every time after Arduino is connected.

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
