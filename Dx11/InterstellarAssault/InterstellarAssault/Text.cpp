#include "Text.h"

using namespace DirectX;


// Copy assignment operator: Assigns the properties of one Text instance to another.
Text& Text::operator=(const Text& rhs)
{
	mPos = rhs.mPos;			  // Copy the position.
	mString = rhs.mString;		  // Copy the string content.
	depth = rhs.depth;			  // Copy the depth value.
	colour = rhs.colour;		  // Copy the colour.
	rotation = rhs.rotation;	  // Copy the rotation angle.
	scale = rhs.scale;			  // Copy the scale factor.
	origin = rhs.origin;		  // Copy the origin point.
	mSFont = rhs.mSFont;		  // Copy the SpriteFont pointer.
	mSFontData = rhs.mSFontData;  // Copy the font data pointer.
	mActive = rhs.mActive;        // Copy the mActive state.
	return *this;				  // Return a reference to the current object.
}

// Draw function: Renders the text using the provided SpriteBatch.
void Text::Draw(SpriteBatch& batch) const
{
	if (mString.size() <= 0) // If the string is empty, no need to draw anything.
		return;

	// Convert the std::string to std::wstring as that's what DirectXTK uses.
	std::wstring ws = std::wstring(mString.begin(), mString.end());

	// Draw the string through our SpriteFont using the reference 
	// to the SpriteBatch and all of our properties.
	mSFont->DrawString(&batch, ws.c_str(), mPos, colour, rotation, origin, scale, SpriteEffects::SpriteEffects_None, depth);
}

// GetSize function: Calculates the size of the text with string considered
DirectX::SimpleMath::Vector2 Text::GetSize() const
{
	std::wstring ws(mString.begin(), mString.end());
	DirectX::XMVECTOR size = mSFont->MeasureString(ws.c_str());
	return DirectX::SimpleMath::Vector2(DirectX::XMVectorGetX(size), DirectX::XMVectorGetY(size));
}

// SetFont function: Associates a new SpriteFont with the text.
void Text::SetFont(SpriteFont& font)
{
	mSFont = &font; // Set the new SpriteFont.
	// Retrieve and set the associated font data from the font cache.
	mSFontData = &mD3D.GetFontCache().Get(mSFont);
}