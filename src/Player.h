#pragma once

#include "GameObj.h"
#include "PlayMode.h"


// Player class: Inherits from GameObj and is all of the logic behind 
// the user being able to move around and shoot whilst in the PlayMode.
class Player : public GameObj
{
public:
	Player(MyD3D& d3d);                 // Constructor to initialize the player
	~Player();
	void Update(float dTime) override;  // Update function called every frame to handle player movement and actions
	void Render(float dTime, DirectX::SpriteBatch& batch) override;

	void Hit();  // Logic for what happens when a player is hit

	int GetLifes() const { return mLifes; }

	// Link the player to the game mode it belongs to
	void SetMode(PlayMode& pm) {
		mMyMode = &pm;
	}

	void AddLifes(int amount) { mLifes += amount; }
	void SetLifes(int amount) { mLifes = amount; }

	bool IsAlive() const { 
		if (mLifes > 0)
			return true;
		
		return false;
	}

private:
	RECTF mPlayArea;			   // Defines the area within which the player can move
	PlayMode* mMyMode = nullptr;  // Pointer to the game mode that owns this player
	float mFireTimer = 0;		   // Timer to manage the delay between firing missiles

	int mLifes = 3;  // Player's current and starting lifes
	const float mTimeToRecover = 2.5f;  // Time it takes for the little flash animation to happen
	float mRecoveryTimer = 0.0f;
	bool mRecovering = false;

	void Init();  // Initialize or reset the player's state and position
};

// Missile class: Also inherits from GameObj and is "shot" from the
// players ship and then continuously moves up until it hits or goes off screen.
class Missile : public GameObj
{
public:
	Missile(MyD3D& d3d);                // Constructor to initialize the missile
	void Update(float dTime) override;  // Update function called every frame to handle missile movement and animation

	// Link the missile to the game mode it belongs to
	void SetMode(PlayMode& pm) {
		mMyMode = &pm;
	}
private:
	PlayMode* mMyMode = nullptr;  // Pointer to the game mode that owns this player
};