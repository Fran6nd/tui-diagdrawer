-- Any plugin need at least one NAME and one KEY.
NAME = "Demo Plugin"
KEY = string.byte("d")

-- The returned value is displayed on [ctrl] + [h].
function get_help()
    return "I'm the demo plugin!"
end

-- A point we wanna store.
p = {}
p.x = 1
p.y = 1

-- When the user move upper than the beginning of the file,
-- we increment the position of all lines by 1 so we need
-- to do so if we want to keep p in position.
function on_top_line_add()
    p.y = p.y + 1
end
-- Same here with on_left_column_add.
function on_left_column_add()
    p.x = p.x + 1
end

function on_key_event(key)
    if move_cursor(key) == nil then
        --show_message_blocking("Ok! working! " .. get_cursor_pos().x .. " " .. get_cursor_pos().y .. " " .. (key or "(nil)"))
        --local p = get_cursor_pos()
        --p.y = p.y + 1
        --set_cursor_pos(p)
        -- show_message_blocking(get_char_at(get_cursor_pos()))
        set_char_at(get_cursor_pos(), "x", 1)
        p = get_cursor_pos()
        -- Now we make a recovery point for the undo/redo system.
        do_change()
    end
end

-- If the position of the character beeing drawn is the one stored,
-- we draw it with the COL_SELECTION.
-- 4 colors are availables: COL_SELECTION COL_EMPTY COL_CURSOR COL_NORMAL 4.
function on_draw(pos, char)
    if(pos.x == p.x and pos.y == p.y)then
        char.color = COL_SELECTION
    end
    return char
end