PACKAGE="brutopolis"
VERSION="0.0.2"

setup()
{
    rm -rf buxu bruter
    git clone https://github.com/brutopolis/bruter -b experimental
    git clone https://github.com/brutopolis/feraw # feraw
}

run()
{
    cd brutopolis
    ../feraw/feraw.js "brutopolis.feraw" "brutopolis.debug.c"
    gcc -o brutopolis.exe "brutopolis.debug.c" -O3 -g -lm -lminifb -lX11 -lGL -DBRUTER_DEFAULT_SIZE=8
    ./$FILENAME.exe
}

debug()
{
    cd brutopolis
    ../feraw/feraw.js "brutopolis.feraw" "brutopolis.debug.c"
    gcc -o brutopolis.exe "brutopolis.debug.c" -O0 -g -lm -lminifb -lX11 -lGL -DBRUTER_DEFAULT_SIZE=8
    valgrind \
    --leak-check=full \
    --show-leak-kinds=definite \
    --track-origins=yes \
    --log-file=./valgrind-out.txt \
    --verbose \
    ./brutopolis.exe
}

nelua_debug()
{
    nelua bruter.nelua

    gcc -o main ~/.cache/nelua/bruter.c -g -O0

    valgrind \
    --leak-check=full \
    --show-leak-kinds=definite \
    --track-origins=yes \
    --log-file=./valgrind-out.txt \
    --verbose \
    ./main
}

"$@"