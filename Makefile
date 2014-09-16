# Makefile.  Generated from Makefile.in by configure.

poppassd: poppassd.c Makefile
	gcc -g -O2 -fstack-protector-all -Wall -O3 -D_FORTIFY_SOURCE=2 poppassd.c -o poppassd -lpam 

install: poppassd
	/usr/bin/install -c -g bin -o root -m 500 poppassd ${exec_prefix}/sbin

clean:
	rm -f *.o *~* core Makefile.new Makefile.bak poppassd *.log *.status
	rm -rf ./autom4te.cache

reconf: m4
	autoreconf -i --install
