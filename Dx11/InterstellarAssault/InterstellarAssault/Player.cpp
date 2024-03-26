#include "Player.h"
#include "Game.h"
#include "Shelter.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;


// Constructor for Player: Initializes a player object
Player::Player(MyD3D& d3d)
	: GameObj(d3d)
{
	Init();
}

// Initialize or reset the player's state and position
void Player::Init()
{
	MyD3D& d3d = WinUtil::Get().GetD3D();
	// Load and orient the ship sprite
	ID3D11ShaderResourceView* p = d3d.GetTexCache().LoadTexture(&d3d.GetDevice(), "sprites/ship.dds");
	mSpr.SetTex(*p);
	mSpr.SetScale(Vector2(0.1f, 0.1f));
	mSpr.origin = mSpr.GetTexData().dim / 2.0f;
	mSpr.rotation = PI * 10.0f;

	// Set up the play area boundaries
	int w, h;
	WinUtil::Get().GetClientExtents(w, h);
	mPlayArea.left = mSpr.GetScreenSize().x * 0.6f;
	mPlayArea.top = mSpr.GetScreenSize().y * 0.6f;
	mPlayArea.right = w - mPlayArea.left;
	mPlayArea.bottom = h * 0.9f;
	mSpr.mPos = Vector2((mPlayArea.left + mPlayArea.right) / 2.0f, mPlayArea.bottom);

	// Set the initial fire timer for missile shooting
	mFireTimer = GetClock() + GC::FIRE_DELAY;
}

// Update function: Handle the logic behind the player's movement, shooting, health, etc.
void Player::Update(float dTime)
{
	Game& gm = Game::Get();

	// If we're recovering/just been hit by a laser
	if (mRecovering || mMyMode->IsGameOver())
	{
		// Let's tick the timer that when drawing
		// we will use to flash the player and show
		// that it is currently recovering.
		mRecoveryTimer += dTime;
		if (mRecoveryTimer >= mTimeToRecover && !mMyMode->IsGameOver())
			mRecovering = false;

		return;
	}


	// Get the current position of the mouse
	Vector2 mouse{ gm.mMKIn.GetMousePos(false) };

	// Check if movement keys are pressed
	bool keypressed = gm.mMKIn.IsPressed(VK_RIGHT) || gm.mMKIn.IsPressed(VK_LEFT) ||
		gm.mMKIn.IsPressed(VK_D) || gm.mMKIn.IsPressed(VK_A);
	// Check if the gamepad is connected and active
	bool controllerActive = gm.mGamepad.IsConnected();

	// Fire a missile if the fire button is pressed and the fire delay has passed
	if ((gm.mMKIn.IsPressed(VK_SPACE) || gm.mMKIn.GetMouseButton(MouseAndKeys::ButtonT::LBUTTON) || gm.mGamepad.GetButtonPressed(XBtns.A)) || gm.mGamepad.RightTrigger() > 0.5f)
	{
		// Just a little shoot limit that stops us from shooting
		// immediatly when we transition from the main menu to game
		if (GetClock() > mFireTimer)
		{
			// Find an inactive missile and activate it
			GameObj* pM = mMyMode->FindFirst(typeid(Missile), false);
			if (pM) 
			{
				pM->mActive = true;
				// Set the missile's position slightly ahead of the player's ship
				pM->mSpr.mPos = Vector2(mSpr.mPos.x, mSpr.mPos.y - mSpr.GetScreenSize().y/2);
				pM->mSpr.rotation = PI * 0.0f;
				// Reset the fire timer
				mFireTimer = GetClock() + GC::FIRE_DELAY;
				// Play the shooting sound effect
				gm.GetAudMgr().CreateSFXInstance(AudioManager::SoundList::MISSILE_SHOOT);
			}

		}
	}

	// Initialize a vector to store the change in position
	Vector2 pos(0, 0);

	// Handle movement via keyboard
	if (keypressed)
	{
		// Move right
		if (gm.mMKIn.IsPressed(VK_RIGHT) || gm.mMKIn.IsPressed(VK_D))
			pos.x += GC::SHIP_SPEED * dTime;
		// Move left
		if (gm.mMKIn.IsPressed(VK_LEFT) || gm.mMKIn.IsPressed(VK_A))
			pos.x -= GC::SHIP_SPEED * dTime;
	}
	// If the keyboard isn't being used then check the gamepad
	else if (controllerActive)
	{
		// If left stick is not in the deadzone, move according to its X axis
		if (!gm.mGamepad.LStickInDeadzone())
		{
			pos.x += gm.mGamepad.LeftStickX() * GC::SHIP_SPEED * dTime;
		}
	}

	// Apply the position changes to the current position
	pos += mSpr.mPos;

	// Constrain the player's position to the play area
	pos.x = std::clamp(pos.x, mPlayArea.left, mPlayArea.right);
	pos.y = std::clamp(pos.y, mPlayArea.top, mPlayArea.bottom);

	// Update the player's position
	mSpr.mPos = pos;

	// Update our bounding boxes left,right,top,bottom to match
	// our laser's position with scale factored in
	mBoundingBox.UpdateBox(mSpr.mPos, mSpr.GetScreenSize(), 1.0f, true);
}

// Render function: Handles drawing the player sprite to the screen
void Player::Render(float dTime, DirectX::SpriteBatch& batch)
{
	if (mActive)
	{
		// If our player was struck by a laser and "recovering"
		if (mRecovering)
		{
			// I couldn't think of a better way to do this, so every half a second of the 
			// timer we wont the player to not draw so it gives off a flashing effect
			if (mRecoveryTimer > 0.5f && mRecoveryTimer < 1.0f || 
				mRecoveryTimer > 1.5f && mRecoveryTimer < 2.0f || 
				mRecoveryTimer > 2.5f && mRecoveryTimer < 3.0f || 
				mRecoveryTimer > 3.5f && mRecoveryTimer < 4.0f || 
				mRecoveryTimer > 4.5f && mRecoveryTimer < 5.0f || 
				mRecoveryTimer > 5.5f && mRecoveryTimer < 6.0f)
			{
				mSpr.Draw(batch);

#if defined(DEBUG) || (_DEBUG)
				mBoundingBox.DebugDraw(batch);
#endif
			}

			return;
		}

		// Not recovering so justa draw the player
		mSpr.Draw(batch);

#if defined(DEBUG) || (_DEBUG)
		mBoundingBox.DebugDraw(batch);
#endif
	}
}

// Hit function: Handles the logic of when a player is hit by a laser
void Player::Hit()
{
	// If we've been hit whilst recovering then just skip the function
	if (mRecovering)
		return;

	// Otherwise take away a life and reset our recovery timer
	mLifes--;
	mRecoveryTimer = 0.0f;
	
	// Set our recovering bool to true
	mRecovering = true;

	// If we're dead then make the PlayMode aware of it
	if (!IsAlive())
		mMyMode->GameIsOver();
}

// Constructor for Missile: initializes a missile object
Missile::Missile(MyD3D& d3d)
	:GameObj(d3d)
{
	// Load the frames for the missile's spinning animation
	std::vector<RECTF> frames2(GC::MISSILE_SPIN_FRAMES, GC::MISSILE_SPIN_FRAMES + sizeof(GC::MISSILE_SPIN_FRAMES) / sizeof(GC::MISSILE_SPIN_FRAMES[0]));
	ID3D11ShaderResourceView* p = d3d.GetTexCache().LoadTexture(&d3d.GetDevice(), "sprites/missile.dds", "missile", true, &frames2);

	// Set up the missile sprite
	mSpr.SetTex(*p);
	mSpr.GetAnim().Init(0, 3, 15, true);
	mSpr.GetAnim().Play(true);
	mSpr.SetScale(Vector2(0.5f, 0.5f));
	mSpr.origin = Vector2((GC::MISSILE_SPIN_FRAMES[0].right - GC::MISSILE_SPIN_FRAMES[0].left) / 2.0f, (GC::MISSILE_SPIN_FRAMES[0].bottom - GC::MISSILE_SPIN_FRAMES[0].top) / 2.0f);
	mActive = false;
}

// Update the missile's position and animation each frame
void Missile::Update(float dTime)
{
	// Move the missile upward each frame, speed adjusted by dTime for smoothness
	mSpr.mPos.y -= GC::MISSILE_SPEED * dTime;

	// In preperation for our bounding box update we must get our screen size
	// and then divide by the amount of frames as this is an animated sprite
	Vector2 spriteScreenSize = mSpr.GetScreenSize();
	spriteScreenSize.x = spriteScreenSize.x / mSpr.GetAnim().GetFrameAmount();

	// Update our bounding boxes left,right,top,bottom to match
	// our missile's position with scale factored in
	mBoundingBox.UpdateBox(mSpr.mPos, spriteScreenSize, 0.75f, true);

	// Deactivate missile if it moves off the screen
	if (mSpr.mPos.y < 0)
			mActive = false;

	// Update the missile's spinning animation
	mSpr.GetAnim().Update(dTime);

	// Check with our shelter manager if we're colliding with one of them
	ShelterManager* sM = (ShelterManager*)mMyMode->FindFirst(typeid(ShelterManager), true);  // Retrieve the ShelterManager from our PlayMode

	// Laser hit shelter check
	if (sM != nullptr && sM->AnySheltersLeft())
		sM->CheckMissileCollision(this);

}