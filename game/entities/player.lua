
require "common.collision"

KaizoEntityPlayer = {}
KaizoEntityPlayer.__index = KaizoEntityPlayer

function KaizoEntityPlayer:Create(room)
    local o = {}
    o.x = 0
    o.y = 0
    o.w = 32
    o.h = 32

    --player
    o.velx = 0
    o.vely = 0

    o.max_velx = 5
    o.max_vely = 15

    o.grounded = false
    o.max_jump_timer = 20
    o.jump_timer = 0

    o.camera_y_must_follow = true

    o.room = room
    o = setmetatable(o, KaizoEntityPlayer)
    room.entities[#room.entities+1] = o
    return o
end

function KaizoEntityPlayer:FixCameraPosition()
    KaizoLevel.camerax = math.floor(KaizoLevel.camerax)
    KaizoLevel.cameray = math.floor(KaizoLevel.cameray)

    local temprect = {
        x = KaizoLevel.camerax,
        y = KaizoLevel.cameray,
        w = KZWindow.GetWidth(),
        h = KZWindow.GetHeight(),
    }

    KaizoCollision:KeepInRoom(temprect, self.room)

    KaizoLevel.camerax = temprect.x
    KaizoLevel.cameray = temprect.y
end

local function TouchGround(self, y_pos)
    self.vely = 0
    self.y = y_pos - self.h

    self.grounded = true
    self.jump_timer = 0
end

local function Die(self)
    self.dead = true
    self.private_death_anim = true
end

function KaizoEntityPlayer:Update()

    if self.dead then
        --death effect and all that
        return
    end

    local inputstate = KZInput.InputState()

    if inputstate.Jump then
        if self.grounded then
            self.jump_timer = self.max_jump_timer
        end
    elseif self.jump_timer > 5 then
        self.jump_timer = 5
    end

    if inputstate.Run then
        self.max_velx = 9
    else
        self.max_velx = 5
    end

    if inputstate.Left then
        self.velx = self.velx - 0.75
    elseif inputstate.Right then
        self.velx = self.velx + 0.75
    else
        self.velx = self.velx * 0.75
    end

    if self.jump_timer > 0 then
        self.vely = -15 * (self.jump_timer / self.max_jump_timer)
        self.jump_timer = self.jump_timer - 1
    else
        if(not self.grounded) then
            self.vely = self.vely + 1
        end
    end



    if self.velx > self.max_velx then
        self.velx = self.max_velx
    elseif self.velx < -self.max_velx then
        self.velx = -self.max_velx
    end

    if self.vely > self.max_vely then
        self.vely = self.max_vely
    elseif self.vely < -self.max_vely then
        self.vely = -self.max_vely
    end

    local horizontalfirst = true

    if (not self.grounded) or self.jump_timer then
        horizontalfirst = false
    end

    --default for collision
    self.grounded = false

    for index, ent in ipairs(self.room.entities) do
        if not ent.is_solid then
            goto continue
        end

        local testsquare
        local exitloop
        local curcheck

        curcheck = not horizontalfirst
        while not exitloop do

            if not curcheck then
                testsquare = {x = self.x + self.velx, y = self.y, w = self.w, h = self.h}
                if KaizoCollision:CheckTouching(testsquare, ent) then
                    local hor = KaizoCollision:CheckHorizontal(testsquare, ent)
                    if hor > 0 then
                        if(self.velx < 0) then
                            self.velx = 0
                            self.x = ent.x + ent.w
                        end
                    elseif hor < 0 then
                        if(self.velx > 0) then
                            self.velx = 0
                            self.x = ent.x - self.w
                        end
                    end
                end
            else
                testsquare = {x = self.x + self.velx, y = self.y + self.vely, w = self.w, h = self.h}
                if KaizoCollision:CheckTouching(testsquare, ent) then
                    local ver = KaizoCollision:CheckVertical(testsquare, ent)
                    if ver > 0 then
                        if(self.vely < 0) then
                            self.vely = 0
                            self.y = ent.y + ent.h
                        end
                    elseif ver < 0 then
                        if(self.vely > 0) then
                            TouchGround(self, ent.y)
                        end
                    end
                end
            end
            if curcheck == horizontalfirst then
                exitloop = true
            else
                curcheck = horizontalfirst
            end
        end

        ::continue::
    end

    self.x = self.x + self.velx
    self.y = self.y + self.vely

    local touched_sides = KaizoCollision:KeepInRoom(self, self.room)

    if(touched_sides.down) then
        TouchGround(self, self.room.y + self.room.h)
        if(self.room.down_solid == "death") then
            Die(self)
        end
    end

    local targetx = self.x + self.w/2
    local cameraxcenter = KaizoLevel.camerax + KZWindow.GetWidth()/2

    KaizoLevel.camerax = KaizoLevel.camerax + (targetx - cameraxcenter) * 0.7

    local targety = self.y + self.h/2
    local cameraycenter = KaizoLevel.cameray + KZWindow.GetHeight()/2

    if(self.camera_y_must_follow) then
        KaizoLevel.cameray = KaizoLevel.cameray + (targety - cameraycenter) * 0.25

        if self.grounded and (targety <= cameraycenter + 0.5 and targety >= cameraycenter - 0.5) then
            self.camera_y_must_follow = false
        end
    end

    if (not (targety <= cameraycenter + 180 and targety >= cameraycenter - 180)) then
        self.camera_y_must_follow = true
    end

    self:FixCameraPosition()
end

function KaizoEntityPlayer:Render()
    KZImages.RenderImage(0, 0, 0, 32, 32,self.x - KaizoLevel.camerax, self.y - KaizoLevel.cameray, self.w, self.h)
end

