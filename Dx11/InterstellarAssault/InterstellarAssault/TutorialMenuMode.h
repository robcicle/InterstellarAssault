#pragma once

#include "ModeMgr.h"
#include "Text.h"
#include "UIManager.h"

#include <sstream>

// TutorialMenuMode class: Displays info on how to play the game.
class TutorialMenuMode : public AMode
{
public:
    static const std::string MODE_NAME; // Static constant for the mode name.

    // Constructor: Initializes the tutorial menu mode.
    TutorialMenuMode();

    // Destructor: Cleans up resources used by the tutorial menu mode.
    ~TutorialMenuMode();

    // Update function: Updates the tutorial menu mode logic.
    void Update(float dTime) override;

    // Render function: Renders the tutorial menu UI elements.
    void Render(float dTime, DirectX::SpriteBatch& batch) override;

    // GetMName function: Returns the name of the mode.
    std::string GetMName() const override {
        return "TUTORIALMENU";
    }

    // Reset function: Resets the tutorial menu to its initial state.
    void Reset() override;

private:
    Text* mTutorialTitleText = nullptr;  // Text object for the tutorial menu title.

    UIManager mUIMgr;  // UI Manager for managing UI elements in the tutorial menu.
    bool mIsKeyboardSelected = true;  // Flag to see if the user wants to see keyboard tutorial

    Text* mKeyboardText = nullptr;    // Text object for the tutorial menu title.
    Text* mControllerText = nullptr;  // Text object for the tutorial menu title.
};