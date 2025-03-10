#pragma once

#include "GameObj.h"
#include "PlayMode.h"
#include "Enemy.h"
#include "Game.h"

// Shelter class: Represents a protective structure in the game.
class Shelter : public GameObj
{
public:
    Shelter(MyD3D& d3d);
    void Update(float dTime) override;  // Update the shelter's state each frame.
    void UpdateTexture();               // Update the shelter's texture to reflect damage.
    void Hit();                         // Respond to being hit by an enemy or projectile.

private:
    int mLifes = 10;                 // The number of hits the shelter can take before being destroyed.
    int mTextureState = 0;           // Current state of the shelter's texture (to show damage).
    float mShelterRectRightAmt = 0;  // Amount to adjust the texture rectangle for damage states.

    const static int mTextureStates = 10;  // Number of texture states to show different damage levels.
};

// ShelterManager class: Manages a collection of shelters in the game.
class ShelterManager : public GameObj
{
public:
    ShelterManager(MyD3D& d3d, PlayMode& pM);  // Constructor to initialize the shelter manager.
    void Update(float dTime) override;         // Update each shelter managed by this class.
    void Render(float dTime, DirectX::SpriteBatch& batch) override;  // Render the shelters.

    void Hit(float _hitX);  // Handle a hit on the shelter closest to the specified X coordinate.

    void CheckLaserCollision(Laser* _laser);        // Check and handle collisions with lasers.
    void CheckMissileCollision(Missile* _missile);  // Check and handle collisions with missiles.

    bool AnySheltersLeft() const;  // Check if any shelters are still active.

private:
    PlayMode* mMyMode;  // Pointer to the game mode that owns this shelter manager.

    Shelter* mShelters[GC::NUM_SHELTERS];  // Array of shelters managed by this class.
};