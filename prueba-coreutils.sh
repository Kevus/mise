//Script .sh donde se declara una lista de 89 herramientas y, después, se ejecuta una orden específica con cada una.
//Si el programa termina con una excepción, guarda el nombre del programa junto con la cade "ERROR" en un fichero de texto.
//Si el programa termina con normalidad, guarda el nombre del programa junto con la cadena "OK" en el mismo fichero de texto.
//Junto a "ERROR", aparecerá la excepción que ha provocado el error.
//El fichero de texto se llamará "log.txt" y se guardará en el mismo directorio que el script.

/bin/bash
#Declaración de la lista de herramientas, que son [ base64 basename cat chcon chgrp chmod chown chroot cksum comm cp csplit cut date dd df dircolors dirname du echo env expand expr factor false fmt fold head hostid hostname id ginstall join kill link ln logname ls md5sum mkdir mkfifo mknod mktemp mv nice nl nohup od paste pathchk pinky pr printenv printf ptx pwd readlink rm rmdir runcon seq setuidgid shred shuf sleep sort split stat stty sum sync tac tail tee touch tr tsort tty uname unexpand uniq unlink uptime users wc whoami who yes
tools=(base64 basename cat chcon chgrp chmod chown chroot cksum comm cp csplit cut date dd df dircolors dirname du echo env expand expr factor false fmt fold head hostid hostname id ginstall join kill link ln logname ls md5sum mkdir mkfifo mknod mktemp mv nice nl nohup od paste pathchk pinky pr printenv printf ptx pwd readlink rm rmdir runcon seq setuidgid shred shuf sleep sort split stat stty sum sync tac tail tee touch tr tsort tty uname unexpand uniq unlink uptime users wc whoami who yes)

#Bucle que recorre la lista de herramientas
for tool in "${tools[@]}"; do
    #Ejecución de la orden específica
    ~/mise/build/bin/klee --simplify-sym-indices --write-cvcs --write-cov --output-module --max-memory=1000 --disable-inlining --optimize --use-forked-solver --use-cex-cache --libc=uclibc --posix-runtime --external-calls=all --only-output-states-covering-new --env-file=../../test.env --run-in-dir=/tmp/sandbox --max-sym-array-size=4096 --max-solver-time=30s --max-time=3min --watchdog --max-memory-inhibit=false --max-static-fork-pct=1 --max-static-solve-pct=1 --max-static-cpfork-pct=1 --switch-type=internal --search=random-path --search=nurs:covnew --use-batching-search --batch-instructions=10000 ./$tool.bc --sym-args 0 1 10 --sym-args 0 2 2 --sym-files 1 8 --sym-stdin 8 --sym-stdout
    #Comprobación del estado de la orden
    if [ $? -eq 0 ]; then
        #Guardado del estado en el fichero de texto
        echo "$tool OK" >> log.txt
    else
        #Guardado del estado en el fichero de texto
        echo "$tool ERROR" >> log.txt
        #Guardado de la excepción en el fichero de texto
        echo "$tool ERROR" >> log.txt
    fi
done
```
