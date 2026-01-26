model.o:model.c
	gcc -c model.c -lm -o model.o -Wall -Werror

main:main.c model.o
	gcc main.c model.o -lm -Wall -Werror -o main

run_main:main
	./main

control.o: control.c
	gcc control.c -lSDL2 -O2 -o control.o

host_run:control.o
	./control.o

clean:
	gio trash model.o control.o

dependencies:
	sudo apt install libsdl2-dev
