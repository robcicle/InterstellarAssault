#include "LuaHelper.h"

#include <assert.h>
#include <iostream>

using namespace std;

bool LuaOK(lua_State* L, int id)
{
    if (id != LUA_OK)
    {
        cout << lua_tostring(L, -1) << endl;  // Print the error
        return false;  // Trigger an assert
    }
    return true;  // No error
}

int LuaGetInt(lua_State* L, const std::string& name)
{
    // Get Lua variable by name
    lua_getglobal(L, name.c_str());
    // Check it's an integer
    if (!lua_isinteger(L, -1))
        assert(false);

    return (int)lua_tointeger(L, -1);
}

std::string LuaGetStr(lua_State* L, const std::string& name)
{
    // Get Lua variable by name
    lua_getglobal(L, name.c_str());
    // Check it's a string
    if (!lua_isstring(L, -1))
        assert(false);

    return lua_tostring(L, -1);
}
