mkdir -p build &>/dev/null
set -x
cd build

cmake .. -G "Ninja" -DBUILD_TESTS=OFF &&  \
    cmake --build . --config Debug && \
    ./src/save-archiver
