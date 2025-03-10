#pragma once

#include "ModeMgr.h"
#include "Text.h"
#include "UIManager.h"


// MainMenuMode class: Manages the main menu of the game.
class MainMenuMode : public AMode
{
public:
    static const std::string MODE_NAME; // Static constant for the mode name.

    // MainMenuMode Constructor: Sets up the main menu with UI elements.
    MainMenuMode();

    // MainMenuMode Destructor: Cleans up resources used by the main menu mode.
    ~MainMenuMode();

    // Update function: Handles the update logic of the main menu mode.
    void Update(float dTime) override;

    // Render function: Renders the main menu UI elements.
    void Render(float dTime, DirectX::SpriteBatch& batch) override;

    // GetMName function: Returns the name of the mode.
    std::string GetMName() const override {
        return "MAINMENU";
    }

    // Reset function: Resets the main menu to its initial state.
    void Reset() override;

private:
    std::vector<Text*> mTexts;       // Collection of Text objects for rendering.

    Text* mTitleText = nullptr;      // Text object for the main menu title.
    UIManager mUIMgr;                // UI Manager for managing UI elements in the main menu.
};