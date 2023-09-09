# arm-none-eabi-gccへのパスを一時的に追加
$env:Path = $env:Path + "C:\arm-none-eabi/bin;"

# makefileを作成
cmake -B build -G Ninja --toolchain ./toolchain-STM32F302R8.cmake

# ビルド
cmake --build build

## binファイルを作成
### ビルドでelfが作られるので、objcopyでbinに変換する
Set-Alias objcopy C:\arm-none-eabi\bin\arm-none-eabi-objcopy.exe
objcopy -O binary ./build/sample.elf ./build/sample.bin

# その他コマンド
## デバッガ gdb
### Set-Alias gdb C:\arm-none-eabi\bin\arm-none-eabi-gdb.exe
### gdb ./build/sample.elf
## バイナリダンプ
### Set-Alias objdump C:\arm-none-eabi\bin\arm-none-eabi-objdump.exe
### elfをダンプする: objdump -d .\build\sample.elf
### binをダンプする: objdump -D -b binary -m arm -M force-thumb ./build/sample.bin
