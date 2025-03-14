#include "LuaHelper.h"

#include "Game.h"

#include <assert.h>
#include <iostream>

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;


// LuaOK function: Check Lua stack operation results and print any errors.
bool LuaHelper::LuaOK(lua_State* L, int id)
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
void LuaHelper::CallVoidVoidCFunc(lua_State* L, const string& fName)
{
    lua_getglobal(L, fName.c_str()); // Retrieve the function from the Lua global scope

    if (!lua_isfunction(L, -1))
        assert(false); // Assert if not a function

    if (!LuaOK(L, lua_pcall(L, 0, 0, 0)))
        assert(false); // Call the function and assert on failure
}

// CallVoidVoidCFunc function: Call a Lua function by name, passing a single float parameter.
void LuaHelper::CallVoidVoidCFunc(lua_State* L, const string& fName, float number)
{
    lua_getglobal(L, fName.c_str()); // Retrieve the function from Lua global scope

    if (!lua_isfunction(L, -1))
        assert(false); // Assert if not a function

    lua_pushnumber(L, number); // Push the float parameter onto the Lua stack

    if (!LuaOK(L, lua_pcall(L, 1, 0, 0)))
        assert(false); // Call the function and assert on failure
}

// LuaFRandomNum function: Return a random number from a Lua function, specifying a range.
float LuaHelper::LuaFRandomNum(lua_State* L, const string& fName, float min, float max)
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
float LuaHelper::LuaFLerpNum(lua_State* L, const string& fName, float a, float b, float t)
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

int LuaHelper::CustomPrint(lua_State* L)
{
    int n = lua_gettop(L);  // Number of arguments
    std::cout << "[Lua]: ";
    for (int i = 1; i <= n; ++i)
    {
        if (lua_isstring(L, i))
        {
            std::cout << lua_tostring(L, i);
        }
        else
        {
            std::cout << luaL_tolstring(L, i, nullptr);
            lua_pop(L, 1);  // Remove the result of luaL_tolstring
        }
        if (i < n)
        {
            std::cout << "\t";
        }
    }
    std::cout << std::endl;
    return 0;
}

// LuaUpdateMusicVol function: Update the game music volume based on a value provided by a Lua script.
int LuaHelper::LuaUpdateMusicVol(lua_State* L)
{
    float newVolume = (float)lua_tonumber(L, 1); // Retrieve new volume level from Lua stack

    Game::Get().UpdateMusicVolume(newVolume); // Update game music volume

    lua_pop(L, 1); // Clean up the Lua stack
    return 1; // Return success
}

// LuaAdjustMasterVol function: Adjust the master volume across the game based on a Lua function call.
int LuaHelper::LuaAdjustMasterVol(lua_State* L)
{
    float newVolume = (float)lua_tonumber(L, 1); // Retrieve the new volume level from Lua

    Game::Get().GetAudMgr().AdjustMasterVolume(newVolume); // Adjust the master volume

    lua_pop(L, 1); // Clean up the Lua stack
    return 1; // Return success
}

// LuaAdjustMusicVol function: Adjust the music volume across the game based on a Lua function call.
int LuaHelper::LuaAdjustMusicVol(lua_State* L)
{
    float newVolume = (float)lua_tonumber(L, 1); // Retrieve the new volume level from Lua

    Game::Get().GetAudMgr().AdjustMusicVolume(newVolume); // Adjust the music volume

    lua_pop(L, 1); // Clean up the Lua stack
    return 1; // Return success
}

// LuaAdjustGameVol function: Adjust the game volume across the game based on a Lua function call.
int LuaHelper::LuaAdjustGameVol(lua_State* L)
{
    float newVolume = (float)lua_tonumber(L, 1); // Retrieve the new volume level from Lua

    Game::Get().GetAudMgr().AdjustGameVolume(newVolume); // Adjust the game volume

    lua_pop(L, 1); // Clean up the Lua stack
    return 1; // Return success
}

// ******VARIABLES*******

// LuaGetInt function: Return an int from a Lua script based on a variable name.
int LuaHelper::LuaGetInt(lua_State* L, const string& name, const int& default)
{
    lua_getglobal(L, name.c_str()); // Retrieve Lua variable by name

    if (!lua_isinteger(L, -1))
    {
        return default;
    }

    return (int)lua_tointeger(L, -1); // Return the retrieved integer
}

// LuaGetNum function: Return a float from a Lua script based on a variable name.
float LuaHelper::LuaGetNum(lua_State* L, const string& name, const float& default)
{
    lua_getglobal(L, name.c_str()); // Retrieve Lua variable by name

    if (!lua_isnumber(L, -1))
    {
        return default;
    }

    return (float)lua_tonumber(L, -1); // Return the retrieved number
}

// LuaGetStr function: Return a string from a Lua script based on a variable name.
string LuaHelper::LuaGetStr(lua_State* L, const string& name, const string& default)
{
    lua_getglobal(L, name.c_str()); // Retrieve Lua variable by name

    if (!lua_isstring(L, -1))
    {
        return default;
    }

    return lua_tostring(L, -1); // Return the retrieved string
}

// LuaGetVec2 function: Return a vector from a Lua script based on a variable name.
Vector2 LuaHelper::LuaGetVec2(lua_State* L, const string& name)
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

    return Vector2((float)x, (float)y); // Return the constructed Vector2
}

// Static member initialization: Stores all registered commands accessible by function name.
map<string, Dispatcher::Command> Dispatcher::library;

// LuaCall function: Called from Lua to dispatch the request to the corresponding C++ function.
int Dispatcher::LuaCall(lua_State* L)
{
    // Retrieve the function name from the first argument passed from Lua.
    string fName = lua_tostring(L, 1);

    // Find the command in the library map using the function name.
    map<string, Command>::iterator it = library.find(fName);
    // Ensure the function is registered before proceeding.
    assert(it != library.end());

    // Reference to the command associated with the function name.
    Command& cmd = (*it).second;

    // Check if the function pointer is valid before calling.
    if (cmd.voidintfunct)
    {
        // Retrieve the integer parameter from Lua stack to pass to the C++ function.
        int param = (int)lua_tointeger(L, 2);
        // Call the registered function with the parameter.
        cmd.voidintfunct(param);
        // Pop the parameter off the Lua stack after use.
        lua_pop(L, 1);
    }
    else
        assert(false);  // Assert failure if the function pointer is not valid.

    return 1;  // Return success status to Lua.
}