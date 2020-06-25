CC=gcc
CFLAGS=-I.

task1:
	$(CC) -o Task1/task1 Task1/find_min_max.c Task1/utils.c Task1/parallel_min_max.c $(CFLAGS)

task3:
	$(CC) -o mem process_memory.c $(CFLAGS)

task5:
	$(CC) -o Task5/parallel_sum.o -c Task5/parallel_sum.c -pthread
	$(CC) -o Task5/Sum.o -c Task5/Sum.c
	ar rsc Task5/libSum.a Task5/Sum.o
	$(CC) -o Task1/utils.o -c Task1/utils.c
	$(CC) -o Task5/parallel_sum Task5/parallel_sum.o -LTask5 -lSum Task1/utils.o $(CFLAGS) -pthread