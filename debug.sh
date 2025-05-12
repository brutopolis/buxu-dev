cd buxu
cp -rf ../bruter .
rm -rf bruter/.git .buxu
./build.sh --install --debug
bupm install-from localhost:3000/packages io math std os string dycc bit byte condition
./build.sh --debug-file example/loops.br