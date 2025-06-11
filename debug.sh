cd buxu
cp -rf ../bruter .
cp -rf ../bruter-representation .
rm -rf bruter/.git .buxu
./build.sh --install --debug
bupm install-from localhost:3000/packages io math std os string dycc bit byte condition list alloc bsr
./build.sh --debug-file ../bruter-representation/example/bsr.br