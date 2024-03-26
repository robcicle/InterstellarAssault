#include "Shelter.h"
#include "Player.h"
#include "Game.h"

#include <ostream>

using namespace DirectX;
using namespace DirectX::SimpleMath;


// Shelter Constructor: Initializes the shelter sprite and its properties.
Shelter::Shelter(MyD3D& d3d)
	: GameObj(d3d)
{
	// Load the texture for the shelter
	ID3D11ShaderResourceView* p = d3d.GetTexCache().LoadTexture(&d3d.GetDevice(), "sprites/sheltersheet.dds");

	// Set up the shelter sprite
	mSpr.SetTex(*p);
	mSpr.SetScale(Vector2(0.15f, 0.15f));
	mSpr.origin = Vector2((mSpr.GetTexData().dim.x / (float)mTextureStates) / 2.0f, mSpr.GetTexData().dim.y / 2.0f);
	RECTF shelterRect = mSpr.GetTexRect();
	// The sprite sheet isn't an equal amount so over time the pixels
	// from the next sprite seem to leak over, subtracting the 1 seems
	// to fix this issue
	mShelterRectRightAmt = (shelterRect.right / (float)mTextureStates) - 1;
	shelterRect.right = mShelterRectRightAmt;

	// Pixel seems to be wrapping from the top and left 
	// too now not sure why but this will fix it
	shelterRect.top += 5;
	shelterRect.left += 5;
	mSpr.SetTexRect(shelterRect);

	mActive = true;
}

// Update function: Updates the bounding box of the shelter based on its remaining life.
void Shelter::Update(float dTime)
{
	// Adjust the collider size based off of it's 
	// textureState and update its bounding box accordingly.
	float colliderYSlice = ((float)mLifes / (float)mTextureStates);

	Vector2 screenSize = mSpr.GetScreenSize();
	float prevScreenSizeY = screenSize.y;
	screenSize.y *= colliderYSlice;

	screenSize.x /= (float)mTextureStates;

	Vector2 spritePos = mSpr.mPos;
	if (prevScreenSizeY != screenSize.y)
		spritePos.y += (prevScreenSizeY - screenSize.y) / 2;

	mBoundingBox.UpdateBox(spritePos, screenSize, 0.95f);
}

// UpdateTexture function: Updates the sprite's texture rectangle to reflect damage.
void Shelter::UpdateTexture()
{
	MyD3D& d3d = WinUtil::Get().GetD3D();

	if (mTextureState < 9)
		mTextureState++;

	// Move the texture's rect to the next damaged sprite.
	RECTF newShelterRect = mSpr.GetTexRect();
	newShelterRect.left = mShelterRectRightAmt * mTextureState;
	newShelterRect.right = mShelterRectRightAmt * (mTextureState + 1);

	mSpr.SetTexRect(newShelterRect);
}

// Hit function: Reduces the shelter's life and updates its texture or deactivates it if life is zero.
void Shelter::Hit()
{
	// Reduce the life of the shelter
	mLifes--;

	// Check if it should be demolished
	if (mLifes < 1)
	{
		mActive = false;
		return;
	}

	// Update the texture to reflect the damage
	UpdateTexture();
}

// ShelterManager Constructor: Creates and positions multiple shelters in the game.
ShelterManager::ShelterManager(MyD3D& d3d, PlayMode& pM)
	: GameObj(d3d), mMyMode(&pM)
{
	// Position shelters based on the game's width and player's position.
	float perShelterPos = (float)WinUtil::Get().GetClientWidth() / GC::NUM_SHELTERS;
	Player* p = (Player*)mMyMode->FindFirst(typeid(Player), true);  // Retrieve the player from our PlayMode

	// Fill our array with shelters
	for (int i = 0; i < 4; i++)
	{
		mShelters[i] = new Shelter(d3d);
		mShelters[i]->SetPosition(Vector2((perShelterPos - (mShelters[i]->mSpr.GetScreenSize().x / (float)GC::SHELTER_TEXTURE_STATES) / 2.0f) * (float)(i + 1),
			p->mSpr.mPos.y - GC::SHELTER_OFFSET_Y));
	}
}

// Update function: Updates each active shelter.
void ShelterManager::Update(float dTime)
{
	// Loop through and update all active shelters.
	for (int i = 0; i < GC::NUM_SHELTERS; i++)
		if (mShelters[i]->mActive)
			mShelters[i]->Update(dTime);

}

// Render function: Renders each active shelter.
void ShelterManager::Render(float dTime, DirectX::SpriteBatch& batch)
{
	// Draw each active shelter in the game.
	for (int i = 0; i < GC::NUM_SHELTERS; i++)
		if (mShelters[i]->mActive)
			mShelters[i]->Render(dTime, batch);


}

// Hit function: Determines which shelter is closest to a given hit position and damages it.
void ShelterManager::Hit(float _hitX)
{
	// Find the closest active shelter to the hit position and trigger its Hit function.

	int closestIndex = -1;  // Index of the closest shelter
	float closestDistance = 9000;  // Initialize with max float value

	for (int i = 0; i < GC::NUM_SHELTERS; i++)
	{
		if (mShelters[i]->mActive)  // Check if the shelter is active
		{
			// Calculate the distance from _hitX to the center of the shelter
			float distance = std::abs(mShelters[i]->mSpr.mPos.x - _hitX);

			// Check if this shelter is closer than the currently closest
			if (distance < closestDistance)
			{
				closestDistance = distance;  // Update the closest distance
				closestIndex = i;  // Update the index of the closest shelter
			}
		}
	}

	// If a closest shelter wasn't found (closestIndex is -1), return out of the function
	if (closestIndex == -1)
	{
		return;
	}

	// Otherwise we tell the specific shelter to process the hit
	mShelters[closestIndex]->Hit();
}

// CheckLaserCollision function: Checks and handles collisions between lasers and shelters.
void ShelterManager::CheckLaserCollision(Laser* _laser)
{
	Game& gm = Game::Get();

	// Check for collisions between the laser and each shelter.
	for (int i = 0; i < GC::NUM_SHELTERS; i++)
	{
		if (mShelters[i] != nullptr && mShelters[i]->mActive)
		{
			// Check if one object is to the mLeft of the other
			if (mShelters[i]->mBoundingBox.mRight < _laser->mBoundingBox.mLeft || _laser->mBoundingBox.mRight < mShelters[i]->mBoundingBox.mLeft)
				continue; // No collision on the X axis

			// Check if one object is above the other
			if (mShelters[i]->mBoundingBox.mTop > _laser->mBoundingBox.mBottom || _laser->mBoundingBox.mTop > mShelters[i]->mBoundingBox.mBottom)
				continue; // No collision on the Y axis

			// If past this then it hit this shelter

			// Handle the hit and play sound effects.

			_laser->mActive = false;  // Deactivate the laser
			_laser->mBoundingBox.UpdateBox(0.0f, 0.0f, 0.0f, 0.0f);

			// Tell the shelter it was hit
			mShelters[i]->Hit();
			gm.GetAudMgr().CreateSFXInstance(AudioManager::SoundList::HIT); // Trigger hit sound
			return;
		}
	}
}

// CheckMissileCollision function: Checks and handles collisions between missiles and shelters.
void ShelterManager::CheckMissileCollision(Missile* _missile)
{
	Game& gm = Game::Get();

	// Check for collisions between the missile and each shelter.
	for (int i = 0; i < GC::NUM_SHELTERS; i++)
	{
		if (mShelters[i] != nullptr && mShelters[i]->mActive)
		{
			// Check if one object is to the mLeft of the other
			if (mShelters[i]->mBoundingBox.mRight < _missile->mBoundingBox.mLeft || _missile->mBoundingBox.mRight < mShelters[i]->mBoundingBox.mLeft)
				continue; // No collision on the X axis

			// Check if one object is above the other
			if (mShelters[i]->mBoundingBox.mTop > _missile->mBoundingBox.mBottom || _missile->mBoundingBox.mTop > mShelters[i]->mBoundingBox.mBottom)
				continue; // No collision on the Y axis

			// If past this then it hit this shelter
			
			// Handle the hit and play sound effects.

			_missile->mActive = false;  // Deactivate the laser
			gm.GetAudMgr().CreateSFXInstance(AudioManager::SoundList::EXPLOSION); // Trigger explosion sound


			// Tell the laser it was hit
			mShelters[i]->Hit();
			gm.GetAudMgr().CreateSFXInstance(AudioManager::SoundList::HIT); // Trigger hit sound
			return;
		}
	}
}

// AnySheltersLeft function: Checks if there are any active shelters left.
bool ShelterManager::AnySheltersLeft() const
{
	// Return true if there's at least one active shelter, otherwise false.
	for (int i = 0; i < GC::NUM_SHELTERS; i++)
		if (mShelters[i] != nullptr && mShelters[i]->mActive)
			return true;

	return false;
}
