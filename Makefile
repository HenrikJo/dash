dash.binblob:dash.c
	gcc dash.c -lm -o dash.binblob

run:dash.binblob
	./dash.binblob

host_simulation.binblob: host_simulation.c
	gcc host_simulation.c -lSDL2 -O2 -o host_simulation.binblob

host_run:host_simulation.binblob
	./host_simulation.binblob

clean:
	gio trash dash.binblob

dependencies:
	sudo apt install libsdl2-dev
