#pragma once

#include "PlayMode.h"
#include "GameObj.h"
#include "Player.h"
#include "Game.h"
#include <random>



// Laser class: Also inherits from GameObj and is "shot" from the
// squid enemy and is to collide with the player or shelter
class Laser : public GameObj
{
public:
    Laser(MyD3D& d3d);                  // Constructor to initialize the laser
    void Update(float dTime) override;  // Update function called every frame to handle laser movement and animation

private:
    int mScreenHeight;
};

// Enemy class: Inherits from GameObj and represents individual enemy entities in the game
class Enemy : public GameObj
{
public:
    // Enum to represent different types of enemies
    enum EnemyType { OCTOPUS, CRAB, SQUID, UFO };

public:
    Enemy(EnemyType _eTypeToBe); // Constructor to initialize an enemy with a specific type
    ~Enemy(); // Destructor for clean-up
    void Update(float dTime) override;  // Update function to handle enemy behavior each frame

    EnemyType GetEnemyType() const { return mEType; } // Getter to retrieve the enemy's type

    Laser* mLaser = nullptr;
private:
    EnemyType mEType = EnemyType::OCTOPUS; // Variable to store the type of enemy
    float mShootTimer = 0;                   // For keeping track of the amount of time

    std::random_device mRD;
    std::mt19937 mMt{ mRD() };
};

// EnemyManager class: Inherits from GameObj and manages all enemy entities in the game
class EnemyManager : public GameObj
{
public:
    EnemyManager(MyD3D& d3d); // Constructor to initialize the manager
    ~EnemyManager(); // Destructor for clean-up

    void Init(); // function to initialize or reset the enemies

    void Update(float dTime) override; // Update function to handle enemy movement and game logic
    void Render(float dTime, DirectX::SpriteBatch& batch) override; // Render function to draw enemies

    // function to link the manager to the mode it belongs to
    void SetMode(PlayMode& pm) {
        mMyMode = &pm;
    }

private:
    void CheckCollisions();  // function to handle collision detection and response

    void MissileHit(Enemy* _enemy, GameObj* _missile);  // function used for deleting/exploding the enemy & missiles
    void LaserHit(Player* _player, Laser* _laser);      // function used for hitting the player with a laser

    int GetETypePoints(Enemy::EnemyType _eType) const; // function to determine the points value for an enemy

    PlayMode* mMyMode = nullptr;  // Pointer to the mode that owns this manager

    Enemy* mEnemies[GC::NUM_ENEMIES];  // Array to store all enemy entities
    Enemy* mUfoEnemy = nullptr;        // Specifically our UFO enemy because it does things different

    std::random_device mRD;
    std::mt19937 mMt{ mRD() };

    int mDirection;        // Direction of enemy movement (1 for left, -1 for right)
    int mUfoDirection;     // Direction of ufo movement (1 for left, -1 for right)
    float mSpeed = 10.0f;  // Speed of enemy movement

    int mActiveEnemies = 0;  // Int to hold count of how many enemies are active

    bool mUfoActive = true;

    // Screen boundaries for enemy movement
    float mLeftLimit = 50.0f;    // Left boundary
    float mRightLimit = 500.0f;  // Right boundary
};