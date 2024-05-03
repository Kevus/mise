#!/bin/bash

# This script checks if a specific tool is currently available in coreutils-9.4
if [ ! -d coreutils-9.4 ]; then
    echo "Error: coreutils-9.4 folder not found. Trying to download it..."
    wget https://ftp.gnu.org/gnu/coreutils/coreutils-9.4.tar.xz
    tar -xf coreutils-9.4.tar.xz
fi

cd coreutils-9.4

# Check if the tool is available in coreutils-9.4
tools=(base64 basename cat chcon chgrp chmod chown chroot cksum comm cp csplit cut \
         date dd df dircolors dirname du echo env expand expr factor false fmt fold  \
         head hostid hostname id ginstall join kill link ln logname ls md5sum mkdir \
         mkfifo mknod mktemp mv nice nl nohup od paste pathchk pinky pr printenv printf \
         ptx pwd readlink rm rmdir runcon seq setuidgid shred shuf sleep sort split \
         stat stty sum sync tac tail tee touch tr tsort tty uname unexpand uniq unlink \
         uptime users wc whoami who yes)

echo "Preparing results file..."
echo "tool, src, lib" > ../results.csv

for i in "${tools[@]}"
do
    # Check if src/$i.c exists
    if [ ! -f src/$i.c ]; then
        echo "Error: src/$i.c not found. Looking for it..."
        # Check if lib/$i.c exists. Otherwise, exit and prompt the error
        if [ ! -f lib/$i.c ]; then
            echo "Error: lib/$i.c not found. Not available in coreutils 9.4."
            echo "$i, no, no" >> ../results.csv
        else
            echo "$i, no, yes" >> ../results.csv
        fi
    else
        echo "$i, yes, no" >> ../results.csv
    fi
done