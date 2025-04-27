cd buxu
cp -rf ../bruter .
rm -rf bruter/.git .buxu
./build.sh --install
bpm install-from http://127.0.0.1:3000/packages io std math bit byte ffi string
./build.sh --debug-file example/hello_world.br