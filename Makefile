model.o:model.c
	gcc -c model.c -lm -o model.o -Wall -Werror

view.o: view.c
	gcc view.c -o view.o -Wall -Werror

run_view: view.o
	./view.o
	ls trace

main:main.c model.o
	gcc main.c model.o -lm -Wall -Werror -o main

test:test.c model.o
	gcc test.c model.o -lm -Wall -Werror -o test

run_main:main
	./main

run_test:test
	./test

control.o: control.c
	gcc control.c -lSDL2 -O2 -o control.o

host_run:control.o
	./control.o

clean:
	gio trash model.o control.o

dependencies:
	sudo apt install libsdl2-dev
