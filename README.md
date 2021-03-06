# A TUI software to draw basics ASCII diagrams.
![alt text](showcase.gif)

## Introduction

A minimalist and lightweight ascii diagram editor working with ncurses.<br>
It features an easy to use interface for drawing rects, lines, arrows, text...<br>
and doing things such as copy-pasting, filling selection...<br>
It's written in C because first I like C, C is beautiful, C is simple,<br>
C's memmory management is challenging and it compiles fast.<br>
<br>
This project is not intended to be a good text editor but a good ascii diagram drawer.<br>
Even if I'm trying to improve the `TEXT` mode as much as possible :)<br>
The main purpose of this project is to help writing documentation for devs<br>
but it can also be used to draw a tui-videogame map!<br>
<br>
The project support LUA scripting (scripts are in `~/.tui-diagdrawer`)<br>
and if you run `make install` it will install a demo script.<br>
Be careful if you want to use plugins written by someone else!<br>
The sandbox is not done yet!<br>

## How to use

### How to setup

To install dependancies (ncurses and Lua):

* `sudo apt-get install libncurses5-dev libncursesw5-dev`
* `curl -R -O http://www.lua.org/ftp/lua-5.4.0.tar.gz`
* `tar zxf lua-5.4.0.tar.gz`
* `cd lua-5.4.0`
* `make`
* `make install`


To build the project:

* `git clone git@github.com:Fran6nd/tui-diagdrawer.git`
* `cd tui-diagdrawer`
* `make`
* `./tui-diagdrawer myfile`

To install:

* `sudo make install`

### Common operations

* `[tab]`: enter/exit the menu.
* `[ctrl] + [u]`: undo.
* `[ctrl] + [r]`: redo.
* `[arrow]`: move the cursor.
* `[ctrl] + [arrow]`: move the cursor faster.

## Contributing

Any contribution is appreciated to improve the software's capabilities/architecture<br>
In case af style changes, of course it's welcome but do not forget to explain why :)<br>

## TODO

* Multiple files editing.
* Finding words, chunk...
* Display strings bigger than screen in help viewport.
* Improve the text editor.
* Better sandbox for LUA scripts.
* Enable the user to change the name of the outpout file.
* Better Lua errors handling (avoid segfault if args not ok).
* Capability to override existing built-in edit modes.

# Here is what we can do:

```
 | +-----------+
 | |rects      |-------+
 | +-----------+       |
 |                     |
 | lines:              |
 |  |   |              |
 |  |   |              |
 |  |   |<---arrows----+
 |  +---+
 |
 |  fill:
 |  **********
 |  **********
 |  **********
 |  **********
 |  copy past:
 |  ********** ********** **********
 |  ********** ********** **********
 |  ********** ********** **********
 |  ********** ********** **********
 +-----------------------------------+
 ```

 # And an example diagram I made for another project.

 ```
                      +-----B-West to A----+   +---A-East to B-------+
                      |                    |   |                     |
               |      V      |             |   |              |      V      | 
  edge  0  <---| +---------+ |--->edge 0-------+  edge  0 <---| +---------+ |--->edge  0
  edge ... <---|-| BRICK A |-|--->edge ... |      edge ...<---|-| BRICK B |-|--->edge ...
  edge  n  <---| +---------+ |--->edge n   +------edge  n <---| +---------+ |--->edge  n
               |             |                                |             |
               |             |                                |             |
               |             |                                |             |
               |             |                                |             |
           WEST SIDE     EAST SIDE                        WEST SIDE     EAST SIDE
```
