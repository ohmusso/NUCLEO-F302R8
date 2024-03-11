# makefileを作成
$path = "test/build"
cmake -B $path -G Ninja --toolchain ./toolchain-x86.cmake

# ビルド
cmake --build $path
