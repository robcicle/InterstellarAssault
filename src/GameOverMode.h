#pragma once

#include "D3D.h"
#include "SpriteBatch.h"
#include "Sprite.h"
#include "ModeMgr.h"
#include "UIManager.h"


// GameOverMode class: Represents the game over state of the game, derived from AMode.
class GameOverMode : public AMode
{
public:
    static const std::string MODE_NAME;  // Static variable holding the name of this mode.

    GameOverMode();   // Constructor: Initializes resources needed for this mode.

    // Update function: Handles input and transitions between modes per frame.
    void Update(float dTime) override;

    // Render function: Displays game over text and instructions per frame.
    void Render(float dTime, DirectX::SpriteBatch& batch) override;

    // GetMName function: Returns the name of the mode.
    std::string GetMName() const override {
        return "GAMEOVER";
    }

    // Reset function: Use it to reset the UI Mgr whenever we switch modes
    void Reset() override;

private:
    Text mGameOverText;  // Our "Game Over" text
    UIManager mUIMgr;    // UI manager for handling buttons
};