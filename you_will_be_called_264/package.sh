#!/bin/bash

debug()
{
    ../feraw/feraw.js you_will_be_called_264.feraw main.c;
    gcc main.c -o main -lminifb -lm -lGL -lX11 -g;
    valgrind     --leak-check=full     --show-leak-kinds=definite     --track-origins=yes     --log-file=./valgrind-out.txt     --verbose ./main
}

"$@"