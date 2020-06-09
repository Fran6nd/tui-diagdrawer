# A TUI software to create basics ASCII diagrams.
### BIG REFACTO NEEDED, UGLY CODE!

![alt text](showcase.gif)

## Introduction

A minimalist and lightweight ascii diagram editor working with ncurses.<br>It
 features an easy interface for drawing rects, lines, arrows, text, copy-pasting...<br>

## How to use

Common operations:
* `[tab]`: enter/exit the menu.
* `[u]`: undo.
* `[ctrl] + [r]`: redo.
* `[arrow]`: move the cursor.
* `[ctrl] + [arrow]`: move the cursor faster.

Menu:
* `[p]`: put mode: basically set the char at the cursor position.
* `[t]`: text mode: allow writing text without destroying rects/arrows.
* `[r]`: rect mode: allow drawing some rectangles.
* `[a]`: arrow mode.
* `[l]`: line mode.
* `[s]`: select mode: allow copy `[y]`,  pasting `[p]`, cut `[c]` and fill `[f]`, then the character to use.

## Contributing

Any contribution is appreciated to improve the software's capabilities/architecture<br>
In case af style changes, of course tit's welcome but do not forget to explain why :)<br>
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
