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
// Call a Lua function from our C++ code
void CallVoidVoidCFunc(lua_State* L, const std::string& fName);
// Call a Lua function from our C++ and pass through a float
void CallVoidVoidCFunc(lua_State* L, const std::string& fName, float number);


// Return a random number from a Lua function
float LuaFRandomNum(lua_State* L, const std::string& fName, float min, float max);

// Return a number that has gone through linear interpolation
float LuaFLerpNum(lua_State* L, const std::string& fName, float a, float b, float t);

// C functions for our Lua scripts to call to enforce that
// their volume adjustments/updates are applied to the game.
int LuaUpdateMusicVol(lua_State* L);
int LuaAdjustMasterVol(lua_State* L);
int LuaAdjustMusicVol(lua_State* L);
int LuaAdjustGameVol(lua_State* L);

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