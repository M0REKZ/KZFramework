

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

KZWindow.DrawFillRect(x,y,10,10,KZWindow.ConvertToWindowDrawColor(255,0,255,255));


if xd == nil then
    xd = 2
end

if xd == 1 then
    KZFiles.OpenFileForWriting("test.txt")
    KZFiles.WriteLine("Hello World!")
    KZFiles.CloseFile()

    xd = 5
end

if xd == 2 then
    KZFiles.OpenFileForReading("test.txt")
    local axd = KZFiles.GetNextLine()
    KZFiles.CloseFile()
    print(axd)
    xd = 5
end

