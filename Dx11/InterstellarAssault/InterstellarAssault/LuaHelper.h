#pragma once

// Lua helper to link Lua with C++
#include <string>
#include <functional>
#include <map>
#include <SimpleMath.h>

// Required Lua libraries
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

// ******FUNCTIONS******
// 
// Return a random number from a Lua function
float LuaFRandomNum(lua_State* L, const std::string& fName, float min, float max);

float LuaFLerpNum(lua_State* L, const std::string& fName, float a, float b, float t);

// ******VARIABLES*******
// 
// Return an int from a Lua script
int LuaGetInt(lua_State* L, const std::string& name);

// Return a float from a Lua script
float LuaGetNum(lua_State* L, const std::string& name);

// Return a string from a Lua script
std::string LuaGetStr(lua_State* L, const std::string& name);

// Return a vector from a Lua script
DirectX::SimpleMath::Vector2 LuaGetVec2(lua_State* L, const std::string& name);