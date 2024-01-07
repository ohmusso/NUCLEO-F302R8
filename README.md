# NUCLEO-F302R8 BLDC 6Step

<iframe width="560" height="315" src="https://www.youtube.com/embed/A2dFJH2sC58?si=GxkFvOtkD9a5N7TP" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>

## Board

<https://www.st.com/ja/evaluation-tools/nucleo-f302r8.html>

* CPU: Arm Cortex-M4. Single core.
* Internal Clock: 8MHz

### X-NUCLEO-IHM07M1

NUCLEO board extension of motor driver.

<https://www.st.com/ja/ecosystems/x-nucleo-ihm07m1.html>

#### L6230

Motor driver.

<https://www.st.com/en/motor-drivers/l6230.html?rt=um&id=UM1943>

### USB Cable mini B

Connect board and PC.

ex) elecom: U2C-MF10BK

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
