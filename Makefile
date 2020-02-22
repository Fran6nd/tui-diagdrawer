.DEFAULT_GOAL := all

ad_file.o: src/ad_file.c
	gcc -O -c src/ad_file.c -std=c99 -o ad_file.o -Iinclude
main.o: ad_file.o
	gcc -D _DEFAULT_SOURCE -O -c src/main.c -std=c99 -o main.o -Iinclude
all: clean main.o
	gcc -g main.o ad_file.o -Iinclude -lncurses -o ascii-drawer
clean:
	rm *.o
	rm ascii-drawer

#PREFIX is environment variable, but if it is not set, then set default value
ifeq ($(PREFIX),)
    PREFIX := /usr
endif

install: mz_lib.a
	install -d /usr/lib/
	install -m 644 $(DIST)/mz_lib.a /usr/lib
	install -d $(PREFIX)/include/mz_lib
	install -m 644 $(LIB_INC)/mz_lib.h $(PREFIX)/include/mz_lib
