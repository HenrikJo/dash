vehicle.binblob:vehicle.c
	gcc vehicle.c -lm -o vehicle.binblob

run:vehicle.binblob
	./vehicle.binblob

host_simulation.binblob: host_simulation.c
	gcc host_simulation.c -lSDL2 -O2 -o host_simulation.binblob

host_run:host_simulation.binblob
	./host_simulation.binblob

clean:
	gio trash vehicle.binblob host_simulation.binblob

dependencies:
	sudo apt install libsdl2-dev
