.DEFAULT_GOAL := all


ui.o: src/ui.c
	gcc -O -c src/ui.c -std=c99 -o ui.o -Iinclude
position.o: src/position.c
	gcc -O -c src/position.c -std=c99 -o position.o -Iinclude
position_list.o: src/position_list.c
	gcc -O -c src/position_list.c -std=c99 -o position_list.o -Iinclude
chunk.o: src/chunk.c position.o position_list.o
	gcc -O -c src/chunk.c -std=c99 -o chunk.o -Iinclude
main.o: chunk.o ui.o
	gcc -D _DEFAULT_SOURCE -O -c src/main.c -std=c99 -o main.o -Iinclude
all: main.o
	gcc -g main.o chunk.o position.o position_list.o ui.o -Iinclude -lncurses -o ascii-drawer
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
