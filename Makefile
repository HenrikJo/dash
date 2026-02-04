model.o:model.c
	gcc -c model.c -lm -o model.o -Wall -Werror

test_model.o:test_model.c model.o
	gcc test_model.c model.o -lm -Wall -Werror -o test_model.o

view.o:view.c
	gcc -c view.c -Wall -Werror -o view.o

test_view.o: test_view.c view.o
	gcc test_view.c view.o -Wall -Werror -o test_view.o

test_view:test_view.o
	./test_view.o

test_model:test_model.o
	./test_model.o

run_test:test
	./test

control.o: control.c
	gcc -c control.c -lSDL2 -O2 -o control.o

host.o:host_simulation.c model.o view.o control.o
	gcc host_simulation.c model.o view.o control.o -lSDL2 -O2 -lm -Wall -Werror -o host.o

host_run:host.o
	./host.o

clean:
	gio trash model.o view.o control.o

dependencies:
	sudo apt install libsdl2-dev
