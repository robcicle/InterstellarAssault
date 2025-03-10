#pragma once
#include "SpriteBatch.h"
#include "D3D.h"
#include "WindowUtils.h"

class Sprite;

// Animate class: By flicking through a series of sub-rectangle in an texture atlas
class Animate {
private:
	int mStart = 0, mStop = 0, mCurrent = 0; // Start, stop and current frame of animation
	float mRateSec = 0;		// How fast to play back
	float mElapsedSec = 0;	// How long the current frame has been on screen
	bool mLoop = false;		// Loop at the end ?
	bool mPlay = false;		// Should we be playing right now
	Sprite& mSpr;			// The parent sprite

public:
	Animate(Sprite& spr)
		:mSpr(spr)
	{}
	/*
	start	- FrameID starting at zero
	stop	- Final frameID
	rate	- Playback speed
	loop	- Loop at the end?
	*/
	void Init(int _start, int _stop, float _rate, bool _loop);
	// Choose the frame
	void Update(float _elapsedSec);
	// Start and stop
	void Play(bool go) {
		mPlay = go;
	}
	int GetFrameAmount() const { return mStop + 1; }
	Animate& operator=(const Animate& rhs);
};

// Sprite class: Manages the properties and rendering of a sprite, including texture, position, and animations.
class Sprite
{
private:
	ID3D11ShaderResourceView* mpTex;     // Pointer to the texture resource.
	MyD3D& mD3D;                         // Reference to the Direct3D object.
	RECTF mTexRect;                      // Texture rectangle for sprite animation or partial texture drawing.
	DirectX::SimpleMath::Vector2 scale;  // Scaling factor of the sprite.
	const TexCache::Data* mpTexData;     // Pointer to the texture data.
	Animate mAnim;                       // Animation handler for the sprite.

public:
	DirectX::SimpleMath::Vector2 mPos;   // Position of the sprite.
	DirectX::SimpleMath::Vector2 mVel;   // Velocity of the sprite for movement.
	float depth;                         // Depth for sorting sprites during rendering.
	DirectX::SimpleMath::Vector4 colour; // RGBA colour of the sprite.
	float rotation;                      // Rotation angle of the sprite in radians.
	DirectX::SimpleMath::Vector2 origin; // Origin point for rotation and scaling.

	// Constructor: Initializes default values for sprite properties.
	Sprite(MyD3D& d3d)
		: mPos(0, 0), mVel(0, 0), depth(0), mTexRect{ 0,0,0,0 }, colour(1, 1, 1, 1),
		rotation(0), scale(1, 1), origin(0, 0), mpTex(nullptr),
		mD3D(d3d), mAnim(*this), mpTexData(0)
	{}

	// Copy constructor: Creates a copy of an existing sprite.
	Sprite(const Sprite& rhs)
		: mD3D(rhs.mD3D), mAnim(*this)
	{
		(*this) = rhs;
	}

	// Assignment operator: Assigns one sprite to another.
	Sprite& operator=(const Sprite& rhs);

	// Draw method: Renders the sprite using the provided SpriteBatch.
	void Draw(DirectX::SpriteBatch& batch);

	// SetTex method: Changes the texture of the sprite. Optionally isolates part of the texture.
	void SetTex(ID3D11ShaderResourceView& tex, const RECTF& texRect = RECTF{ 0,0,0,0 });

	// SetTexRect method: Changes which part of the texture is displayed.
	void SetTexRect(const RECTF& texRect);

	// Scroll method: Scrolls the texture to create movement or parallax effects.
	void Scroll(float x, float y);

	// SetFrame method: Sets the current frame for sprite animation.
	void SetFrame(int id);

	// GetAnim method: Returns the animation handler for the sprite.
	Animate& GetAnim() {
		return mAnim;
	}

	// Getters for various sprite properties.
	const RECTF& GetTexRect() const { return mTexRect; }
	const TexCache::Data& GetTexData() const {
		assert(mpTexData);
		return *mpTexData;
	}
	ID3D11ShaderResourceView& GetTex() {
		assert(mpTex);
		return *mpTex;
	}
	void SetScale(const DirectX::SimpleMath::Vector2& s) {
		scale = s;
	}
	const DirectX::SimpleMath::Vector2& GetScale() const { return scale; }

	// GetScreenSize method: Returns the size of the sprite on the screen.
	DirectX::SimpleMath::Vector2 GetScreenSize() const {
		assert(mpTexData);
		return scale * mpTexData->dim;
	}
};