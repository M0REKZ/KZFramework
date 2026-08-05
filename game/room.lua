
KaizoRoom = {}
KaizoRoom.__index = KaizoRoom

function KaizoRoom:Create()
    local o = {}
    o.x = 0
    o.y = 0
    o.w = 0
    o.h = 0
    o.entities = {}
    o.frozen = false
    o.visible = true

    o.up_solid = true
    o.down_solid = "death"
    o.left_solid = true
    o.right_solid = true

    return setmetatable(o, KaizoRoom)
end

function KaizoRoom:Update()
    if self.frozen then
        return
    end

    for _, entity in ipairs(self.entities) do
        --if isincamera then
            entity.active = true
            if(entity.Update) then
                entity:Update()
            end
        --else
        --    entity.active = false
        --end
    end
end

local function DrawBackground(self)
    local clipX = math.max(0, math.floor(self.x - KaizoLevel.camerax))
    local clipY = math.max(0, math.floor(self.y - KaizoLevel.cameray))

    local clipW = math.min(KZWindow.GetWidth(), math.floor((self.x + self.w) - KaizoLevel.camerax))
    local clipH = math.min(KZWindow.GetHeight(), math.floor((self.y + self.h) - KaizoLevel.cameray))

    if self.image_id then
        local SCREEN_W = KZWindow.GetWidth()
        local SCREEN_H = KZWindow.GetHeight()

        local IMAGE_W = KZImages.GetImageWidth(self.image_id)
        local IMAGE_H = KZImages.GetImageHeight(self.image_id)

        local scaleX = SCREEN_W / IMAGE_W
        local scaleY = SCREEN_H / IMAGE_H

        local srcX = math.floor(clipX / scaleX)
        local srcY = math.floor(clipY / scaleY)
        local srcW = math.floor(clipW / scaleX)
        local srcH = math.floor(clipH / scaleY)

        KZImages.RenderImage(
            self.image_id,
            srcX, srcY,
            srcW,
            srcH,
            clipX,
            clipY,
            clipW,
            clipH
        )
    else
        KZWindow.DrawFillRect(
            clipX, clipY,
            clipX + self.w,
            clipY + self.h,
            0
        )
    end
end

function KaizoRoom:Render()

    --KZImages.RenderImage()

    DrawBackground(self)

    if not self.visible then
        return
    end

    for _, entity in ipairs(self.entities) do
        --if isincamera then
            if(entity.Render) then
                entity:Render()
            end
        --else
        --    entity.active = false
        --end
    end
end
