

KZWindow.DrawFillRect(0,0,KZWindow.GetWidth(),KZWindow.GetHeight(),KZWindow.ConvertToWindowDrawColor(0,0,0,255));

if(x == nil) then
    x = 0
end

if(y == nil) then
    y = 0
end

local input = KZInput.InputState()

if(input.Left) then
    x = x - 1
elseif(input.Right) then
    x = x + 1
end

if(input.Up) then
    y = y - 1
elseif(input.Down) then
    y = y + 1
end

KZWindow.DrawFillRect(x,y,100,100,KZWindow.ConvertToWindowDrawColor(255,0,255,255));

