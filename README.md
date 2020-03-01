# A TUI software to create basics ASCII diagrams.
### BIG REFACTO NEEDED, UGLY CODE!

## Introduction

A minimalist and lightweight ascii diagram editor working with ncurses.<br>It
 features an easy interface for drawing rects, lines, arrows, text, copy-pasting...<br>

## How to use

Common operations:
* `[tab]`: enter/exit the menu.
* `[u]`: undo.
* `[ctrl] + [u]`: redo.
* `[arrow]`: move the cursor.
* `[ctrl] + [arrow]`: move the cursor faster.

Menu:
* `[p]`: put mode.
* `[t]`: text mode.
* `[r]`: rect mode.
* `[a]`: arrow mode.
* `[l]`: line mode.
* `[s]`: select mode.

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
