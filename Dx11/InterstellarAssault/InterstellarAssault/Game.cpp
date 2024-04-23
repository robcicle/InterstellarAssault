#include "Game.h"
#include "Model.h"
#include "WindowUtils.h"
#include "IntroMode.h"
#include "MainMenuMode.h"
#include "ScoreMenuMode.h"
#include "SettingsMenuMode.h"
#include "TutorialMenuMode.h"
#include "PlayMode.h"
#include "GameOverMode.h"
#include "CommonStates.h"
#include "Utils.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

// Setup function for initializing a model with properties like mesh, scale, position, and rotation.
void Setup(Model& m, Mesh& source, const Vector3& scale, const Vector3& pos, const Vector3& rot)
{
	m.initialize(source);  // Initialize the model with the source mesh.
	m.GetScale() = scale;  // Set the model's scale.
	m.GetPosition() = pos;  // Set the model's position.
	m.GetRotation() = rot;  // Set the model's rotation.
}

// Overloaded Setup function for uniform scaling.
void Setup(Model& m, Mesh& source, float scale, const Vector3& pos, const Vector3& rot)
{
	Setup(m, source, Vector3(scale, scale, scale), pos, rot);  // Call the original setup with a uniform scale.
}

// Game Constructor: Sets up the game, loads resources, and initializes modes.
Game::Game() : mpSB(nullptr), mpFont(nullptr), mpLuaState(nullptr)
#if defined(DEBUG) || defined(_DEBUG)
    , mDebugData(WinUtil::Get().GetD3D())
#endif
{
    // Initialize Lua
    mpLuaState = luaL_newstate();
    luaL_openlibs(mpLuaState);  // Open main libraries for scripts

    // Load and parse Lua scripts
    if (!LuaOK(mpLuaState, luaL_dofile(mpLuaState, "GameVariables.lua")))
        assert(false);
    if (!LuaOK(mpLuaState, luaL_dofile(mpLuaState, "UtilityFunctions.lua")))
        assert(false);

	// Initialization of input handling, sprite batch, and fonts.
	mMKIn.Initialize(WinUtil::Get().GetMainWnd(), true, false);
	mpSB = new SpriteBatch(&WinUtil::Get().GetD3D().GetDeviceCtx());
	mpFont = new SpriteFont(&WinUtil::Get().GetD3D().GetDevice(), L"data/fonts/retrotech.spritefont");

    // Initialization of audio.
    automationSong = mAudMgr.CreateMusicInstance(AudioManager::MusicList::AUTOMATION_SONG);
    trashySong = mAudMgr.CreateMusicInstance(AudioManager::MusicList::TRASHY_SONG);

	// Adding different game modes to the Mode Manager.
	mMMgr.AddMode(new IntroMode());
    mMMgr.AddMode(new MainMenuMode());
    mMMgr.AddMode(new ScoreMenuMode());
    mMMgr.AddMode(new SettingsMenuMode());
    mMMgr.AddMode(new TutorialMenuMode());
	mMMgr.AddMode(new PlayMode());
	mMMgr.AddMode(new GameOverMode());

#if defined(DEBUG) || defined(_DEBUG)
    // Make sure our mode manager goes to the correct one
	mMMgr.SwitchMode(MainMenuMode::MODE_NAME);

#else
    // Make sure our mode manager goes to the correct one
    mMMgr.SwitchMode(IntroMode::MODE_NAME);

	// Begin loading models in a separate thread.
	mLoadData.mTotalToLoad = Modelid::TOTAL;
	mLoadData.mLoadedSoFar = 0;
	mLoadData.mRunning = true;
	mLoadData.mLoader = std::async(launch::async, &Game::Load, this);
#endif

    // Because of how IsUp or MouseButtonUp works, we need to refresh
    // the states immediatly otherwise EVERYTHING is up on first frame
    mMKIn.RefreshState();
}

// Load function: Asynchronously loads game models and updates loading progress.
void Game::Load()
{
    // Preallocate space for models.
    Model m;
    mModels.insert(mModels.begin(), Modelid::TOTAL, m);

    // Load and setup specific meshes for different model types.
    // For each model type, create its mesh, setup the model, and increment the loaded count.
    Mesh& alienMesh = WinUtil::Get().GetD3D().GetMeshMgr().CreateMesh("alien");
    alienMesh.CreateFrom("data/models/alien.fbx", WinUtil::Get().GetD3D());
    Setup(mModels[Modelid::ALIEN], alienMesh, 0.05f, Vector3(0, 1, 2), Vector3(0, PI, 0));
    mLoadData.mLoadedSoFar++;

    Mesh& arcadeMachineMesh = WinUtil::Get().GetD3D().GetMeshMgr().CreateMesh("arcademachine");
    arcadeMachineMesh.CreateFrom("data/models/arcademachine.fbx", WinUtil::Get().GetD3D());
    Setup(mModels[Modelid::ARCADEMACHINE], arcadeMachineMesh, 1.0f, Vector3(0, 0, 0), Vector3(PI / 2, PI / 2, 0));
    mLoadData.mLoadedSoFar++;

    Mesh& warehouseMesh = WinUtil::Get().GetD3D().GetMeshMgr().CreateMesh("warehouse");
    warehouseMesh.CreateFrom("data/models/warehouse.fbx", WinUtil::Get().GetD3D());
    Setup(mModels[Modelid::WAREHOUSE], warehouseMesh, 0.00025f, Vector3(0, 0, 0), Vector3(0, PI / 2, PI / 2));
    mLoadData.mLoadedSoFar++;
}

// Release function: Cleans up resources like sprite batch, font, and mode manager.
void Game::Release()
{
    // Safely release the sprite batch and font resources.
    delete mpSB;
    mpSB = nullptr;

    // Release all modes managed by Mode Manager.
    mMMgr.Release();

    lua_close(mpLuaState);
}

// Update function: Updates the game state including input, audio, and mode manager.
void Game::Update(float dTime)
{
    // If the game is still loading, update the loading screen logic instead of the game.
    if (mLoadData.mRunning)
    {
        mLoadData.Update(dTime);
        return;
    }

    // Update the global rotation angle for animating models.
    gAngle += dTime * 0.5f;

    // Update input, audio, and mode manager components.
    mGamepad.Update(dTime);
    mAudMgr.Update();
    mMMgr.Update(dTime);

#if defined(DEBUG) || defined(_DEBUG)
    // In debug mode, overlay additional information for development purposes.
    mDebugData.UpdateDebug(dTime);
#endif

    // Refresh input states for the next frame.
    mGamepad.RefreshState();
    mMKIn.RefreshState();

    if (mMMgr.GetCurrentModeStr() == IntroMode::MODE_NAME)
    {
        automationSong->Stop();
        trashySong->Stop();
    }
    else if (mMMgr.GetCurrentModeStr() != PlayMode::MODE_NAME && automationSong->GetState() != DirectX::SoundState::PLAYING)
    {
        automationSong->Play();
        trashySong->Stop();
    }
    else if (mMMgr.GetCurrentModeStr() == PlayMode::MODE_NAME && trashySong->GetState() != DirectX::SoundState::PLAYING)
    {
        automationSong->Stop();
        trashySong->Play();
    }
}

// Render function: Renders the game's 2D and 3D elements.
void Game::Render(float dTime)
{
    // If the game is still loading, render the loading screen instead of the game.
    if (mLoadData.mRunning)
    {
        mLoadData.Render(dTime, *mpSB);
        return;
    }

    // Render the game elements based on the current mode.
    RenderGame(dTime);
}

void Game::UpdateMusicVolume(float newMusicVolume)
{
    automationSong->SetVolume(newMusicVolume);
    trashySong->SetVolume(newMusicVolume);
}

// RenderGame function: Renders game elements based on the current mode.
void Game::RenderGame(float dTime)
{
    // Begin rendering with the DirectX toolkit, setting up 3D and 2D elements.
    MyD3D& d3d = WinUtil::Get().GetD3D();
    d3d.BeginRender(mSkyBoxColour);

    d3d.GetFX().SetPerFrameConsts(d3d.GetDeviceCtx(), mCamPos);

    // Setup the view and projection matrices for 3D rendering.
    CreateViewMatrix(d3d.GetFX().GetViewMatrix(), mCamPos, Vector3(0, 3, 0), Vector3(0, 1, 0));
    CreateProjectionMatrix(d3d.GetFX().GetProjectionMatrix(), 0.25f * PI, WinUtil::Get().GetD3D().GetAspectRatio(), 1, 1000.0f);
    Matrix w = Matrix::CreateRotationY(sinf(gAngle));
    d3d.GetFX().SetPerObjConsts(d3d.GetDeviceCtx(), w);

    // Setup and begin rendering 2D elements such as sprites.
    CommonStates dxstate(&d3d.GetDevice());
    mpSB->Begin(SpriteSortMode_Deferred, dxstate.NonPremultiplied(), &d3d.GetWrapSampler());
    mMMgr.Render(dTime, *mpSB);

#if defined(DEBUG) || defined(_DEBUG)
    // In debug mode, overlay additional information for development purposes.
    mDebugData.RenderDebug(dTime, *mpSB);
#endif

    // End the sprite batch and 3D rendering.
    mpSB->End();
    d3d.EndRender();

}

// LoadData struct: Simple struct that's meant to handle threading the loading process whilst 
// still rendering out information to the screen so the player knows it is loading assets/models
// 
// LoadData Constructor: Initializes the loading bar graphics and settings.
Game::LoadData::LoadData()
    : mLoadBorder(WinUtil::Get().GetD3D()),
    mLoadFill(WinUtil::Get().GetD3D())
{
    MyD3D& d3d = WinUtil::Get().GetD3D();

    // Load textures for the loading bar and orientate them.
    ID3D11ShaderResourceView* p = d3d.GetTexCache().LoadTexture(&d3d.GetDevice(), "ui/loadbar-border.dds");
    mLoadBorder.SetTex(*p);
    mLoadBorder.SetScale(Vector2(.1f, .1f));
    mLoadBorder.origin = mLoadBorder.GetTexData().dim / 2.0f;
    mLoadBorder.rotation = PI * 10.0f;

    p = d3d.GetTexCache().LoadTexture(&d3d.GetDevice(), "ui/loadbar-fill.dds");
    mLoadFill.SetTex(*p);
    mLoadFill.SetScale(Vector2(.1f, .1f));
    mLoadFill.origin = mLoadBorder.GetTexData().dim / 2.0f;
    mLoadFill.rotation = PI * 10.0f;

    mLoadFillRect = mLoadFill.GetTexRect();
    mMaxFillAmt = mLoadFillRect.right;

    mLoadFont = new SpriteFont(&WinUtil::Get().GetD3D().GetDevice(), L"data/fonts/vcr.spritefont");
}

// LoadData Destructor: Cleans up resources related to the loading bar.
Game::LoadData::~LoadData()
{
    // Safely release the loading font resource.
    delete mLoadFont;
    mLoadFont = nullptr;
}

// LoadData::Update function: Checks the logic behind our threaded function and checks for when it finishes
void Game::LoadData::Update(float dTime)
{
    // If the game is still loading, render the loading screen instead of the game.
    if (mRunning)
    {
        // If the loader is not ready, keep rendering the loading screen.
        if (!mLoader._Is_ready())
        {
            return;
        }
        // Transition out of the loading screen once loading is complete.
        else if (mLoadTransitionTimer <= mTimeTillTransition)
        {
            mLoadTransitionTimer += dTime;
            return;
        }
        // Finalize the loading process and prepare for the game rendering.
        mLoader.get();
        mRunning = false;
        return;
    }
}

// LoadData::Render function: Renders the loading scene with progress visuals.
void Game::LoadData::Render(float dTime, SpriteBatch& batch)
{
    // Begin rendering the loading scene with a black background.
    MyD3D& d3d = WinUtil::Get().GetD3D();
    d3d.BeginRender(Colours::Black);

    // Calculate the current fill amount for the loading bar based on the number of loaded models.
    // This interpolates the fill amount smoothly for a more visually pleasing loading bar.
    mCurrentFillAmt = Lerp(mCurrentFillAmt, mMaxFillAmt * ((float)mLoadedSoFar / (float)mTotalToLoad), dTime * 3);

    // Update the texture rectangle for the loading fill to reflect the current progress.
    mLoadFill.SetTexRect({ mLoadFillRect.left, mLoadFillRect.top, mCurrentFillAmt, mLoadFillRect.bottom });

    // Calculate the center position for the loading graphics.
    float w = (float)WinUtil::Get().GetClientWidth();
    float h = (float)WinUtil::Get().GetClientHeight();
    mLoadBorder.mPos = Vector2(w / 2, h / 2);
    mLoadFill.mPos = Vector2(w / 2, h / 2);

    // Begin rendering 2D elements/sprites.
    CommonStates dxstate(&d3d.GetDevice());
    batch.Begin(SpriteSortMode_Deferred, dxstate.NonPremultiplied(), &d3d.GetWrapSampler());

    // Draw the loading bar border and fill.
    mLoadFill.Draw(batch);
    mLoadBorder.Draw(batch);

    // Display "LOADING...." text with a variable number of periods to indicate progress.
    static int pips = 0;
    static float elapsed = 0;
    elapsed += dTime;
    if (elapsed > 0.25f) {
        pips++;
        elapsed = 0;
    }
    if (pips > 10)
        pips = 0;
    wstringstream ss;
    ss << L"LOADING";
    for (int i = 0; i < pips; ++i)
        ss << L'.';
    mLoadFont->DrawString(&batch, ss.str().c_str(), Vector2(w / 3.0f, h / 2.5f));

    // Display the load percentage text.
    ss.str(L"");
    ss << (int)(((float)mLoadedSoFar / (float)mTotalToLoad) * 100.0f) << L"%";
    mLoadFont->DrawString(&batch, ss.str().c_str(), Vector2(w / 1.6f, h / 2.5f));

    // In debug mode, overlay additional information for development purposes.
#if defined(DEBUG) || defined(_DEBUG)
    Game::Get().mDebugData.RenderDebug(dTime, batch);
#endif

    // End the sprite batch and finish rendering the loading scene.
    batch.End();
    d3d.EndRender();
}

#if defined(DEBUG) || defined(_DEBUG)
// DebugData constructor: Initialize debug data such as FPS text.
Game::DebugData::DebugData(MyD3D& d3d)
{
    int w, h;
    WinUtil::Get().GetClientExtents(w, h);

    // Get the spritefont
    SpriteFont* retrotechSF = d3d.GetFontCache().LoadFont(&d3d.GetDevice(), "retrotech.spritefont");

    mFpsText = new Text(d3d);
    mFpsText->SetFont(*retrotechSF);
    mFpsText->mPos = Vector2((float)w / 2.0f, (float)h * 0.95f);
    mFpsText->mActive = true;
}

// UpdateDebug function: Updates debug information on the screen in real-time.
void Game::DebugData::UpdateDebug(float dTime)
{
    Game& gm = Game::Get();

    if (gm.mMKIn.IsDown(VK_L) ||
        gm.mGamepad.IsConnected() && gm.mGamepad.GetButtonDown(XBtns.Y) && gm.mGamepad.RightStickX() > 0.9f)
        mDebugDrawColliders = !mDebugDrawColliders;

    mFpsText->mString = "FPS: ";
    mFpsText->mString += std::to_string((int)(1.0f / dTime));
    mFpsText->CentreOriginX();
}

// RenderDebug function: Renders debug information through text.
void Game::DebugData::RenderDebug(float dTime, SpriteBatch& batch) const
{
    mFpsText->Draw(batch);
}
#endif