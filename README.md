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
Even if I'm trying to improve the `TEXT` mode as possible :)<br>

## How to use

Common operations:
* `[tab]`: enter/exit the menu.
* `[u]`: undo.
* `[ctrl] + [r]`: redo.
* `[arrow]`: move the cursor.
* `[ctrl] + [arrow]`: move the cursor faster.

## Contributing

Any contribution is appreciated to improve the software's capabilities/architecture<br>
In case af style changes, of course it's welcome but do not forget to explain why :)<br>

## TODO

* Multiple files editing.
* Finding words, chunk...
* Improvethe text editor.

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
