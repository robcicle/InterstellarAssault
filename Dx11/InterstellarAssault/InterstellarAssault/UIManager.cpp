#include "UIManager.h"


// AddButton function: Adds a UIButton to the UIManager's list of buttons.
void UIManager::AddButton(UIButton button)
{
    mButtons.push_back(std::move(button)); // Move the provided button into the buttons list.
    UpdateButtonColors(); // Update the colors of all buttons after adding a new one.
}

// AddCounter function: Adds a UICounter to the UIManager's list of counters.
void UIManager::AddCounter(UICounter counter) {
    mCounters.push_back(std::move(counter)); // Add the counter to the collection.
}

// HandleInput function: Manages user input for UI elements including buttons and counters.
void UIManager::HandleInput()
{
    HandleButtonInput(); // Handle input related to buttons.
    HandleCounterInput(); // Handle input related to counters.
}

// Render function: Draws each UI element managed by the UIManager.
void UIManager::Render(DirectX::SpriteBatch& batch)
{
    for (auto& button : mButtons)
        button.mText.Draw(batch); // Draw each button.

    for (auto& counter : mCounters)
        counter.Draw(batch); // Draw each counter.
}

// SetSelected function: Sets the index of the currently selected button.
void UIManager::SetSelected(int index)
{
    if (static_cast<size_t>(index) >= 0 && static_cast<size_t>(index) < mButtons.size())
    {
        mSelectedIndex = index; // Set the selected button index if within valid range.
    }
}

// UpdateButtonColors function: Updates button colors based on their selection state.
void UIManager::UpdateButtonColors()
{
    for (size_t i = 0; i < mButtons.size(); ++i)
    {
        if (i == static_cast<size_t>(mSelectedIndex)) {
            // Set color based on whether the button is selected or being interacted with.
            mButtons[i].mText.colour = (mMouseIsOverButton || mUsingKeys) ? GC::UI_SELECT_COLOR : GC::UI_REGULAR_COLOR;
            if (mButtonDown && (mUsingKeys || mMouseIsOverButton))
                mButtons[i].mText.colour = GC::UI_INTERACTING_COLOR;
        }
        else {
            mButtons[i].mText.colour = GC::UI_REGULAR_COLOR; // Regular color for non-selected buttons.
        }
    }
}

// Reset function: Resets UIManager's flags and states.
void UIManager::Reset()
{
    mUsingKeys = false;          // Reset keyboard input flag.
    mMouseIsOverButton = false;  // Reset mouse hover flag.
    mJoystickMovedUp = false;    // Reset joystick upward movement flag.
    mJoystickMovedDown = false;  // Reset joystick downward movement flag.
    mPrevMousePos = Game::Get().mMKIn.GetMousePos(true);  // Update the previous mouse position.
}

// IsMouseOverButton function: Checks if the mouse is hovering over a specific UIButton.
bool UIManager::IsMouseOverButton(const DirectX::SimpleMath::Vector2& _mousePos, const UIButton& button)
{
    DirectX::SimpleMath::Rectangle buttonBounds = button.GetBounds();  // Retrieve the button's bounds.
    return buttonBounds.Contains(_mousePos);  // Check if mouse position is within the button's bounds.
}

// HandleButtonInput function: Handles the checks behind wether a button was activated/moved
void UIManager::HandleButtonInput()
{
    Game& gm = Game::Get();  // Get a reference to the game object

    // Get the current mouse position
    DirectX::SimpleMath::Vector2 mousePos = gm.mMKIn.GetMousePos(true);

    // Check if the mouse has moved
    if (mousePos != mPrevMousePos)
    {
        mMouseIsOverButton = false;  // Reset flag indicating if mouse is over a button
        mUsingKeys = false;
        mPrevMousePos = mousePos;  // Update the previous mouse position
    }

    if (gm.mMKIn.GetMouseButtonDown(MouseAndKeys::ButtonT::LBUTTON) || gm.mMKIn.IsDown(VK_SPACE) ||
        gm.mMKIn.IsDown(VK_RETURN) || gm.mGamepad.GetButtonDown(XBtns.A))
        mButtonDown = true;

    // Iterate over all buttons to determine mouse hover and click actions
    for (size_t i = 0; i < mButtons.size(); ++i) {
        if (IsMouseOverButton(mousePos, mButtons[i])) {
            // Mouse is currently over button i
            mSelectedIndex = i; // Set the selected button index
            mMouseIsOverButton = true; // Set flag to indicate mouse is over a button

            if (!mSelectSoundPlayed)
            {
                gm.GetAudMgr().CreateSFXInstance(AudioManager::SoundList::SELECT);
                mSelectSoundPlayed = true;
            }

            // Check if the left mouse button is released over a button
            if (gm.mMKIn.GetMouseButtonUp(MouseAndKeys::ButtonT::LBUTTON)) {
                mButtons[i].Activate(); // Activate the button
                mButtonDown = false;
            }
        }
        // Check if the left mouse button is released in general for colours
        if (gm.mMKIn.GetMouseButtonUp(MouseAndKeys::ButtonT::LBUTTON))
            mButtonDown = false;
    }

    if (!mMouseIsOverButton)
    {
        mSelectSoundPlayed = false;
    }

    // Check for keyboard or gamepad input to activate the selected button
    if (gm.mMKIn.IsUp(VK_SPACE) || gm.mMKIn.IsUp(VK_RETURN) || gm.mGamepad.GetButtonUp(XBtns.A) && mUsingKeys)
    {
        mButtons[mSelectedIndex].Activate(); // Activate the selected button
        mButtonDown = false;
    }

    UpdateButtonColors(); // Update the colors of all buttons

    // Handle navigation through buttons using keyboard or gamepad
    if (gm.mMKIn.IsDown(VK_UP) || gm.mMKIn.IsDown(VK_W) || gm.mGamepad.GetButtonDown(XBtns.DPadUp) || gm.mGamepad.LeftStickY() > 0.5f)
    {
        if (mJoystickMovedUp)
            return;

        mJoystickMovedUp = true;
        if (mUsingKeys)
        {
            mSelectedIndex--; // Move selection up

            if (mButtons.size() > 1)
                gm.GetAudMgr().CreateSFXInstance(AudioManager::SoundList::SELECT);
        }
        if (mSelectedIndex < 0)
        {
            mSelectedIndex = mButtons.size() - 1;  // Wrap around to the last button
        }
        UpdateButtonColors(); // Update colors after changing selection
        mUsingKeys = true;
    }
    else if (gm.mMKIn.IsDown(VK_DOWN) || gm.mMKIn.IsDown(VK_S) || gm.mGamepad.GetButtonDown(XBtns.DPadDown) || gm.mGamepad.LeftStickY() < -0.5f)
    {
        if (mJoystickMovedDown)
            return;

        mJoystickMovedDown = true;
        if (mUsingKeys)
        {
            mSelectedIndex++; // Move selection down

            if (mButtons.size() > 1)
                gm.GetAudMgr().CreateSFXInstance(AudioManager::SoundList::SELECT);
        }
        if ((size_t)mSelectedIndex >= mButtons.size())
        {
            mSelectedIndex = 0;  // Wrap around to the first button
        }
        UpdateButtonColors(); // Update colors after changing selection

        mUsingKeys = true;
    }
    else if (mJoystickMovedUp || mJoystickMovedDown)
    {
        // Reset joystick movement flags
        mJoystickMovedUp = false;
        mJoystickMovedDown = false;
    }
}

// HandleCounterInput function: Handles the checks behind wether a counter was clicked
void UIManager::HandleCounterInput()
{
    Game& gm = Game::Get(); // Get a reference to the game object
    DirectX::SimpleMath::Vector2 mousePos = gm.mMKIn.GetMousePos(true);

    for (auto& counter : mCounters) {
        // Check if the mouse clicks on the up arrow
        if (gm.mMKIn.GetMouseButtonDown(MouseAndKeys::ButtonT::LBUTTON) &&
            counter.GetUpArrowBounds().Contains(mousePos)) {
            counter.Increment();
        }

        // Check if the mouse clicks on the down arrow
        if (gm.mMKIn.GetMouseButtonDown(MouseAndKeys::ButtonT::LBUTTON) &&
            counter.GetDownArrowBounds().Contains(mousePos)) {
            counter.Decrement();
        }
    }
}
