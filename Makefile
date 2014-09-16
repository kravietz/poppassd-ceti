# Makefile.  Generated from Makefile.in by configure.

BINDIR = /usr/sbin

poppassd: poppassd.c Makefile
	gcc -g -O2 -fstack-protector-all -Wall -O3 -D_FORTIFY_SOURCE=2 poppassd.c -o poppassd -lpam 

install: poppassd
	install -g bin -o root -m 500 poppassd $(BINDIR)

clean:
	rm -f *.o *~* core Makefile.new Makefile.bak poppassd

m4:
	autoreconf -i --install

