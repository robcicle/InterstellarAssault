#include "UIButton.h"


// Activate function: Triggers the button's action.
void UIButton::Activate() {
    if (mCallback) {
        mCallback();  // Execute the assigned callback function when the button is activated.
        Game::Get().GetAudMgr().CreateSFXInstance(AudioManager::SoundList::ENTER);
    }
}

// GetBounds method: Computes the bounding rectangle of the UIButton text.
DirectX::SimpleMath::Rectangle UIButton::GetBounds(bool _xCentred, bool _yCentred) const
{
    assert(mText.mSFont); // Ensure the sprite font is available.
    // Convert the string to a wide string format for compatibility with DirectX text measurement.
    std::wstring ws(mText.mString.begin(), mText.mString.end());

    // Measure the size of the string using the sprite font.
    DirectX::XMVECTOR size = mText.mSFont->MeasureString(ws.c_str());
    float width = DirectX::XMVectorGetX(size); // Extract the width from the size vector.
    float height = DirectX::XMVectorGetY(size); // Extract the height from the size vector.

    // Calculate the top-left position of the text, considering the centering options.
    float left = mText.mPos.x; // Starting horizontal position.
    float top = mText.mPos.y; // Starting vertical position.

    // Adjust the position if the text is centered along the x or y axis.
    if (_xCentred)
        left -= width / 2; // Center horizontally.
    if (_yCentred)
        top -= height / 2; // Center vertically.

    // Create and return the bounding rectangle.
    DirectX::SimpleMath::Rectangle bounds;
    bounds.x = static_cast<long>(left);
    bounds.y = static_cast<long>(top);
    bounds.width = static_cast<long>(width);
    bounds.height = static_cast<long>(height);

    return bounds;
}