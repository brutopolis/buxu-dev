cd buxu
cp -rf ../bruter .
rm -rf bruter/.git .buxu
./build.sh --install --debug
bupm install-from http://127.0.0.1:3000/packages io math std os string dycc bit byte
./build.sh --debug-file example/loops.br