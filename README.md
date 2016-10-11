# Linux-Arduino-Serial-Joystick

### Short version 
Convert (game)controller signals with Arduino or ESP8266 to Serial TTL -signal and use it in Linux like any other joystick (/dev/input/jsX).

### Longer version
There can be situations, when you cannot do or use USB-HID-joystick adapter and you still need way to use Linux with some kind of (game)controller.

Orange Pi has 4 hardware serial ports (UART), which are rarely used, so one reason is simply to save USB-port(s).

This page is for information how to connect some controllers to serial port. You will get /dev/input/jsX-device (joysticks and gamepads) or /dev/eventX-device (keyboards).


## Software

### Arduino IDE

Stinger_WiiClassicController.ino emulates Gravis Stinger controller. It reads 6 data bytes from Wii Classic Controller with I²C. Then it converts them to Gravis Stinger 4 data bytes and sends them through serial port (physical or USB). Linux are set up like there would be real Stinger connected to physical serial port. (analog sticks of Wii Classic Controller are disabled)

### RetrOrangePi 2.5.2

This needs serport.ko and stinger.ko -modules. Serport is compiled to kernel (CONFIG_SERIO_SERPORT=y). From this github-page you can download stinger-module for RetrOrangePi 2.5.2. Put it `/lib/modules/3.4.112-sun8i/kernel/drivers/input/joystick/stinger.ko` and do `sudo depmod -a`. inputattach-command loads it automatically, so you don't have to start it by hand.

(I have not yet tested module with clean install of ROPi)

Install inputattach:

`sudo apt install inputattach`

Joystick-device is created with command:

`sudo inputattach --stinger /dev/ttyUSB0 &`

You can test it with (device number depends your another controllers):

`jstest /dev/js0`


## Hardware

### ESP8266 (recommended)

Wii Classic Controller (I²C):

ESP8266: SDA = D2 (GPIO4), SCL = D1 (GPIO5)

ESP8266, Wii Classic Controller and Orange Pi GPIO's are 3.3V, so there is no need for voltage level shifting.

#### ESP8266 with Arduino IDE
Instructions to add ESP8266-support to Arduino IDE:

https://github.com/esp8266/Arduino

Short version:

1. File -> Preferences -> Additional Boards Manager URLs: http://arduino.esp8266.com/stable/package_esp8266com_index.json

2. Tools -> Board -> Boards manager... -> esp8266 ->  Install

### Arduino

Wii Classic Controller (I²C) (3.3V!):

Uno: SDA = A4, SCL = A5

Pro Micro: SDA = D2, SCL = D3

#### Notice, if you use 5V Arduino
Remember, Orange Pi GPIO's are 3.3V. So if you gonna connect Arduino's TX to OPi, use 3.3V Arduino or do voltage level shifting. Wii Classic Controller is also 3.3V.

#### Notice, if you use Arduino USB serial port
Many Arduinos resets when serial communication starts. When Linux driver asks for initialization, Arduino doesn't answer fast enough, because it is resetting itself. Here is some ways to overcome this: http://playground.arduino.cc/Main/DisablingAutoResetOnSerialConnection

One software solution is to use socat. It creates new serialport device, keeps original port open and makes link between virtual and real port.

`sudo socat open:/dev/ttyACM0 PTY,link=/dev/ttyV0`

`sudo inputattach --stinger /dev/ttyV0 &`

Command must be given every time after Arduino is connected.

### Orange Pi

Orange Pi has 4 hardware serialports.

UART0 "Debug TTL UART" (may be in debug/terminal use):

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

## inputattach

For testing purposes you can start inputattach with dump-device:

`inputattach --dump /dev/ttyUSB0`

It prints out hexadecimal numbers and ascii characters of serial device to console.

## Links

https://github.com/torvalds/linux/blob/master/drivers/input/joystick/stinger.c

https://github.com/torvalds/linux/blob/master/drivers/input/joystick

https://www.kernel.org/doc/Documentation/input/joystick.txt

https://sourceforge.net/p/linuxconsole/code/ci/master/tree/utils/inputattach.c


## Linux serial tricks
Get some programs:

`sudo apt get install socat inputattach minicom`

(minicom exit: ctrl-a, x, enter)

### Create two virtual serial ports linked together and test them with two terminal
```
sudo socat PTY,link=/dev/ttyS10 PTY,link=/dev/ttyS11 &
sudo chmod 666 /dev/ttyS1?
minicom -D /dev/ttyS10
with other cosole/terminal/window: minicom -D /dev/ttyS11
```
### Check what some serial device sends (e.g. for initialization)
```
inputattach --dump /dev/ttyS0
```
### Redirect existing serial port to new virtual serial port
```
sudo socat open:/dev/ttyACM0 PTY,link=/dev/ttyV0
```
### Send PS2 keyboard codes to computer through terminal
```
sudo socat PTY,link=/dev/ttyS10 PTY,link=/dev/ttyS11 &
sudo chmod 666 /dev/ttyS1?
sudo inputattach --ps2serkbd /dev/ttyS10 &
minicom -D /dev/ttyS11
```
(careful! if you send example only an 'A' (0x1C), it repeats forewer until you send breakcode (0xF01C) or kill inputattach)

http://www.computer-engineering.org/ps2keyboard/scancodes2.html
