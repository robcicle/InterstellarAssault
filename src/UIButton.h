#pragma once

#include "Text.h"
#include "Game.h"


// UIButton class: Represents a UI button that extends the Text class.
class UIButton
{
public:
    Text mText;
public:
    using Callback = std::function<void()>;  // Define a callback type for button actions.

    // Constructor: Initializes a UIButton with a callback function.
    UIButton(Text _text, Callback _callback)
        : mText(_text), mCallback(std::move(_callback))
    {
        mText = _text;
        mText.mActive = true;  // Set the button as active upon creation.
    }

    // Overloaded constructor: Initializes a UIButton with additional properties.
    UIButton(MyD3D& d3d, Callback _callback, DirectX::SpriteFont* _spriteFont,
        DirectX::SimpleMath::Vector2 _pos, std::string _string)
        : mText(d3d), mCallback(std::move(_callback))
    {
        mText.SetFont(*_spriteFont);  // Set the font for the button text.
        mText.mPos = _pos;            // Set the position of the button.
        mText.mString = _string;      // Set the display string for the button.
        mText.mActive = true;         // Set the button as active.
    }

    // Activate function: Triggers the button's action.
    virtual void Activate();

    // GetBounds function: Returns the bounds of the UIButton for collision detection or layout.
    DirectX::SimpleMath::Rectangle GetBounds(bool _xCentred = true, bool _yCentred = false) const;

private:
    Callback mCallback;  // A callback function to be called when the button is activated.
};