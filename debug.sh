cd buxu
cp -rf ../bruter .
cp -rf ../br .
rm -rf bruter/.git .buxu
./build.sh --install --debug
bupm install-from localhost:3000/packages io math std os string dycc bit byte condition list
./build.sh --debug-file ../br/example/list.br