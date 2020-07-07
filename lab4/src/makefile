CC=gcc
CFLAGS=-I.

all : task1 task2 task3 task5

task1 :
	$(CC) -o Task1/task1 Task1/find_min_max.c Task1/utils.c Task1/parallel_min_max.c $(CFLAGS)

task2 :
	$(CC) -o Task2/zombie Task2/zombie.c $(CFLAGS)

task3 :
	$(CC) -o mem process_memory.c $(CFLAGS)

task5 :
	$(CC) -o Task5/parallel_sum.o -c Task5/parallel_sum.c -pthread
	$(CC) -o Task5/Sum.o -c Task5/Sum.c
	ar rsc Task5/libSum.a Task5/Sum.o
	$(CC) -o Task1/utils.o -c Task1/utils.c
	$(CC) -o Task5/parallel_sum Task5/parallel_sum.o -LTask5 -lSum Task1/utils.o $(CFLAGS) -pthread
clean :
	find . -name "*.o" -type f -delete
	find . -name "*.a" -type f -delete
	rm mem Task1/task1 Task2/zombie Task5/parallel_sum
