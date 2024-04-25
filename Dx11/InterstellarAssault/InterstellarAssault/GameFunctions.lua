-- updateMusicVolume function: Updates the game music volume.
function updateMusicVolume(newAmount)
    -- This function calls the C++ LuaUpdateMusicVol defined in LuaHelper.h with a parameter.
    updateMusicVolume(newAmount, 1)
end

-- adjustMasterVolume function: Adjusts the master volume of the game.
function adjustMasterVolume(newAmount)
    -- This function calls the C++ LuaAdjustMasterVol defined in LuaHelper.h with a parameter.
    adjustMasterVolume(newAmount, 1)
end

-- adjustMusicVolume function: Adjusts the music volume of the game.
function adjustMusicVolume(newAmount)
    -- This function calls the C++ LuaAdjustMusicVol defined in LuaHelper.h with a parameter.
    adjustMusicVolume(newAmount, 1)
end

-- adjustGameVolume function: Adjusts the game volume.
function adjustGameVolume(newAmount)
    -- This function calls the C++ LuaAdjustGameVol defined in LuaHelper.h with a parameter.
    adjustGameVolume(newAmount, 1)
end

function setPlayerLifes()
    CDispatcher("setLifes", 10)
end