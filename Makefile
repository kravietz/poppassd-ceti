BINDIR = /usr/sbin
CFLAGS = -O2
CC=gcc

poppassd: poppassd.c Makefile
	$(CC) $(CFLAGS) poppassd.c -o poppassd -lpam -ldl

install: poppassd
	install -g bin -o root -m 500 poppassd $(BINDIR)

clean:
	rm -f *.o *~* core Makefile.new Makefile.bak poppassd
