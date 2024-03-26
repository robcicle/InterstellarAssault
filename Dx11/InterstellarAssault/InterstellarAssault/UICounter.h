#pragma once

#include "Text.h"
#include "Sprite.h"
#include "UIButton.h"  // For the Callback definition and other dependencies.


// UICounter class: Manages a UI counter element with up and down arrow sprites and a numerical display.
class UICounter {
public:
    using Callback = UIButton::Callback;  // Reuse the Callback type from UIButton.

    // Constructor: Initializes the UICounter with text, arrows, value range, and a callback function.
    UICounter(MyD3D& d3d, Text text, Sprite upArrow, Sprite downArrow, int initialValue, int minValue, int maxValue, Callback _callback)
        : mText(text), mUpArrow(upArrow), mDownArrow(downArrow), mValue(initialValue),
        mMinValue(minValue), mMaxValue(maxValue), mCallback(std::move(_callback)) {

        UpdateDisplay();  // Update the display with the initial value.
    }

    // Increment function: Increases the counter's value within its maximum limit.
    void Increment();

    // Decrement function: Decreases the counter's value within its minimum limit.
    void Decrement();

    // UpdateDisplay function: Updates the counter's display with its current value.
    void UpdateDisplay();

    // TriggerCallback function: Executes the callback function, if defined.
    void TriggerCallback();

    // Draw function: Renders the counter's text and arrow sprites.
    void Draw(DirectX::SpriteBatch& batch);

    // SetPosition function: Sets the position of the counter and aligns the arrows.
    void SetPosition(const DirectX::SimpleMath::Vector2& position);

    // GetUpArrowBounds function: Returns the bounding rectangle of the up arrow sprite.
    DirectX::SimpleMath::Rectangle GetUpArrowBounds() const;

    // GetDownArrowBounds function: Returns the bounding rectangle of the down arrow sprite.
    DirectX::SimpleMath::Rectangle GetDownArrowBounds() const;

    // GetCurrentValue function: Returns the current value of the counter.
    int GetCurrentValue() const { return mValue; }

    Text GetText() const { return mText; }

private:
    Text mText;         // Text class instance for displaying the number.
    Sprite mUpArrow;    // Sprite for the up arrow.
    Sprite mDownArrow;  // Sprite for the down arrow.
    int mValue;         // Current value of the counter.
    int mMinValue;      // Minimum allowed value for the counter.
    int mMaxValue;      // Maximum allowed value for the counter.
    Callback mCallback; // Callback function to be triggered when the counter is changed.
};