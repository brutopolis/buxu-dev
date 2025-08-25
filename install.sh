if [ -z "$CC" ]; then
    CC="gcc -O3"
fi

sudo rm -rf /usr/local/include/bruter.h /usr/local/include/feraw.h
sudo cp ./bruter/bruter.h /usr/include/bruter.h
