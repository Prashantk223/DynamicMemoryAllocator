gcc -g -c mm.c -o mm.o
gcc -g -c testapp.c -o testapp.o
gcc -g mm.o testapp.o -o exe