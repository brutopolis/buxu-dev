name republicavelha
version 0.0.1
description "a blocky game"
files terrain.c 

build()
{
    gcc -O3 -fPIC -shared terrain.c -o terrain.brl -lm -lX11 -lGL -lminifb -I./include -L./lib
}

setup()
{
    rm -rf minifb
    rm -rf lib
    git clone https://github.com/emoon/minifb
    mkdir -p lib include
    cd minifb
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    make
    cd ..
    cp build/libminifb.a ../lib/
    cp include/*.h ../include/
}

run()
{
    source build.sh
    buxu --eval "load {$(pwd)/terrain.brl}"
}