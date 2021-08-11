CC=gcc

all: test
test:
	$(CC) -g -std=c99 ipsrvdb.c test.c -o test
clean:
	if [ -f test ];then rm -f test; fi
