#!/bin/bash

# This script is used to compare MISE with KLEE. SUT=coreutils
# MISE: must be in $HOME/mise/build/bin/mise
# KLEE: muse be in path

# Set up the environment
export PATH=$HOME/mise/build/bin:$PATH

# Check if coreutils-9.4 folder exists
if [ ! -d coreutils-9.4 ]; then
    echo "Error: coreutils-9.4 folder not found. Trying to download it..."
    wget https://ftp.gnu.org/gnu/coreutils/coreutils-9.4.tar.xz
    tar -xf coreutils-9.4.tar.xz
fi

cd coreutils-9.4

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
coreutils=(base64) # basename cat chcon chgrp chmod chown chroot cksum comm csplit cut
# date df dirname du env expand factor false fmt fold
# head hostid hostname id ginstall join link ln logname ls md5sum mkdir
# mkfifo mktemp mv nice nl nohup paste pinky pr printenv
# ptx pwd readlink runcon seq setuidgid shred shuf sort split
# stat stty sum sync tac tail tee tr tsort tty uname unexpand uniq unlink
# uptime users wc whoami who yes)

for i in "${coreutils[@]}"
do
    klee --simplify-sym-indices --output-module --solver-backend=z3 \
    --max-memory=1000 --disable-inlining --optimize --use-forked-solver \
    --use-cex-cache --libc=uclibc --posix-runtime \
    --external-calls=all --only-output-states-covering-new \
    --max-sym-array-size=4096 --max-solver-time=30s --max-time=4min \
    --watchdog --max-memory-inhibit=false --max-static-fork-pct=1 \
    --max-static-solve-pct=1 --max-static-cpfork-pct=1 --switch-type=internal \
    --search=random-path --search=nurs:covnew \
    --use-batching-search --batch-instructions=10000 \
    ./$i.bc --sym-args 0 1 10 --sym-args 0 2 2 --sym-files 1 8 --sym-stdin 8 --sym-stdout

    mv klee-out-0 klee-out-$i

    # Now we will run MISE on the same coreutils
    mise --simplify-sym-indices --output-module --solver-backend=z3 \
    --max-memory=1000 --disable-inlining --optimize --use-forked-solver \
    --use-cex-cache --libc=uclibc --posix-runtime \
    --external-calls=all --only-output-states-covering-new \
    --max-sym-array-size=4096 --max-solver-time=30s --max-time=4min \
    --watchdog --max-memory-inhibit=false --max-static-fork-pct=1 \
    --max-static-solve-pct=1 --max-static-cpfork-pct=1 --switch-type=internal \
    --search=random-path --search=nurs:covnew \
    --use-batching-search --batch-instructions=10000 \
    ./$i.bc --sym-args 0 1 10 --sym-args 0 2 2 --sym-files 1 8 --sym-stdin 8 --sym-stdout

    mv klee-out-0 mise-out-$i

    # Now we will generate the outputs for the tests
    cd klee-out-$i

    # Run klee-replay on each test case separately
    for j in test*.ktest
    do
        klee-replay $(readlink -f ../../../obj-gcov/src/$i) $j > klee-$j.out
    done

    cd ..
    cd mise-out-$i

    # Run klee-replay on each test case separately
    for j in test*.ktest
    do
        klee-replay $(readlink -f ../../../obj-gcov/src/$i) $j > mise-$j.out
    done

    # Now we are prepared to run mucpp
    cd ../../..
    mucpp applyall src/$i.c --

    # Now we will repeat the process for each git branch
    for j in $(git branch | cut -c 3-)
    do 
        cd obj-gcov
        make src/$i 
        cd ..

        cd obj-llvm/src
        cd klee-out-$i

        mkdir -p $HOME/mise-results/$j

        # Run klee-replay on each test case separately
        for k in test*.ktest
        do
            klee-replay $(readlink -f ../../../obj-gcov/src/$i) $k > $HOME/mise-results/$j/klee-$k.out
        done

        cd ..
        cd mise-out-$i

        # Run klee-replay on each test case separately
        for k in test*.ktest
        do
            klee-replay $(readlink -f ../../../obj-gcov/src/$i) $k > $HOME/mise-results/$j/mise-$k.out
        done

        cd ../..
    done
    

done

