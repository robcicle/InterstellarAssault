#include "GameObj.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;


#if defined(DEBUG) || (_DEBUG)
// Constructor for BoundBox: Initializes the debug sprite for visualizing the bounding box.
BoundBox::BoundBox()
    : mDebugBoxSpr(WinUtil::Get().GetD3D())
{
    MyD3D& d3d = WinUtil::Get().GetD3D();
    // Load the debug texture for drawing the bounding box.
    ID3D11ShaderResourceView* p = d3d.GetTexCache().LoadTexture(&d3d.GetDevice(), "debug/collision.dds", "DebugTexture");
    assert(p && "Failed to load debug texture");
    mDebugBoxSpr.SetScale(Vector2(1.0f, 1.0f));
    mDebugBoxSpr.SetTex(*p);
    mDebugBoxSpr.origin = mDebugBoxSpr.GetTexData().dim / 2.0f;
    mDebugBoxSpr.rotation = PI * 10.0f;
}
#endif

// UpdateBox function: Updates the bounding box with specific edges.
void BoundBox::UpdateBox(float _left, float _top, float _right, float _bottom)
{
    // Set the bounding box edges directly.
    mLeft = _left;
    mTop = _top;
    mRight = _right;
    mBottom = _bottom;
}

// UpdateBox function (overloaded): Updates the bounding box based on a central position and size.
void BoundBox::UpdateBox(Vector2& _pos, Vector2& _screenSize, float _uniformScale, bool _isOriginCentred)
{
    // Adjust the box dimensions based on the center and scale.
    _screenSize = (_isOriginCentred ? _screenSize / 2.0f : _screenSize) * _uniformScale;

    // Calculate the new edges of the bounding box.
    mLeft = _pos.x - _screenSize.x;
    mTop = _pos.y - _screenSize.y;
    mRight = _pos.x + _screenSize.x;
    mBottom = _pos.y + _screenSize.y;
}

#if defined(DEBUG) || (_DEBUG)
#include "Game.h"
// DebugDraw function: Renders the bounding box visually for debugging.
void BoundBox::DebugDraw(SpriteBatch& batch)
{
    // Check if collider debugging is enabled.
    if (!Game::Get().mDebugData.mDebugDrawColliders)
        return;

    // Calculate scales for width and height based on the bounding box dimensions.
    float adjustedWidth = mRight - mLeft;
    float adjustedHeight = mBottom - mTop;
    float scaleForWidth = adjustedWidth / mDebugBoxSpr.GetTexData().dim.x;
    float scaleForHeight = adjustedHeight / mDebugBoxSpr.GetTexData().dim.y;
    mDebugBoxSpr.SetScale(Vector2(scaleForWidth, scaleForHeight));

    // Set the position for the debug sprite to the center of the bounding box.
    Vector2 posToBe = { (mLeft + mRight) / 2.0f, (mTop + mBottom) / 2.0f };
    mDebugBoxSpr.mPos = posToBe;

    // Draw the debug sprite.
    mDebugBoxSpr.Draw(batch);
}
#endif