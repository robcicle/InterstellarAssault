#include "LuaHelper.h"

#include <assert.h>
#include <iostream>

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

bool LuaOK(lua_State* L, int id)
{
    if (id != LUA_OK)
    {
        cout << lua_tostring(L, -1) << endl;  // Print the error
        return false;  // Trigger an assert
    }
    return true;  // No error
}

// ******FUNCTIONS******

// Return a random number from a Lua function
float LuaFRandomNum(lua_State* L, const std::string& fName, float min, float max)
{
    // Get Lua function by name
    lua_getglobal(L, fName.c_str());
    if (!lua_isfunction(L, -1))
        assert(false);

    // Push the parameters for the function
    lua_pushnumber(L, min);
    lua_pushnumber(L, max);

    // Calls the function in protected mode.
    if (!LuaOK(L, lua_pcall(L, 2, 1, 0)))
        assert(false);

    // Grab the returned variable from the function
    float returnedVariable = (float)lua_tonumber(L, -1);

    // Pop it off as we've retrieved it
    lua_pop(L, 1);
    
    return returnedVariable;
}


float LuaFLerpNum(lua_State* L, const std::string& fName, float a, float b, float t)
{
    // Get Lua function by name
    lua_getglobal(L, fName.c_str());
    if (!lua_isfunction(L, -1))
        assert(false);

    // Push the parameters for the function
    lua_pushnumber(L, a);
    lua_pushnumber(L, b);
    lua_pushnumber(L, t);

    // Calls the function in protected mode.
    if (!LuaOK(L, lua_pcall(L, 3, 1, 0)))
        assert(false);

    // Grab the returned variable from the function
    float returnedVariable = (float)lua_tonumber(L, -1);

    // Pop it off as we've retrieved it
    lua_pop(L, 1);

    return returnedVariable;
}

// ******VARIABLES*******

// Return an int from a Lua script
int LuaGetInt(lua_State* L, const string& name)
{
    // Get Lua variable by name
    lua_getglobal(L, name.c_str());

    // Check it's an integer
    if (!lua_isinteger(L, -1))
        assert(false);

    return (int)lua_tointeger(L, -1);
}

// Return a string from a Lua script
string LuaGetStr(lua_State* L, const string& name)
{
    // Get Lua variable by name
    lua_getglobal(L, name.c_str());

    // Check it's a string
    if (!lua_isstring(L, -1))
        assert(false);

    return lua_tostring(L, -1);
}

// Return a vector from a Lua script
Vector2 LuaGetVec2(lua_State* L, const string& name)
{
    int x, y;

    lua_getglobal(L, name.c_str());
    if (!lua_istable(L, -1))
        assert(false);

    lua_pushstring(L, "x");
    // Pops X off and replaces it with the value
    lua_gettable(L, -2);
    x = (int)lua_tointeger(L, -1);
    // Then pop the value off once we have X
    lua_pop(L, 1);

    lua_pushstring(L, "y");
    // Pops Y off and replaces it with the value
    lua_gettable(L, -2);
    y = (int)lua_tointeger(L, -1);
    // Then pop the value off once we have Y
    lua_pop(L, 1);

    return DirectX::SimpleMath::Vector2((float)x, (float)y);
}
