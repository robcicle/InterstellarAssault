#include "CommonStates.h"
#include "Sprite.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;


// Initialize the animation with start frame, end frame, frame rate, and loop option.
void Animate::Init(int _start, int _stop, float _rate, bool _loop)
{
    mElapsedSec = 0;      // Reset the elapsed time counter.
    mStart = _start;      // Set the starting frame.
    mStop = _stop;        // Set the ending frame.
    mRateSec = _rate;     // Set the rate of animation in frames per second.
    mLoop = _loop;        // Set whether the animation should loop.
    mSpr.SetFrame(mStart); // Initialize the sprite to the starting frame.
    mCurrent = mStart;    // Initialize current frame to the start frame.
}

// Update the animation based on elapsed time.
void Animate::Update(float _elapsedSec)
{
    if (!mPlay) return; // Skip updating if animation is not playing.

    mElapsedSec += _elapsedSec; // Accumulate elapsed time.
    // Check if it's time to move to the next frame.
    if (mElapsedSec > (1.0f / mRateSec))
    {
        mElapsedSec = 0; // Reset elapsed time for next frame.
        mCurrent++;      // Move to the next frame.
        // Handle animation looping or stopping at the last frame.
        if (mCurrent > mStop)
        {
            mCurrent = mLoop ? mStart : mStop;
        }
        mSpr.SetFrame(mCurrent); // Update the sprite frame.
    }
}

// Assignment operator for Animate: Copies animation settings.
Animate& Animate::operator=(const Animate& rhs)
{
    // Copy animation parameters. Sprite reference cannot be copied, hence omitted.
    mStart = rhs.mStart;
    mStop = rhs.mStop;
    mCurrent = rhs.mCurrent;
    mRateSec = rhs.mRateSec;
    mElapsedSec = rhs.mElapsedSec;
    mLoop = rhs.mLoop;
    mPlay = rhs.mPlay;
    return *this;
}

// Assignment operator for Sprite: Handles copying of sprite properties.
Sprite& Sprite::operator=(const Sprite& rhs) {
    // Copy all sprite properties. D3D reference cannot be copied, hence omitted.
    mPos = rhs.mPos;
    mVel = rhs.mVel;
    depth = rhs.depth;
    mTexRect = rhs.mTexRect;
    colour = rhs.colour;
    rotation = rhs.rotation;
    scale = rhs.scale;
    origin = rhs.origin;
    mpTex = rhs.mpTex;
    mpTexData = rhs.mpTexData;
    mAnim = rhs.mAnim;
    return *this;
}

// Draw function: Renders the sprite using the provided SpriteBatch.
void Sprite::Draw(SpriteBatch& batch)
{
    // Draw the sprite with current properties.
    batch.Draw(mpTex, mPos, &(RECT)mTexRect, colour, rotation, origin, scale, DirectX::SpriteEffects::SpriteEffects_None, depth);
}

// SetTex function: Sets the texture and texture rectangle for the sprite.
void Sprite::SetTex(ID3D11ShaderResourceView& tex, const RECTF& texRect)
{
    mpTex = &tex;            // Set the texture.
    mTexRect = texRect;      // Set the texture rectangle.
    mpTexData = &mD3D.GetTexCache().Get(mpTex); // Retrieve texture data.

    // Set the full texture rectangle if not specified.
    if (mTexRect.left == mTexRect.right && mTexRect.top == mTexRect.bottom)
    {
        SetTexRect(RECTF{ 0,0,mpTexData->dim.x,mpTexData->dim.y });
    }
}

// SetTexRect function: Updates the texture rectangle for the sprite.
void Sprite::SetTexRect(const RECTF& texRect) {
    mTexRect = texRect;
}

// Scroll function: Scrolls the sprite's texture.
void Sprite::Scroll(float x, float y) {
    // Adjust texture rectangle for scrolling effect.
    mTexRect.left += x;
    mTexRect.right += x;
    mTexRect.top += y;
    mTexRect.bottom += y;
}

// SetFrame function: Sets the sprite's frame for animation.
void Sprite::SetFrame(int id)
{
    const TexCache::Data& data = mD3D.GetTexCache().Get(mpTex);
    SetTexRect(data.frames.at(id)); // Set the texture rectangle based on frame id.
}