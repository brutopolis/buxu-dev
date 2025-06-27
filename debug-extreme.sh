#!/bin/bash
CC="gcc -std=c99 -pedantic -Wall -Wextra -Werror \
    -Wstrict-prototypes -Wold-style-definition \
    -Wmissing-prototypes -Wmissing-declarations \
    -Wredundant-decls -Wnested-externs \
    -Wcast-qual -Wcast-align -Wwrite-strings \
    -Wconversion -Wshadow -Wlogical-op \
    -Waggregate-return -Wstrict-overflow=5 \
    -Wswitch-default -Wswitch-enum \
    -Wunreachable-code -Wfloat-equal"

source install.sh


CC="$CC" bupm install-from localhost:3000/packages io math std os string dycc bit byte condition list alloc type #bsr

valgrind --tool=massif --stacks=yes --detailed-freq=1 --verbose  buxu $1
ms_print massif.out.* > ./massif-out.txt
rm massif.out.*

valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --log-file=./valgrind-out.txt \
    --verbose buxu $1