OBJS = main.o usart.o ringbuffer.o

main: $(OBJS)
	gcc $(OBJS) -o main -lpthread
main.o: main.c usart.h ringbuffer.h
	gcc -c main.c -o main.o
usart.o: usart.c usart.h
	gcc -c usart.c -o usart.o
ringbuffer.o : ringbuffer.c ringbuffer.h
	gcc -c ringbuffer.c -o ringbuffer.o

clean:
	rm -rf *.o main
