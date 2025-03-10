#include "UICounter.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;


// SetPosition method: Sets the central position of the UICounter and aligns the arrow sprites.
void UICounter::SetPosition(const Vector2& position)
{
    // Set the central position of the text.
    mText.mPos = position;

    // Retrieve the size of the text for alignment.
    Vector2 textSize = mText.GetSize();

    // Position the up arrow to the right of the text, accounting for an offset.
    mUpArrow.mPos.x = mText.mPos.x + textSize.x / 2 + GC::UI_ARROW_OFFSET;
    mUpArrow.mPos.y = mText.mPos.y;

    // Position the down arrow to the left of the text, accounting for an offset.
    mDownArrow.mPos.x = mText.mPos.x - textSize.x / 2 - GC::UI_ARROW_OFFSET;
    mDownArrow.mPos.y = mText.mPos.y;
}

void UICounter::Increment()
{
    if (mValue < mMaxValue) {
        mValue++;
        UpdateDisplay();
        TriggerCallback();
    }
}

void UICounter::Decrement()
{
    if (mValue > mMinValue) {
        mValue--;
        UpdateDisplay();
        TriggerCallback();
    }
}

void UICounter::UpdateDisplay()
{
    mText.mString = std::to_string(mValue); // Convert the current value to string.
    mText.CentreOriginX(); // Center the text horizontally.
    mText.CentreOriginY(); // Center the text vertically.
}

void UICounter::TriggerCallback()
{
    if (mCallback) {
        mCallback();  // Call the callback function.
        Game::Get().GetAudMgr().CreateSFXInstance(AudioManager::SoundList::ENTER);
    }
}

void UICounter::Draw(DirectX::SpriteBatch& batch)
{
    mText.Draw(batch);     // Draw the counter's text.
    mUpArrow.Draw(batch);  // Draw the up arrow sprite.
    mDownArrow.Draw(batch); // Draw the down arrow sprite.
}

// GetUpArrowBounds method: Calculates the bounding rectangle for the up arrow sprite.
DirectX::SimpleMath::Rectangle UICounter::GetUpArrowBounds() const
{
    // Determine the dimensions of the up arrow sprite.
    Vector2 size = mUpArrow.GetScreenSize();
    float width = size.x;
    float height = size.y;

    // Calculate and return the top-left position and dimensions of the up arrow.
    float left = mUpArrow.mPos.x - (width / 2); // Assuming origin is at the center.
    float top = mUpArrow.mPos.y - (height / 2);
    DirectX::SimpleMath::Rectangle bounds(static_cast<long>(left), static_cast<long>(top), static_cast<long>(width), static_cast<long>(height));
    return bounds;
}

// GetDownArrowBounds method: Calculates the bounding rectangle for the down arrow sprite.
DirectX::SimpleMath::Rectangle UICounter::GetDownArrowBounds() const
{
    // Determine the dimensions of the down arrow sprite.
    Vector2 size = mDownArrow.GetScreenSize();
    float width = size.x;
    float height = size.y;

    // Calculate and return the top-left position and dimensions of the down arrow.
    float left = mDownArrow.mPos.x - (width / 2); // Assuming origin is at the center.
    float top = mDownArrow.mPos.y - (height / 2);
    DirectX::SimpleMath::Rectangle bounds(static_cast<long>(left), static_cast<long>(top), static_cast<long>(width), static_cast<long>(height));
    return bounds;
}