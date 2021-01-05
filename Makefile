.DEFAULT_GOAL := tui-diagdrawer

show_keyname.o: src/show_keyname.c
	gcc -g -O -c src/show_keyname.c -std=c99 -o show_keyname.o -Incurses
show_keyname: show_keyname.o
	gcc -g -g show_keyname.o -Iinclude -lncurses -o show_keyname
ui.o: src/ui.c
	gcc -g -O -c src/ui.c -std=c99 -o ui.o -Iinclude
undo_redo.o: src/undo_redo.c
	gcc -g -O -c src/undo_redo.c -std=c99 -o undo_redo.o -Iinclude
position.o: src/position.c
	gcc -g -O -c src/position.c -std=c99 -o position.o -Iinclude
position_list.o: src/position_list.c
	gcc -g -O -c src/position_list.c -std=c99 -o position_list.o -Iinclude
chunk.o: src/chunk.c position.o position_list.o
	gcc -g -O -c src/chunk.c -std=c99 -o chunk.o -Iinclude
edit_mode.o: src/edit_mode.c
	gcc -g -O -c src/edit_mode.c -std=c99 -o edit_mode.o -Iinclude
	gcc -g -O -c src/edit_modes/rect_mode.c -std=c99 -o rect_mode.o -Iinclude
	gcc -g -O -c src/edit_modes/put_mode.c -std=c99 -o put_mode.o -Iinclude
	gcc -g -O -c src/edit_modes/line_mode.c -std=c99 -o line_mode.o -Iinclude
	gcc -g -O -c src/edit_modes/arrow_mode.c -std=c99 -o arrow_mode.o -Iinclude
	gcc -g -O -c src/edit_modes/text_mode.c -std=c99 -o text_mode.o -Iinclude
	gcc -g -O -c src/edit_modes/select_mode.c -std=c99 -o select_mode.o -Iinclude
	gcc -g -O -c src/edit_modes/plugin_mode.c  -std=c99 -Iinclude -o plugin_mode.o
main.o: chunk.o
	gcc -g -D _DEFAULT_SOURCE -O -c  src/main.c -std=c99 -o main.o -Iinclude -lncurses
tui-diagdrawer: main.o position.o ui.o undo_redo.o position_list.o chunk.o edit_mode.o
	gcc -g main.o edit_mode.o rect_mode.o put_mode.o line_mode.o arrow_mode.o text_mode.o select_mode.o plugin_mode.o undo_redo.o chunk.o position.o position_list.o ui.o -Iinclude -lncurses -llua -lm -ldl -o tui-diagdrawer
all:tui-diagdrawer show_keyname
clean:
	rm -f *.o
	rm -f tui-diagdrawer
	rm -f show_keyname
rebuild: clean all

#PREFIX is environment variable, but if it is not set, then set default value
ifeq ($(PREFIX),)
    PREFIX := /usr
endif

install: tui-diagdrawer
	sudo cp tui-diagdrawer /usr/local/bin
	mkdir -p ~/.tui-diagdrawer
	cp src/demo_plugin.lua ~/.tui-diagdrawer
uninstall:
	sudo rm /usr/local/bin/tui-diagdrawer
	rm -rf ~/.tui-diagdrawer
