

KaizoLevel = {}

function KaizoLevel:Reset()
    self.rooms = nil
    self.rooms = {}
    self.camerax = 0
    self.cameray = 0
end

function KaizoLevel:Update()
    for index, room in ipairs(self.rooms) do
        room:Update()
    end
end

function KaizoLevel:Render()

    KZWindow.DrawFillRect(0 ,0 , KZWindow.GetWidth(), KZWindow.GetHeight(), 0)

    for index, room in ipairs(self.rooms) do
        room:Render()
    end
end
