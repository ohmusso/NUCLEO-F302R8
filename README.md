# NUCLEO-F302R8 wifi with ESP32 WROOM

## Board

### NUCLEO-F302R8

main board.

<https://www.st.com/ja/evaluation-tools/nucleo-f302r8.html>

* CPU: Arm Cortex-M4. Single core.
* Internal Clock: 8MHz

#### USB Cable mini B

Connect board and PC.

ex) elecom: U2C-MF10BK

### ESP32 WROOM

sub board to use wifi.

#### Download AT Firmware

#### Change Baudrate

ESP32 WROOM dafult Baudrate is 115200.

But my stm sw Baudrate is 9600.

Set ESP32 WROOM Baudrate to 9600 by follow the command on serial port.

``` serial port
AT+UART_DEF=9600,8,1,0,0
```

## FreeRTOS TCP

<https://github.com/FreeRTOS/FreeRTOS-Plus-TCP/releases/tag/V4.0.0>

## Development enviroment

### OS

Windows 10

### Arm GNU Toolchain

<https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads>

### Build tool: CMake

version: 3.24.3

Download: <https://cmake.org/download/>

Note: Set system path.

ex) Path: C:\Program Files\CMake\bin

### Build tool: Ninja

version: 1.11.1

Download: <https://github.com/ninja-build/ninja/releases>

Note: downloaded ninja.exe which is zipped then unzip at c drive and set system path.

ex) Path: C:\Program Files\ninja

## Build

Execute in powershell.

```powershell
./build.ps1
```

sample.bin will be created at build directory and reprogramming.

## Reprogramming

### Connect USB cable Between Board and PC

* Device Driver will be installed.
* Recognize drive.
* Two led(LD1, LD3) will be lamping.

![Board](/images/image-2023-09-09-Board.jpg)

### Binary Drag and Drop

※ This step can be skipped

![Drag and Drop](/images/image-2023-09-09-DraAndDrop.png)

LD2 will be blinking.

### copy binary to usb drive

※ this step is included in build.ps1

``` powershell
Copy-Item  -Path "./build/sample.bin" -Destination "E:\"
```
