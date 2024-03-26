#pragma once

#include "Sprite.h"
#include "D3D.h"


// BoundBox Struct: Represents a bounding box for collision detection. 
// It is based on the sprite's position and size on the screen. 
// In debug mode, it includes visual debugging capabilities.
struct BoundBox
{
public:
#if defined(DEBUG) || (_DEBUG)
    // Constructor for debug mode: Initializes the debug sprite.
    BoundBox();
#endif

    // Updates the bounding box edges to specified coordinates.
    void UpdateBox(float _left, float _top, float _right, float _bottom);

    // Updates the bounding box based on its position and size.
    // The _uniformScale parameter allows for scaling the bounding box size.
    // The _isOriginCentred parameter indicates if the sprite's origin is centred.
    void UpdateBox(DirectX::SimpleMath::Vector2& _pos, DirectX::SimpleMath::Vector2& _screenSize, float _uniformScale = 1.0f, bool _isOriginCentred = true);

    float mLeft = 0, mTop = 0, mRight = 0, mBottom = 0;  // Edges of the bounding box.

#if defined(DEBUG) || (_DEBUG)
    // DebugDraw method: Renders a visual representation of the bounding box for debugging.
    void DebugDraw(DirectX::SpriteBatch& batch);
private:
    Sprite mDebugBoxSpr;  // Sprite for visual representation of the bounding box in debug mode.
#endif
};

// GameObj Class: Represents any object in the game with a sprite.
// Game objects are used in game modes to update and render.
class GameObj
{
public:
    GameObj(MyD3D& d3d) : mSpr(d3d) {}
    virtual ~GameObj() {}

    // Pure virtual methods for updating and rendering - must be implemented by derived classes.
    virtual void Update(float dTime) = 0;
    virtual void Render(float dTime, DirectX::SpriteBatch& batch) {
        if (mActive)
            mSpr.Draw(batch);  // Draw the sprite if the object is active.
#if defined(DEBUG) || (_DEBUG)
        mBoundingBox.DebugDraw(batch);  // Draw the debug bounding box in debug mode.
#endif
    }

    // SetPosition function: Sets the position of the game object.
    void SetPosition(DirectX::SimpleMath::Vector2 _pos) { mSpr.mPos = _pos; }

    Sprite mSpr;           // Main sprite of the game object.
    bool mActive = false;  // Indicates whether the object is active (updates and renders).
    BoundBox mBoundingBox; // Bounding box for collision handling (optional use).
};