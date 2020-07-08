NAME = "Demo Plugin"
KEY = string.byte("d")

function get_help()
    return "I'm the demo plugin!"
end

function on_key_event(key)
    if move_cursor(key) == nil then
        --show_message_blocking("Ok! working! " .. get_cursor_pos().x .. " " .. get_cursor_pos().y .. " " .. (key or "(nil)"))
        --local p = get_cursor_pos()
        --p.y = p.y + 1
        --set_cursor_pos(p)
        show_message_blocking(get_char_at(get_cursor_pos()))
        set_char_at(get_cursor_pos(), nil)
    end
end
function on_draw(pos, char)
    
    if(pos.x % 2 == 0)then
        char.color = COL_EMPTY
    end
    if(pos.y % 2 == 0)then
        char.color = COL_EMPTY
    end
    return char
end