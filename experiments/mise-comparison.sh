#!/bin/bash

# This script is used to compare MISE with KLEE. SUT=coreutils
# MISE: must be in $HOME/mise/build/bin/mise
# KLEE: muse be in path

# Check if coreutils-9.4 folder exists
if [ ! -d coreutils-9.4 ]; then
    echo "Error: coreutils-9.4 folder not found. Trying to download it..."
    wget https://ftp.gnu.org/gnu/coreutils/coreutils-9.4.tar.xz
    tar -xf coreutils-9.4.tar.xz
fi

cd coreutils-9.4
mucpp clean
echo "ROR" > list_operators.txt

# Now we will compile everything according to KLEE experiments
mkdir obj-gcov
cd obj-gcov
../configure --disable-nls CFLAGS="-g -fprofile-arcs -ftest-coverage"
make
cd ..

mkdir obj-llvm
cd obj-llvm
export LLVM_COMPILER=clang
CC=wllvm ../configure --disable-nls CFLAGS="-g -O1 -Xclang -disable-llvm-passes -D__NO_STRING_INLINES  -D_FORTIFY_SOURCE=0 -U__OPTIMIZE__"
make
cd src
find . -executable -type f | xargs -I '{}' extract-bc '{}'

# Now we will run KLEE on the coreutils received as an argument
tools=(base64) # basename cat chcon chgrp chmod chown chroot cksum comm cp csplit cut \
        # date dd df dircolors dirname du echo env expand expr factor false fmt fold  \
        # head hostid hostname id join link ln logname ls mkdir \
        # mkfifo mknod mktemp mv nice nl nohup od paste pathchk pinky pr printenv printf \
        # ptx pwd readlink rm rmdir runcon seq shred shuf sleep sort split \
        # stat stty sum sync tac tail tee touch tr tsort tty uname unexpand uniq unlink \
        # uptime users wc whoami who yes)

for i in "${coreutils[@]}"
do
    klee --simplify-sym-indices --output-module --solver-backend=z3 \
    --max-memory=1000 --disable-inlining --optimize --use-forked-solver \
    --use-cex-cache --libc=uclibc --posix-runtime \
    --external-calls=all --only-output-states-covering-new \
    --max-sym-array-size=4096 --max-solver-time=30s --max-time=3min \
    --watchdog --max-memory-inhibit=false --max-static-fork-pct=1 \
    --max-static-solve-pct=1 --max-static-cpfork-pct=1 --switch-type=internal \
    --search=random-path --search=nurs:covnew \
    --use-batching-search --batch-instructions=10000 \
    ./$i.bc --sym-args 0 1 10 --sym-args 0 2 2 --sym-files 1 8 --sym-stdin 8

    mv klee-out-0 klee-out-$i

    # Now we will run MISE on the same coreutils
    mise --simplify-sym-indices --output-module --solver-backend=z3 \
    --max-memory=1000 --disable-inlining --optimize --use-forked-solver \
    --use-cex-cache --libc=uclibc --posix-runtime \
    --external-calls=all --only-output-states-covering-new \
    --max-sym-array-size=4096 --max-solver-time=30s --max-time=3min \
    --watchdog --max-memory-inhibit=false --max-static-fork-pct=1 \
    --max-static-solve-pct=1 --max-static-cpfork-pct=1 --switch-type=internal \
    --search=random-path --search=nurs:covnew \
    --use-batching-search --batch-instructions=10000 \
    --mutations-file=../../list_operators.txt \
    ./$i.bc --sym-args 0 1 10 --sym-args 0 2 2 --sym-files 1 8 --sym-stdin 8

    mv klee-out-0 mise-out-$i

    # Now we will generate the outputs for the tests
    cd klee-out-$i

    # Run klee-replay on each test case separately
    for j in test*.ktest
    do
        timeout 60 klee-replay $(readlink -f ../../../obj-gcov/src/$i) $j >> klee-$i.out
    done

    cd ..
    cd mise-out-$i
    mise-dcleaner .

    # Run klee-replay on each test case separately
    for j in test*.ktest
    do
        timeout 60 klee-replay $(readlink -f ../../../obj-gcov/src/$i) $j >> mise-$i.out
    done

    # Now we are prepared to run mucpp
    cd ../../..

    # Check if src/$i.c exists
    if [ ! -f src/$i.c ]; then
        echo "Error: src/$i.c not found. Looking for it..."
        # Check if lib/$i.c exists. Otherwise, exit and prompt the error
        if [ ! -f lib/$i.c ]; then
            echo "Error: lib/$i.c not found. Exiting..."
            exit 1
        else
            mucpp applyall lib/$i.c --
        fi
    else
        mucpp applyall src/$i.c --       
    fi

    # Now we will repeat the process for each git branch
    for j in $(git branch | cut -c 3-)
    do 
    	git checkout $j
    	
        cd obj-gcov
        make src/$i 
        cd ..

        cd obj-llvm/src
        cd klee-out-$i

        mkdir -p $HOME/mise-results/$j

        # Run klee-replay on each test case separately
        for k in test*.ktest
        do
            timeout 60 klee-replay $(readlink -f ../../../obj-gcov/src/$i) $k >> $HOME/mise-results/$j/klee-$i.out
        done

        cd ..
        cd mise-out-$i

        # Run klee-replay on each test case separately
        for k in test*.ktest
        do
            timeout 60 klee-replay $(readlink -f ../../../obj-gcov/src/$i) $k >> $HOME/mise-results/$j/mise-$i.out
        done

        cd ../../..
    done
    

done

