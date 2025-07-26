if [ -z "$CC" ]; then
    CC="gcc -O3"
fi

sudo rm -rf /usr/bin/bpm /usr/include/bruter.h
sudo cp ./bruter/bruter.h /usr/include/bruter.h
sudo cp ./bruter/bpm /usr/bin/bpm
