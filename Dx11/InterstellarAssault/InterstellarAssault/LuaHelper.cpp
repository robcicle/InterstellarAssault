#include "LuaHelper.h"

#include "Game.h"

#include <assert.h>
#include <iostream>

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;


// LuaOK function: Check Lua stack operation results and print any errors.
bool LuaOK(lua_State* L, int id)
{
    if (id != LUA_OK)
    {
        cout << lua_tostring(L, -1) << endl; // Print the Lua error message
        return false; // Return failure if not OK
    }
    return true; // Return success if OK
}

// ******FUNCTIONS******

// CallVoidVoidCFunc function: Call a Lua function by name with no parameters or return values.
void CallVoidVoidCFunc(lua_State* L, const std::string& fName)
{
    lua_getglobal(L, fName.c_str()); // Retrieve the function from the Lua global scope

    if (!lua_isfunction(L, -1))
        assert(false); // Assert if not a function

    if (!LuaOK(L, lua_pcall(L, 0, 0, 0)))
        assert(false); // Call the function and assert on failure
}

// CallVoidVoidCFunc function: Call a Lua function by name, passing a single float parameter.
void CallVoidVoidCFunc(lua_State* L, const std::string& fName, float number)
{
    lua_getglobal(L, fName.c_str()); // Retrieve the function from Lua global scope

    if (!lua_isfunction(L, -1))
        assert(false); // Assert if not a function

    lua_pushnumber(L, number); // Push the float parameter onto the Lua stack

    if (!LuaOK(L, lua_pcall(L, 1, 0, 0)))
        assert(false); // Call the function and assert on failure
}

// LuaFRandomNum function: Return a random number from a Lua function, specifying a range.
float LuaFRandomNum(lua_State* L, const std::string& fName, float min, float max)
{
    lua_getglobal(L, fName.c_str()); // Access the Lua function by name

    if (!lua_isfunction(L, -1))
        assert(false); // Assert if not a function

    lua_pushnumber(L, min); // Push the min value
    lua_pushnumber(L, max); // Push the max value

    if (!LuaOK(L, lua_pcall(L, 2, 1, 0)))
        assert(false); // Execute the function and assert on failure

    float returnedVariable = (float)lua_tonumber(L, -1); // Retrieve the result

    lua_pop(L, 1); // Clean up the Lua stack

    return returnedVariable; // Return the generated random number
}

// LuaFLerpNum function: Interpolate between two values using a Lua function.
float LuaFLerpNum(lua_State* L, const std::string& fName, float a, float b, float t)
{
    lua_getglobal(L, fName.c_str()); // Access the Lua function by name

    if (!lua_isfunction(L, -1))
        assert(false); // Assert if not a function

    lua_pushnumber(L, a); // Push the first value
    lua_pushnumber(L, b); // Push the second value
    lua_pushnumber(L, t); // Push the interpolation parameter

    if (!LuaOK(L, lua_pcall(L, 3, 1, 0)))
        assert(false); // Call the function and assert on failure

    float returnedVariable = (float)lua_tonumber(L, -1); // Retrieve the interpolated value

    lua_pop(L, 1); // Clean up the Lua stack

    return returnedVariable; // Return the interpolated result
}

// LuaUpdateMusicVol function: Update the game music volume based on a value provided by a Lua script.
int LuaUpdateMusicVol(lua_State* L)
{
    float newVolume = (float)lua_tonumber(L, 1); // Retrieve new volume level from Lua stack

    Game::Get().UpdateMusicVolume(newVolume); // Update game music volume

    lua_pop(L, 1); // Clean up the Lua stack
    return 1; // Return success
}

// LuaAdjustMasterVol function: Adjust the master volume across the game based on a Lua function call.
int LuaAdjustMasterVol(lua_State* L)
{
    float newVolume = (float)lua_tonumber(L, 1); // Retrieve the new volume level from Lua

    Game::Get().GetAudMgr().AdjustMasterVolume(newVolume); // Adjust the master volume

    lua_pop(L, 1); // Clean up the Lua stack
    return 1; // Return success
}

// LuaAdjustMusicVol function: Adjust the music volume across the game based on a Lua function call.
int LuaAdjustMusicVol(lua_State* L)
{
    float newVolume = (float)lua_tonumber(L, 1); // Retrieve the new volume level from Lua

    Game::Get().GetAudMgr().AdjustMusicVolume(newVolume); // Adjust the music volume

    lua_pop(L, 1); // Clean up the Lua stack
    return 1; // Return success
}

// LuaAdjustGameVol function: Adjust the game volume across the game based on a Lua function call.
int LuaAdjustGameVol(lua_State* L)
{
    float newVolume = (float)lua_tonumber(L, 1); // Retrieve the new volume level from Lua

    Game::Get().GetAudMgr().AdjustGameVolume(newVolume); // Adjust the game volume

    lua_pop(L, 1); // Clean up the Lua stack
    return 1; // Return success
}

// ******VARIABLES*******

// LuaGetInt function: Return an int from a Lua script based on a variable name.
int LuaGetInt(lua_State* L, const string& name)
{
    lua_getglobal(L, name.c_str()); // Retrieve Lua variable by name

    if (!lua_isinteger(L, -1))
        assert(false); // Assert if not an integer

    return (int)lua_tointeger(L, -1); // Return the retrieved integer
}

// LuaGetNum function: Return a float from a Lua script based on a variable name.
float LuaGetNum(lua_State* L, const std::string& name)
{
    lua_getglobal(L, name.c_str()); // Retrieve Lua variable by name

    if (!lua_isnumber(L, -1))
        assert(false); // Assert if not a number

    return (float)lua_tonumber(L, -1); // Return the retrieved number
}

// LuaGetStr function: Return a string from a Lua script based on a variable name.
string LuaGetStr(lua_State* L, const string& name)
{
    lua_getglobal(L, name.c_str()); // Retrieve Lua variable by name

    if (!lua_isstring(L, -1))
        assert(false); // Assert if not a string

    return lua_tostring(L, -1); // Return the retrieved string
}

// LuaGetVec2 function: Return a vector from a Lua script based on a variable name.
Vector2 LuaGetVec2(lua_State* L, const string& name)
{
    lua_getglobal(L, name.c_str()); // Retrieve Lua variable by name as a table

    if (!lua_istable(L, -1))
        assert(false); // Assert if not a table

    lua_pushstring(L, "x");
    lua_gettable(L, -2); // Retrieve 'x' component
    int x = (int)lua_tointeger(L, -1);
    lua_pop(L, 1); // Clean up 'x' component

    lua_pushstring(L, "y");
    lua_gettable(L, -2); // Retrieve 'y' component
    int y = (int)lua_tointeger(L, -1);
    lua_pop(L, 1); // Clean up 'y' component

    return DirectX::SimpleMath::Vector2((float)x, (float)y); // Return the constructed Vector2
}