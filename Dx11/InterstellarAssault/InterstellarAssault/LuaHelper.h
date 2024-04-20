#pragma once

// Lua helper to link Lua with C++
#include <string>
#include <functional>
#include <map>
#include <SimpleMath.h>

extern "C"
{
	#include "../lua-5.4.4/include/lua.h"
	#include "../lua-5.4.4/include/lauxlib.h"
	#include "../lua-5.4.4/include/lualib.h"
}

#ifdef _WIN32
#pragma comment(lib, "../lua-5.4.4/liblua54.a")
#endif // WIN32

// Check for errors
bool LuaOK(lua_State* L, int id);

// Return an int from Lua
int LuaGetInt(lua_State* L, const std::string& name);

// Return a string from Lua
std::string LuaGetStr(lua_State* L, const std::string& name);

DirectX::SimpleMath::Vector2 LuaGetVec2(lua_State* L, const std::string& name);