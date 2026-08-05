
package.path = package.path .. ";./game/?.lua" .. ";./game/?/init.lua"

require "globals"

require "context"
require "level"
require "room"

require "entities.block"
require "entities.player"

if(not Init) then
    print("By +KZ")


    KaizoLevel:Reset()

    local testroom = KaizoRoom:Create()
    testroom.x = -190
    testroom.y = -20
    testroom.w = 700
    testroom.h = 600
    testroom.image_id = KZImages.LoadImage(1)

    local o = KaizoEntityBlock:Create(testroom)
    o.x = 170
    o.y = 170
    o = nil
    o = KaizoEntityBlock:Create(testroom)
    o.x = 170 - o.w
    o.y = 170 -- o.h
    o = nil

    for i = 1, 5, 1 do
        o = KaizoEntityBlock:Create(testroom)
        o.y = 170 -- o.h 

        o.x = 170 - (o.w * i)
    end
    
    KaizoEntityPlayer:Create(testroom)

    KaizoLevel.rooms[1] = testroom

    Init = true
end

KaizoContext:Update()

KaizoContext:Render()
