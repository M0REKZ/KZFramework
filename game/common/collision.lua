
KaizoCollision = {}

function KaizoCollision:CheckTouching(me, it)
    if(me.y + me.h >= it.y and me.y <= it.y + it.w) and (me.x + me.w >= it.x and me.x <= it.x + it.w) then
        return true
    end
    return false
end

function KaizoCollision:CheckHorizontal(me, it)
    if(me.y + me.h > it.y and me.y < it.y + it.w) and (me.x + me.w >= it.x and me.x <= it.x + it.w) then
        if(me.x + me.w / 2 < it.x + it.w / 2) then
            return -1
        else
            return 1
        end
    end
    return 0
end

function KaizoCollision:CheckVertical(me, it)
    if(me.y + me.h >= it.y and me.y <= it.y + it.w) and (me.x + me.w > it.x and me.x < it.x + it.w) then
        if(me.y + me.h / 2 < it.y + it.h / 2) then
            return -1
        else
            return 1
        end
    end
    return 0
end

function KaizoCollision:KeepInRoom(rect, room)
    local touched_sides = {}
    if room.left_solid and rect.x < room.x then
        rect.x = room.x
        touched_sides.left = true
    elseif room.right_solid and rect.x + rect.w > room.x + room.w then
        rect.x = room.x + room.w - rect.w
        touched_sides.right = true
    end

    if room.up_solid and rect.y < room.y then
        rect.y = room.y
        touched_sides.up = true
    elseif room.down_solid and rect.y + rect.h > room.y + room.h then
        rect.y = room.y + room.h - rect.h
        touched_sides.down = true
    end

    return touched_sides
end
