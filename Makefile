all:sensor lector run
run:
	gcc -o run so_sig_sender.c
sensor:
	gcc -o sensor sensorSO.c -lm
lector:
	gcc -o lector thread_struct.c -lpthread -lm
clean:
	rm ./sensor ./lector ./run
