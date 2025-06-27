#!/bin/bash
source /opt/AMD/aocc-compiler-5.0.0/setenv_AOCC.sh
CC="clang -g -Ofast -flto -fopenmp -mavx2 -ffastlib=AMDLIBM -lamdlibmfast -march=native -lamdalloc-ext -lamdlibm -lm" bupm install-from localhost:3000/packages io math std os string dycc bit byte condition list alloc type #bsr
source install.sh

valgrind --tool=massif --stacks=yes --detailed-freq=1 --verbose  buxu $1
ms_print massif.out.* > ./massif-out.txt
rm massif.out.*

valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --log-file=./valgrind-out.txt \
    --verbose buxu $1