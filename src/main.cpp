#include <windows.h>
#include <string>
#include <cassert>
#include <d3d11.h>

#include "WindowUtils.h"
#include "LuaHelper.h"
#include "Game.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;


// OnResize function: Notifies D3D to handle window resizing.
void OnResize(int screenWidth, int screenHeight, MyD3D& d3d)
{
	d3d.OnResize_Default(screenWidth, screenHeight);  // Adjust D3D to the new window size.
}

// MainWndProc function: Handles window messages.
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Handle different types of window messages.
	switch (msg)
	{
	case WM_CHAR:  // Handle character input through the window.
		Game::Get().ProcessKey((char)wParam);  // Pass the key input to the game.
		break;
	case WM_INPUT:  // Handle raw input messages.
		Game::Get().mMKIn.MessageEvent((HRAWINPUT)lParam);  // Process mouse and keyboard input.
		break;
	}

	// Handle default window messages (resize, full screen, etc.)
	return WinUtil::DefaultMssgHandler(hwnd, msg, wParam, lParam);
}

// Function to load and parse Lua scripts from a directory
void LoadLuaScripts(lua_State* L, const std::string& directory)
{
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile((directory + "\\*.lua").c_str(), &findFileData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return; // No Lua scripts found
	}

	do
	{
		std::string filePath = directory + "\\" + findFileData.cFileName;
		if (!LuaHelper::LuaOK(L, luaL_dofile(L, filePath.c_str())))
		{
			assert(false); // Assert if Lua script loading fails
		}
	} while (FindNextFile(hFind, &findFileData) != 0);

	FindClose(hFind);
}

void EntryPoint(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE prevInstance,
	PSTR cmdLine, _In_ int showCmd)
{
	// Initialize window dimensions.
	int w(1024), h(768);

	// Create the main window.
	new WinUtil();
	if (!WinUtil::Get().InitMainWindow(w, h, hInstance, "Interstellar Assault", MainWndProc, true))
		assert(false);  // Assert if window creation fails.

	// Initialize Direct3D.
	MyD3D d3d;
	if (!d3d.InitDirect3D(OnResize))
		assert(false);  // Assert if Direct3D initialization fails.

	// Set various initial configurations for D3D, textures, fonts, etc.
	WinUtil::Get().SetD3D(d3d);
	d3d.GetTexCache().SetAssetPath("data/");
	d3d.GetFontCache().SetAssetPath("data/fonts/");

	srand((unsigned int)time(0));  // Seed the random number generator.

	// Initialize Lua
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);  // Open main libraries for scripts

	// Load and parse Lua scripts
	LoadLuaScripts(L, "data/scripts");

	// Register functions
	lua_pushcfunction(L, LuaHelper::CustomPrint);
	lua_setglobal(L, "print");

	Dispatcher D;
	D.Init(L);

	new Game(L, D);  // Instantiate and initialize the Game object.

	// Main game loop.
	bool canUpdateRender;
	float dTime = 0;
	Game& gm = Game::Get();
	while (WinUtil::Get().BeginLoop(canUpdateRender))
	{
		if (canUpdateRender && dTime > 0)
		{
			gm.Update(dTime);  // Update game logic.
			gm.Render(dTime);  // Render the game.
		}
		dTime = WinUtil::Get().EndLoop(canUpdateRender);  // Get the delta time for the next loop.
	}

	// Cleanup and release resources.
	delete& gm;
	d3d.ReleaseD3D(true);
	delete& WinUtil::Get();

	// Close our Lua
	lua_close(L);
}

#ifdef _DEBUG
int main(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE prevInstance,
	_In_ PSTR cmdLine, _In_ int showCmd)
{
	EntryPoint(hInstance, prevInstance, cmdLine, showCmd);
}
#else
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE prevInstance,
	_In_ PSTR cmdLine, _In_ int showCmd)
{
	EntryPoint(hInstance, prevInstance, cmdLine, showCmd);
}
#endif