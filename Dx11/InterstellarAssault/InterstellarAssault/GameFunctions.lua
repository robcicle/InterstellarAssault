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

-- setPlayerLifes function: Sets the player's life count to a predefined number.
function setPlayerLifes()
    -- Calls the C++ function registered as 'setLifes' in the Dispatcher with the argument 5.
    -- This sets the player's life count to 5 rather than the original 3.
    CDispatcher("setLifes", 5)
end