#!/bin/bash
sudo rm -rf /usr/bin/buxu /usr/bin/bupm /usr/bin/bucc /usr/include/bruter-representation.h /usr/include/bruter.h
sudo cp ./bruter-representation/bruter-representation.h /usr/include/bruter-representation.h
sudo cp ./bruter/bruter.h /usr/include/bruter.h
sudo cp ./buxu/bupm /usr/bin/bupm
sudo cp ./buxu/bucc /usr/bin/bucc
gcc -o buxu.bin ./buxu/buxu.c -ldl -g
sudo mv buxu.bin /usr/bin/buxu


bupm install-from localhost:3000/packages io math std os string dycc bit byte condition list alloc type #bsr

valgrind --tool=massif --stacks=yes --detailed-freq=1 --verbose  buxu $1
ms_print massif.out.* > ./massif-out.txt
rm massif.out.*

valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --log-file=./valgrind-out.txt \
    --verbose buxu $1