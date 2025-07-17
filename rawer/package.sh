name rawer
version 0.0.3
description "rawer"
# files 
setup()
{
    gcc -O3 -Wl,-rpath=/lib -o rawer main.c std.c
}