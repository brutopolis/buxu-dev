if [ -z "$CC" ]; then
    CC="gcc -O3"
fi

sudo rm -rf /usr/bin/buxu /usr/bin/bupm /usr/bin/bucc /usr/include/bruter-representation.h /usr/include/bruter.h
sudo cp ./bruter-representation/bruter-representation.h /usr/include/bruter-representation.h
sudo cp ./bruter/bruter.h /usr/include/bruter.h
sudo cp ./buxu/bupm /usr/bin/bupm
sudo cp ./buxu/bucc /usr/bin/bucc
$CC -o buxu.bin ./buxu/buxu.c -ldl
sudo mv buxu.bin /usr/bin/buxu
