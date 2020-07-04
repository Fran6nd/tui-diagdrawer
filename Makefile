.DEFAULT_GOAL := ascii-diagdrawer

show_keyname.o: src/show_keyname.c
	gcc -O -c src/show_keyname.c -std=c99 -o show_keyname.o -Incurses
show_keyname: show_keyname.o
	gcc -g show_keyname.o -Iinclude -lncurses -o show_keyname
ui.o: src/ui.c
	gcc -O -c src/ui.c -std=c99 -o ui.o -Iinclude
undo_redo.o: src/undo_redo.c
	gcc -O -c src/undo_redo.c -std=c99 -o undo_redo.o -Iinclude
position.o: src/position.c
	gcc -O -c src/position.c -std=c99 -o position.o -Iinclude
position_list.o: src/position_list.c
	gcc -O -c src/position_list.c -std=c99 -o position_list.o -Iinclude
chunk.o: src/chunk.c position.o position_list.o
	gcc -O -c src/chunk.c -std=c99 -o chunk.o -Iinclude
edit_mode.o: src/edit_mode.c
	gcc -O -c src/edit_mode.c -std=c99 -o edit_mode.o -Iinclude
	gcc -O -c src/edit_modes/rect_mode.c -std=c99 -o rect_mode.o -Iinclude
	gcc -O -c src/edit_modes/put_mode.c -std=c99 -o put_mode.o -Iinclude
	gcc -O -c src/edit_modes/line_mode.c -std=c99 -o line_mode.o -Iinclude
main.o: chunk.o
	gcc -D _DEFAULT_SOURCE -O -c src/main.c -std=c99 -o main.o -Iinclude -lncurses
ascii-diagdrawer: main.o position.o ui.o undo_redo.o position_list.o chunk.o edit_mode.o
	gcc -g main.o undo_redo.o chunk.o position.o position_list.o edit_mode.o ui.o -Iinclude -lncurses -o ascii-diagdrawer rect_mode.o put_mode.o line_mode.o
all:ascii-diagdrawer show_keyname
clean:
	rm *.o
	rm ascii-diagdrawer
	rm show_keyname
rebuild: clean all

#PREFIX is environment variable, but if it is not set, then set default value
ifeq ($(PREFIX),)
    PREFIX := /usr
endif

install: all
	sudo cp ascii-diagdrawer /usr/local/bin
uninstall: all
	sudo rm /usr/local/bin/ascii-diagdrawer
