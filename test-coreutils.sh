#base64 basename cat chcon chgrp chmod chown chroot cksum comm cp csplit cut
#date dd df dircolors dirname du echo env expand expr factor false fmt fold
#head hostid hostname id ginstall join kill link ln logname ls md5sum mkdir
#mkfifo mknod mktemp mv nice nl nohup od paste pathchk pinky pr printenv printf
#ptx pwd readlink rm rmdir runcon seq setuidgid shred shuf sleep sort split
#stat stty sum sync tac tail tee touch tr tsort tty uname unexpand uniq unlink
#uptime users wc whoami who yes

# Store the coreutils package utilities in an array for latter use
coreutils=(base64 basename cat chcon chgrp chmod chown chroot cksum comm csplit cut
date df dirname du env expand factor false fmt fold
head hostid hostname id ginstall join link ln logname ls md5sum mkdir
mkfifo mktemp mv nice nl nohup paste pinky pr printenv
ptx pwd readlink runcon seq setuidgid shred shuf sort split
stat stty sum sync tac tail tee tr tsort tty uname unexpand uniq unlink
uptime users wc whoami who yes)

# Enter obj-llvm/src
cd obj-llvm/src

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
done

# Some utilities have special treatment

#    dd: --sym-args 0 3 10 --sym-files 1 8 --sym-stdin 8 --sym-stdout
#    dircolors: --sym-args 0 3 10 --sym-files 2 12 --sym-stdin 12 --sym-stdout
#    echo: --sym-args 0 4 300 --sym-files 2 30 --sym-stdin 30 --sym-stdout
#    expr: --sym-args 0 1 10 --sym-args 0 3 2 --sym-stdout
#    mknod: --sym-args 0 1 10 --sym-args 0 3 2 --sym-files 1 8 --sym-stdin 8 --sym-stdout
#    od: --sym-args 0 3 10 --sym-files 2 12 --sym-stdin 12 --sym-stdout
#    pathchk: --sym-args 0 1 2 --sym-args 0 1 300 --sym-files 1 8 --sym-stdin 8 --sym-stdout
#    printf: --sym-args 0 3 10 --sym-files 2 12 --sym-stdin 12 --sym-stdout

klee --simplify-sym-indices --output-module --solver-backend=z3 \
--max-memory=1000 --disable-inlining --optimize --use-forked-solver \
--use-cex-cache --libc=uclibc --posix-runtime \
--external-calls=all --only-output-states-covering-new \
--max-sym-array-size=4096 --max-solver-time=30s --max-time=4min \
--watchdog --max-memory-inhibit=false --max-static-fork-pct=1 \
--max-static-solve-pct=1 --max-static-cpfork-pct=1 --switch-type=internal \
--search=random-path --search=nurs:covnew \
--use-batching-search --batch-instructions=10000 \
./dd.bc --sym-args 0 3 10 --sym-files 1 8 --sym-stdin 8 --sym-stdout

mv klee-out-0 klee-out-dd

klee --simplify-sym-indices --output-module --solver-backend=z3 \
--max-memory=1000 --disable-inlining --optimize --use-forked-solver \
--use-cex-cache --libc=uclibc --posix-runtime \
--external-calls=all --only-output-states-covering-new \
--max-sym-array-size=4096 --max-solver-time=30s --max-time=4min \
--watchdog --max-memory-inhibit=false --max-static-fork-pct=1 \
--max-static-solve-pct=1 --max-static-cpfork-pct=1 --switch-type=internal \
--search=random-path --search=nurs:covnew \
--use-batching-search --batch-instructions=10000 \
./dircolors.bc --sym-args 0 3 10 --sym-files 2 12 --sym-stdin 12 --sym-stdout

mv klee-out-0 klee-out-dircolors

klee --simplify-sym-indices --output-module --solver-backend=z3 \
--max-memory=1000 --disable-inlining --optimize --use-forked-solver \
--use-cex-cache --libc=uclibc --posix-runtime \
--external-calls=all --only-output-states-covering-new \
--max-sym-array-size=4096 --max-solver-time=30s --max-time=4min \
--watchdog --max-memory-inhibit=false --max-static-fork-pct=1 \
--max-static-solve-pct=1 --max-static-cpfork-pct=1 --switch-type=internal \
--search=random-path --search=nurs:covnew \
--use-batching-search --batch-instructions=10000 \
./echo.bc --sym-args 0 4 300 --sym-files 2 30 --sym-stdin 30 --sym-stdout

mv klee-out-0 klee-out-echo

klee --simplify-sym-indices --output-module --solver-backend=z3 \
--max-memory=1000 --disable-inlining --optimize --use-forked-solver \
--use-cex-cache --libc=uclibc --posix-runtime \
--external-calls=all --only-output-states-covering-new \
--max-sym-array-size=4096 --max-solver-time=30s --max-time=4min \
--watchdog --max-memory-inhibit=false --max-static-fork-pct=1 \
--max-static-solve-pct=1 --max-static-cpfork-pct=1 --switch-type=internal \
--search=random-path --search=nurs:covnew \
--use-batching-search --batch-instructions=10000 \
./expr.bc --sym-args 0 1 10 --sym-args 0 3 2 --sym-stdout

mv klee-out-0 klee-out-expr

klee --simplify-sym-indices --output-module --solver-backend=z3 \
--max-memory=1000 --disable-inlining --optimize --use-forked-solver \
--use-cex-cache --libc=uclibc --posix-runtime \
--external-calls=all --only-output-states-covering-new \
--max-sym-array-size=4096 --max-solver-time=30s --max-time=4min \
--watchdog --max-memory-inhibit=false --max-static-fork-pct=1 \
--max-static-solve-pct=1 --max-static-cpfork-pct=1 --switch-type=internal \
--search=random-path --search=nurs:covnew \
--use-batching-search --batch-instructions=10000 \
./mknod.bc --sym-args 0 1 10 --sym-args 0 3 2 --sym-files 1 8 --sym-stdin 8 --sym-stdout

mv klee-out-0 klee-out-mknod

klee --simplify-sym-indices --output-module --solver-backend=z3 \
--max-memory=1000 --disable-inlining --optimize --use-forked-solver \
--use-cex-cache --libc=uclibc --posix-runtime \
--external-calls=all --only-output-states-covering-new \
--max-sym-array-size=4096 --max-solver-time=30s --max-time=4min \
--watchdog --max-memory-inhibit=false --max-static-fork-pct=1 \
--max-static-solve-pct=1 --max-static-cpfork-pct=1 --switch-type=internal \
--search=random-path --search=nurs:covnew \
--use-batching-search --batch-instructions=10000 \
./od.bc --sym-args 0 3 10 --sym-files 2 12 --sym-stdin 12 --sym-stdout

mv klee-out-0 klee-out-od

klee --simplify-sym-indices --output-module --solver-backend=z3 \
--max-memory=1000 --disable-inlining --optimize --use-forked-solver \
--use-cex-cache --libc=uclibc --posix-runtime \
--external-calls=all --only-output-states-covering-new \
--max-sym-array-size=4096 --max-solver-time=30s --max-time=4min \
--watchdog --max-memory-inhibit=false --max-static-fork-pct=1 \
--max-static-solve-pct=1 --max-static-cpfork-pct=1 --switch-type=internal \
--search=random-path --search=nurs:covnew \
--use-batching-search --batch-instructions=10000 \
./pathchk.bc --sym-args 0 1 2 --sym-args 0 1 300 --sym-files 1 8 --sym-stdin 8 --sym-stdout

mv klee-out-0 klee-out-pathchk

klee --simplify-sym-indices --output-module --solver-backend=z3 \
--max-memory=1000 --disable-inlining --optimize --use-forked-solver \
--use-cex-cache --libc=uclibc --posix-runtime \
--external-calls=all --only-output-states-covering-new \
--max-sym-array-size=4096 --max-solver-time=30s --max-time=4min \
--watchdog --max-memory-inhibit=false --max-static-fork-pct=1 \
--max-static-solve-pct=1 --max-static-cpfork-pct=1 --switch-type=internal \
--search=random-path --search=nurs:covnew \
--use-batching-search --batch-instructions=10000 \
./printf.bc --sym-args 0 3 10 --sym-files 2 12 --sym-stdin 12 --sym-stdout

mv klee-out-0 klee-out-printf

