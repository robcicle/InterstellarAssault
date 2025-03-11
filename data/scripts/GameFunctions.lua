timer = 0

-- start function (called once)
function start()
    print("Lua script started");
end

-- update function (called every frame)
function update(deltaTime)
    timer = timer + deltaTime
    print("Elapsed time: " .. timer)
end