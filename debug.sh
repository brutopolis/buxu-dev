#!/bin/bash
source install.sh


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