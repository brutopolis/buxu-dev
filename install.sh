sudo rm -rf /usr/bin/buxu /usr/bin/bupm /usr/bin/bucc /usr/include/bruter-representation.h /usr/include/bruter.h
sudo cp ./bruter-representation/bruter-representation.h /usr/include/bruter-representation.h
sudo cp ./bruter/bruter.h /usr/include/bruter.h
sudo cp ./buxu/bupm /usr/bin/bupm
sudo cp ./buxu/bucc /usr/bin/bucc
gcc -o buxu.bin ./buxu/buxu.c -ldl -g
sudo mv buxu.bin /usr/bin/buxu
