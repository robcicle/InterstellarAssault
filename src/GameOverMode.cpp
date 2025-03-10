#include "GameOverMode.h"
#include "Game.h"
#include "WindowUtils.h"
#include "MainMenuMode.h"

#include <vector>
#include "PlayMode.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

// Static definition of the mode name for the game over mode.
const std::string GameOverMode::MODE_NAME = "GAMEOVER";

// Constructor: Initializes the UI elements and text for the game over screen.
GameOverMode::GameOverMode()
    : mGameOverText(WinUtil::Get().GetD3D())
{
    MyD3D& d3d = WinUtil::Get().GetD3D();

    int w, h;
    WinUtil::Get().GetClientExtents(w, h);

    // Set up the 'Game Over' text with position and font
    SpriteFont* retrotechSF = d3d.GetFontCache().LoadFont(&d3d.GetDevice(), "retrotech.spritefont");
    SpriteFont* lRetrotechSF = d3d.GetFontCache().LoadFont(&d3d.GetDevice(), "retrotech-60.spritefont");

    mGameOverText.SetFont(*lRetrotechSF);
    mGameOverText.mActive = true;
    mGameOverText.mString = "Game Over";
    mGameOverText.mPos = Vector2((float)w / 2.0f, (float)h * 0.1f);
    mGameOverText.CentreOriginX();

    // Initialize UI buttons with callbacks for 'Retry', 'Main Menu', and 'Quit'
    UIButton retryButton(d3d, [&]() { Game::Get().GetModeMgr().SwitchMode(PlayMode::MODE_NAME); },
        retrotechSF, Vector2((float)w / 2.0f, (float)h * 0.3f), "RETRY");
    retryButton.mText.CentreOriginX();
    mUIMgr.AddButton(retryButton);

    UIButton menuButton(d3d, [&]() { Game::Get().GetModeMgr().SwitchMode(MainMenuMode::MODE_NAME); },
        retrotechSF, Vector2((float)w / 2.0f, (float)h * 0.4f), "MAIN MENU");
    menuButton.mText.CentreOriginX();
    mUIMgr.AddButton(menuButton);

    UIButton quitButton(d3d, [&]() { PostQuitMessage(0); },
        retrotechSF, Vector2((float)w / 2.0f, (float)h * 0.5f), "QUIT");
    quitButton.mText.CentreOriginX();
    mUIMgr.AddButton(quitButton);
}

// Update function: Handles input processing for UI elements.
void GameOverMode::Update(float dTime)
{
    // Process input for the UI Manager
    mUIMgr.HandleInput();
}

// Render function: Draws game over text and UI elements.
void GameOverMode::Render(float dTime, DirectX::SpriteBatch& batch)
{
    // Draw the 'Game Over' text and UI buttons
    mGameOverText.Draw(batch);
    mUIMgr.Render(batch);
}

// Reset function: Used when switching modes, we use for reseting the UI Manager.
void GameOverMode::Reset()
{
    mUIMgr.Reset();
}
