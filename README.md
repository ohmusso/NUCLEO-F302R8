# NUCLEO-F302R8 Blinking

Try to blink LED on the board.

## Board

<https://www.st.com/ja/evaluation-tools/nucleo-f302r8.html>

* CPU: Arm Cortex-M4. Single core.
* Internal Clock: 8MHz

### USB Cable mini B

Connect board and PC.

ex) elecom: U2C-MF10BK

## Enviroment

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

sample.bin will be created at build directory.

## Reprogramming

### Connect USB cable Between Board and PC

* Device Driver will be installed.
* Recognize drive.
* Two led(LD1, LD3) will be lamping.

![Board](/images/image-2023-09-09-Board.jpg)

### Binary Drag and Drop

![Drag and Drop](/images/image-2023-09-09-DraAndDrop.png)

LD2 will be blinking.

### copu binary to usb drive

``` powershell
Copy-Item  -Path "./build/sample.bin" -Destination "E:\"
```

## If use vscode

Add include path for lint tool.

/.vscode/c_cpp_properties.json

```json
    "configurations": [
        {
            "includePath": [
                "${workspaceFolder}/**",
                "${workspaceFolder}/build/**"
            ],            
```
