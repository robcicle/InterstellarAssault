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

int LuaGetInt(lua_State* L, const string& name)
{
    // Get Lua variable by name
    lua_getglobal(L, name.c_str());
    // Check it's an integer
    if (!lua_isinteger(L, -1))
        assert(false);

    return (int)lua_tointeger(L, -1);
}

string LuaGetStr(lua_State* L, const string& name)
{
    // Get Lua variable by name
    lua_getglobal(L, name.c_str());
    // Check it's a string
    if (!lua_isstring(L, -1))
        assert(false);

    return lua_tostring(L, -1);
}

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
