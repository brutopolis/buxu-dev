if [ -z "$CC" ]; then
    CC="gcc -O3"
fi

sudo rm -rf /usr/bin/buxu /usr/bin/bpm /usr/include/bruter-representation.h /usr/include/bruter.h
sudo cp ./bruter-representation/bruter-representation.h /usr/include/bruter-representation.h
sudo cp ./bruter/bruter.h /usr/include/bruter.h
sudo cp ./buxu/bpm /usr/bin/bpm
$CC -o buxu.bin ./buxu/buxu.c -ldl
sudo mv buxu.bin /usr/bin/buxu
