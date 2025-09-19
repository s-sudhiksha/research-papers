CC=gcc
CFLAGS=-O2 -std=c11 -Wall

SOURCES = performance_metrics.c
OBJ = $(SOURCES:.c=.o)

all: standard cache batch pool

standard: standard_linked_list.c $(SOURCES)
	$(CC) $(CFLAGS) -o standard standard_linked_list.c $(SOURCES)

cache: cache_optimized.c $(SOURCES)
	$(CC) $(CFLAGS) -o cache cache_optimized.c $(SOURCES)

batch: batch_insertion.c $(SOURCES)
	$(CC) $(CFLAGS) -o batch batch_insertion.c $(SOURCES)

pool: memory_pool.c $(SOURCES)
	$(CC) $(CFLAGS) -o pool memory_pool.c $(SOURCES)

clean:
	rm -f standard cache batch pool *.o