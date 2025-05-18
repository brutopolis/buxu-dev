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