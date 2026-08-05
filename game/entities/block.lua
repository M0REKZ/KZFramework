

KaizoEntityBlock = {}
KaizoEntityBlock.__index = KaizoEntityBlock

function KaizoEntityBlock:Create(room)
    local o = {}
    o.x = 0
    o.y = 0
    o.w = 32
    o.h = 32

    --block
    o.is_solid = true

    o.room = room
    o = setmetatable(o, KaizoEntityBlock)
    room.entities[#room.entities+1] = o
    return o
end

function KaizoEntityBlock:Update()
    
end

function KaizoEntityBlock:Render()
    KZImages.RenderImage(0, 0, 0, 32, 32,self.x - KaizoLevel.camerax, self.y - KaizoLevel.cameray, self.w, self.h)
end

