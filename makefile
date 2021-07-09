OBJS = main.o usart.o ringbuffer.o dataprocessing.o client.o

iot_gate: $(OBJS)
	gcc $(OBJS) -o iot_gate -lpthread
main.o: main.c usart.h ringbuffer.h dataprocessing.h client.h
	gcc -c main.c -o main.o
usart.o: usart.c usart.h
	gcc -c usart.c -o usart.o
ringbuffer.o: ringbuffer.c ringbuffer.h
	gcc -c ringbuffer.c -o ringbuffer.o
dataprocessing.o: dataprocessing.c dataprocessing.h
	gcc -c dataprocessing.c -o dataprocessing.o
client.o: client.c client.h
	gcc -c client.c -o client.o

clean:
	rm -rf *.o iot_gate
