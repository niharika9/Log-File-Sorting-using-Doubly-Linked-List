warmup1: warmup1.o my402list.o
	gcc -o warmup1 -g my402list.o warmup1.o

warmup1.o: warmup1.c my402list.h
	gcc -g -c -Wall warmup1.c

my402list.o: my402list.c warmup1.c my402list.h 
	gcc -g -c -Wall my402list.c warmup1.c 

clean:
	rm -f *.o warmup1
