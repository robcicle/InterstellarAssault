#pragma once

#include <SimpleMath.h>
#include <SpriteFont.h>

#include "D3D.h"


// Text class: Represents and manages the needed properties and behaviors of 
// text elements in the game and handles drawing it with the SpriteBatch.
class Text
{
private:
	MyD3D& mD3D;                        // Reference to the D3D object for rendering.
	const FontCache::Data* mSFontData;  // Pointer to the font data associated with this text.

public:
	DirectX::SimpleMath::Vector2 mPos;    // Position of the text.
	std::string mString;                  // String of text to be drawn.
	float depth;                          // Depth value for layering.
	DirectX::SimpleMath::Vector4 colour;  // Colour of the text.
	float rotation;                       // Rotation of the text.
	DirectX::SimpleMath::Vector2 origin;  // Origin point of the text.
	float scale;                          // Scale factor for the text.
	bool mActive;                         // Wether the text is active, should render or not 
	DirectX::SpriteFont* mSFont;          // Pointer to the SpriteFont used for this text.

	// Constructor: Initializes texts variables and holds the reference to MyD3D.
	Text(MyD3D& d3d)
		: mPos(0, 0), depth(0), colour(Colours::White), rotation(0),
		scale(1), origin(0, 0), mSFont(nullptr), mD3D(d3d), mSFontData(0),
		mActive(false)
	{}
	Text(MyD3D& d3d, DirectX::SimpleMath::Vector2 _pos, std::string _string, DirectX::SpriteFont* _spriteFont, bool _active)
		: mPos(_pos), depth(0), colour(Colours::White), rotation(0),
		scale(1), origin(0, 0), mSFont(_spriteFont), mD3D(d3d), mSFontData(0),
		mActive(_active)
	{
		mString = _string;
	}

	// Copy constructor: Creates a new Text instance as a copy of an existing one.
	Text(const Text& rhs)
		: mD3D(rhs.mD3D)
	{
		(*this) = rhs;
	}

	// Copy assignment operator: Assigns the properties of one Text instance to the current instances.
	Text& operator=(const Text& rhs);

	// Draw function: Renders the text using the provided SpriteBatch.
	void Draw(DirectX::SpriteBatch& batch) const;

	// GetSize function: Calculates the size of the text with string considered
	DirectX::SimpleMath::Vector2 GetSize() const;

	// CentreOriginX function: Calculates and sets the horizontal/X center of the text's origin.
	void CentreOriginX() {
		if (mString.size() <= 0)
			return;

		std::wstring ws = std::wstring(mString.begin(), mString.end());
		origin.x = GetSize().x / 2.0f;
	}

	// CentreOriginY function: Calculates and sets the vertical/Y center of the text's origin.
	void CentreOriginY() {
		if (mString.size() <= 0)
			return;

		std::wstring ws = std::wstring(mString.begin(), mString.end());
		origin.y = GetSize().y / 2.0f;
	}

	// SetFont function: Associates a new SpriteFont with the text.
	void SetFont(DirectX::SpriteFont& font);

	// GetFontData function: Returns the FontCache data associated with the text.
	const FontCache::Data& GetFontData() const {
		assert(mSFontData);
		return *mSFontData;
	}
};