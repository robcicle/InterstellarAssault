#pragma once

#include <future>
#include <vector>
#include <sstream>

#include "Input.h"
#include "AudioManager.h"
#include "ScoreSystem.h"
#include "SpriteFont.h"
#include "SpriteBatch.h"
#include "Sprite.h"
#include "Singleton.h"
#include "ModeMgr.h"
#include "Model.h"
#include "constants.h"
#include "Text.h"
#include "LuaHelper.h"


// Game class: Represents the main game, handling inputs, modes, rendering, and updates.
class Game : public Singleton<Game>
{
public:
	MouseAndKeys mMKIn;  // Handle mouse and keyboard inputs specific to the game.
	Gamepad mGamepad;    // Handle controller/joystick inputs.

	Game();    // Constructor: Initializes the game and starts resources loading.
	~Game() {  // Destructor: Ensures that resources are released.
		Release();  
	}

	void Load();     // Load: Create meshes for our imported 3D models.
	void Release();  // Release: Free up any dynamically created objects.

	// Update: Game logic that needs to happen per frame.
	void Update(float dTime);

	// Render: Drawing sprites or triangles to the screen per frame.
	void Render(float dTime);

	// ProcessKey: Handle any key inputs, specifically text input and not mMKIn.
	void ProcessKey(char key) {
		mMMgr.ProcessKey(key);
	}

	// Accessor functions for various components of the game.
	ModeMgr& GetModeMgr() { return mMMgr; }
	AudioManager& GetAudMgr() { return mAudMgr; }
	ScoreSystem& GetScoreSys() { return mScoreSys; }
	DirectX::SpriteFont* GetFont() { return mpFont; }
	lua_State* GetLuaState() { return mpLuaState; }

	// ChangeBackgroundColour: Update the background color of the game.
	void ChangeBackgroundColour(DirectX::SimpleMath::Vector4 colour) {
		mSkyBoxColour = colour;
	}

	void UpdateMusicVolume(float newVolume);

	std::vector<Model> mModels;                               // Holds our models in a vector for easy access.
	enum Modelid { ALIEN, ARCADEMACHINE, WAREHOUSE, TOTAL };  // Identifier for models.

	// Camera position variables: default and dynamic positions.
	DirectX::SimpleMath::Vector3 mCamPos = GC::DEF_CAM_POS;

	float gAngle = 0;  // Global angle for animations or rotations.

#if defined(DEBUG) || defined(_DEBUG)

	struct DebugData
	{
		DebugData(MyD3D& d3d);
		void UpdateDebug(float dTime);  // UpdateDebug: Update any debug logic, e.g. DrawCollisions toggle boolean with L Key
		void RenderDebug(float dTime, DirectX::SpriteBatch& batch) const;  // RenderDebug: Overlays debug information.

		bool mDebugDrawColliders = false;
		Text* mFpsText = nullptr;
		std::stringstream mFPSSS;
	};

	DebugData mDebugData;
#endif
private:
	void RenderGame(float dTime);  // RenderGame: Renders game elements based on the current mode.

	DirectX::SimpleMath::Vector4 mSkyBoxColour = { 0.529411793f, 0.807843208f, 0.980392218f, 1.0f };  // Background colour.

	// LoadData: Simple structure to manage the loading process and loading screen.
	struct LoadData
	{
	public:
		LoadData();   // Constructor: Initializes loading graphics and settings.
		~LoadData();  // Destructor: Ensures resources are released.

		void Update(float dTime);
		// Render: Logic for displaying the load screen while creating model meshes.
		void Render(float dTime, DirectX::DX11::SpriteBatch& batch);

	public:
		std::future<void> mLoader;  // Loader: Thread for loading models while rendering the load screen.
		int mTotalToLoad = 0;       // Total number of models to load.
		int mLoadedSoFar = 0;       // Number of models loaded so far.
		bool mRunning = false;      // Indicates if the loading process is running.

	private:
		Sprite mLoadBorder;  // Sprite for the loading bar border.
		Sprite mLoadFill;    // Sprite for the loading bar fill.

		RECTF mLoadFillRect;        // Texture rectangle for the loading fill.
		float mMaxFillAmt = 0;      // Maximum fill amount for the loading bar.
		float mCurrentFillAmt = 0;  // Current fill amount for the loading bar.

		float mLoadTransitionTimer = 0;    // Timer for transitioning from the loading screen to the game.
		float mTimeTillTransition = 1.5f;  // Time it takes for the transition to be fully completed.

		DirectX::SpriteFont* mLoadFont = nullptr;  // Font for loading text.
	};

	LoadData mLoadData;  // Instance of the LoadData structure.
	ModeMgr mMMgr;       // Manages different game modes.

	DirectX::SpriteBatch* mpSB = nullptr;   // SpriteBatch used to draw sprites.
	DirectX::SpriteFont* mpFont = nullptr;  // Font used throughout the application.

	AudioManager mAudMgr;   // Manages audio for the game.
	ScoreSystem mScoreSys;  // Manages the scoring system.

	lua_State* mpLuaState = nullptr;

	// Songs
	DirectX::SoundEffectInstance* automationSong = nullptr;
	DirectX::SoundEffectInstance* trashySong = nullptr;
};