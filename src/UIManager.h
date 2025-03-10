#pragma once

#include "UIButton.h"
#include "Game.h"
#include "UICounter.h"


// UIManager class: Manages user interface elements such as buttons and counters.
class UIManager {
public:
    // Constructor: Initializes the UIManager with default values.
    UIManager() : mSelectedIndex(0), mPrevMousePos(Game::Get().mMKIn.GetMousePos(true)) {}

    // AddButton function: Adds a UIButton to the UI Manager's collection.
    void AddButton(UIButton button);

    // AddCounter function: Adds a UICounter to the UI Manager's collection.
    void AddCounter(UICounter counter);

    // HandleInput function: Manages user input for interacting with UI elements.
    void HandleInput();

    // Render function: Renders all UI elements managed by the UIManager.
    void Render(DirectX::SpriteBatch& batch);

    // SetSelected function: Sets the currently selected button based on the provided index.
    void SetSelected(int index);

    // UpdateButtonColors function: Updates the colors of buttons based on their state.
    void UpdateButtonColors();

    // Reset function: Resets the UI Manager's member variables to their default states.
    void Reset();

    // IsMouseOverButton function: Checks if the mouse is hovering over a specific UIButton.
    bool IsMouseOverButton(const DirectX::SimpleMath::Vector2& _mousePos, const UIButton& button);

    // GetCounterValue function: Retrieves the value of a specified UICounter by index.
    int GetCounterValue(int _index) { return mCounters[_index].GetCurrentValue(); }

private:
    // HandleButtonInput function: Manages input specific to buttons.
    void HandleButtonInput();

    // HandleCounterInput function: Manages input specific to counters.
    void HandleCounterInput();

    std::vector<UIButton> mButtons;              // Collection of UIButtons.
    std::vector<UICounter> mCounters;            // Collection of UICounters.

    int mSelectedIndex = -1;                     // Index of the currently selected button.

    bool mUsingKeys = false;                     // Flag for keyboard input usage.

    DirectX::SimpleMath::Vector2 mPrevMousePos;  // Stores the previous mouse position.
    bool mMouseIsOverButton = false;             // Flag for mouse hover over button detection.

    bool mJoystickMovedUp = false;               // Flag for joystick upward movement.
    bool mJoystickMovedDown = false;             // Flag for joystick downward movement.

    bool mButtonDown = false;                    // Flag for button press state.
    bool mSelectSoundPlayed = false;             // Flag for select sound playback.
};