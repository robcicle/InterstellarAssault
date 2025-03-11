#include "Enemy.h"
#include "Player.h"
#include "Game.h"
#include "Shelter.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;


// Constructor for Laser: Initializes a laser object
Laser::Laser(MyD3D& d3d)
	:GameObj(d3d), mScreenHeight(WinUtil::Get().GetClientHeight())
{
	// Load the texture for the laser
	ID3D11ShaderResourceView* p = d3d.GetTexCache().LoadTexture(&d3d.GetDevice(), "sprites/laser.dds");

	// Set up the laser sprite
	mSpr.SetTex(*p);
	mSpr.origin = mSpr.GetTexData().dim / 2.0f;
	mSpr.rotation = PI * 10.0f;
	mActive = false;
}

// Update the laser's position and animation each frame
void Laser::Update(float dTime)
{
	// Move the laser upward each frame, speed adjusted by dTime for smoothness
	mSpr.mPos.y += GC::LASER_SPEED * dTime;

	// Update our bounding boxes left,right,top,bottom to match
	// our laser's position with scale factored in
	mBoundingBox.UpdateBox(mSpr.mPos, mSpr.GetScreenSize(), 0.75f, true);

	// Deactivate laser if it moves off the screen
	if (mSpr.mPos.y > (mScreenHeight + mSpr.GetScreenSize().y))
		mActive = false;
}
// Constructor for Enemy: initializes an enemy based on its type
Enemy::Enemy(EnemyType _eTypeToBe)
	: GameObj(WinUtil::Get().GetD3D())
{
	MyD3D& d3d = WinUtil::Get().GetD3D();
	mEType = _eTypeToBe;

	// Load and orient the sprite based on enemy type
	ID3D11ShaderResourceView* p = nullptr;
	switch (mEType)
	{
	case EnemyType::OCTOPUS:
		p = d3d.GetTexCache().LoadTexture(&d3d.GetDevice(), "sprites/octopus.dds");
		mSpr.SetScale(Vector2(0.1f, 0.1f));
		break;
	case EnemyType::CRAB:
		p = d3d.GetTexCache().LoadTexture(&d3d.GetDevice(), "sprites/crab.dds");
		mSpr.SetScale(Vector2(0.09f, 0.09f));
		break;
	case EnemyType::SQUID:
		p = d3d.GetTexCache().LoadTexture(&d3d.GetDevice(), "sprites/squid.dds");
		mSpr.SetScale(Vector2(0.08f, 0.08f));
		mLaser = new Laser(d3d);
		break;
	case EnemyType::UFO:
		p = d3d.GetTexCache().LoadTexture(&d3d.GetDevice(), "sprites/ufo.dds");
		mSpr.SetScale(Vector2(0.15f, 0.1f));
		break;
	default:
		p = d3d.GetTexCache().LoadTexture(&d3d.GetDevice(), "sprites/octopus.dds");  // By default make it the octopus sprite
		break;
	}

	// Make sure that it's now pointing to it's texture
	if (p == nullptr)
		assert(p = nullptr);

	mSpr.SetTex(*p);
	mSpr.origin = mSpr.GetTexData().dim / 2.0f;
	mSpr.rotation = PI * 10.0f;
}

// Destructor for Enemy: Handles clean-up when an enemy is destroyed
Enemy::~Enemy()
{
	if (mLaser != nullptr)
	{
		delete mLaser;
		mLaser = nullptr;
	}
}

// Update function for Enemy: Manages the enemy's state each frame
void Enemy::Update(float dTime)
{
	// Update our bounding boxes left,right,top,bottom to match
	// our enemy's position with scale factored in and and a
	// uniform scale applied on top
	mBoundingBox.UpdateBox(mSpr.mPos, mSpr.GetScreenSize(), 0.9f, true);

	if (mLaser == nullptr)
		return;

	mShootTimer += dTime;

	if (mShootTimer <= (float)GC::ENEMY_TIME_BTWN_SHOTS)
		return;

	// Generate a random number between 0 and 100
	int random = (int)floor(LuaHelper::LuaFRandomNum(Game::Get().GetLuaState(), "randomNumber", 0, 100));

	if (random >= GC::ENEMY_SHOOT_CHANCE)
	{
		mShootTimer = 0;
		return;
	}

	if (!mLaser->mActive)
	{
		mLaser->mActive = true;
		// Set the laser's position slighty below the centre of the enemy
		mLaser->mSpr.mPos = Vector2(mSpr.mPos.x, mSpr.mPos.y + mSpr.GetScreenSize().y / 2);
		mLaser->mSpr.rotation = PI * 0.0f;
		// Play the shooting sound effect
		Game::Get().GetAudMgr().CreateSFXInstance(AudioManager::SoundList::LAZER_SHOOT);
	}

	mShootTimer = 0;
}

// Constructor for EnemyManager: Initializes the manager and sets up enemies
EnemyManager::EnemyManager(MyD3D& d3d)
	: GameObj(d3d)
{
	// Setup the enemies array and fill it with the correct enemies

	mSpeed = 10.0f; // Set the movement speed for enemies

	// Let's quickly create an octopus for data,
	// reason for this is because it's the biggest
	// and we want all of our enemies to be spaced equally
	Enemy* octopusData = new Enemy(Enemy::EnemyType::OCTOPUS);

	// Calculate positions and dimensions for enemy arrangement
	int screenWidth = WinUtil::Get().GetClientWidth();
	//float enemyWidth = (octopusData->mSpr.GetTexData().dim.x * octopusData->mSpr.GetScale().x) + (float)GC::ROWX_SPACING; // Width of an enemy sprite + our horizontal spacing
	float enemyWidth = (octopusData->mSpr.GetTexData().dim.x * octopusData->mSpr.GetScale().x) + (float)LuaHelper::LuaGetInt(Game::Get().GetLuaState(), "rowXSpacing", GC::ROWX_SPACING); // Width of an enemy sprite + our horizontal spacing
	//float totalRowWidth = (float)GC::ENEMIES_PER_ROW * enemyWidth;
	float totalRowWidth = (float)LuaHelper::LuaGetInt(Game::Get().GetLuaState(), "enemiesPerRow", GC::ENEMIES_PER_ROW) * enemyWidth;
	float initialX = (screenWidth - totalRowWidth) / 2; // Center the row of enemies

	// Now we're done using the octopusData so lets delete it
	delete octopusData;

	Enemy* e;

	// Create and position enemies based on rows and columns

	// Grab our variables from the Lua script.
	int numOfRows = LuaHelper::LuaGetInt(Game::Get().GetLuaState(), "numOfRows", GC::NUM_ROWS);
	int enemiesPerRow = LuaHelper::LuaGetInt(Game::Get().GetLuaState(), "enemiesPerRow", GC::ENEMIES_PER_ROW);
	int rowYSpacing = LuaHelper::LuaGetInt(Game::Get().GetLuaState(), "rowYSpacing", GC::ROWY_SPACING);
	int enemyInitialY = LuaHelper::LuaGetInt(Game::Get().GetLuaState(), "enemyInitialY", GC::ENEMY_INITIAL_Y);

	//for (int row = 0; row < GC::NUM_ROWS; row++)
	for (int row = 0; row < numOfRows; row++)
		//for (int col = 0; col < GC::ENEMIES_PER_ROW; col++)
		for (int col = 0; col < enemiesPerRow; col++)
		{
			// Determine the type of enemy based on row
			if (row > 2)
				e = new Enemy(Enemy::EnemyType::OCTOPUS);
			else if (row > 0)
				e = new Enemy(Enemy::EnemyType::CRAB);
			else if (row <= 0)
				e = new Enemy(Enemy::EnemyType::SQUID);

			// Set the initial position with some spacing
			//e->SetPosition(Vector2(initialX + ((float)(col + 0.5) * enemyWidth), (float)GC::ENEMY_INITIAL_Y + (row * GC::ROWY_SPACING)));
			e->SetPosition(Vector2(initialX + ((float)(col + 0.5) * enemyWidth), (float)enemyInitialY + (row * rowYSpacing)));
			mEnemies[mActiveEnemies] = e; // Add to the vector of enemies
			e->mActive = true; // Activate the enemy

			mActiveEnemies++;
		}

	// Finally create the ufo enemy
	mUfoEnemy = new Enemy(Enemy::EnemyType::UFO);
	//mUfoEnemy->SetPosition(Vector2(0, (float)GC::UFO_INITIAL_Y));
	mUfoEnemy->SetPosition(Vector2(0, (float)LuaHelper::LuaGetInt(Game::Get().GetLuaState(), "ufoInitialY", GC::UFO_INITIAL_Y)));
	mUfoDirection = -1;
	mUfoEnemy->mActive = false;

	//mLeftLimit = (float)GC::ENEMY_LIMIT_OFFSET; // Set the left movement limit
	mLeftLimit = (float)LuaHelper::LuaGetInt(Game::Get().GetLuaState(), "enemyLimitOffset", GC::ENEMY_LIMIT_OFFSET); // Set the left movement limit
	//mRightLimit = screenWidth - (float)GC::ENEMY_LIMIT_OFFSET; // Set the right movement limit
	mRightLimit = screenWidth - (float)LuaHelper::LuaGetInt(Game::Get().GetLuaState(), "enemyLimitOffset", GC::ENEMY_LIMIT_OFFSET); // Set the right movement limit

	mDirection = 1; // Start moving to the left
}

// Destructor for EnemyManager: Handles clean-up of all enemies
EnemyManager::~EnemyManager()
{
	for (Enemy* e : mEnemies)
	{
		delete e;  // Delete each enemy in the array
		e = nullptr;
	}

	mActiveEnemies = 0;  // reset the active enemies counter
}

// Init function for EnemyManager: Resets the initial state and positions of enemies after a wave ends
void EnemyManager::Init()
{
	Game::Get().GetScoreSys().AddCurrentPoints(GC::WAVE_FINISH_POINTS); // Reward points for wave completion
	Player* p = (Player*)mMyMode->FindFirst(typeid(Player), true);      // Retrieve the player from our PlayMode
	p->AddLifes(1);  // Give them an extra life for completing a wave

	mSpeed = 10.0f; // Set the movement speed for enemies

	// Let's quickly create an octopus for data,
	// reason for this is because it's the biggest
	// and we want all of our enemies to be spaced equally
	Enemy* octopusData = new Enemy(Enemy::EnemyType::OCTOPUS);

	// Calculate positions and dimensions for enemy arrangement
	int screenWidth = WinUtil::Get().GetClientWidth();
	//float enemyWidth = (octopusData->mSpr.GetTexData().dim.x * octopusData->mSpr.GetScale().x) + (float)GC::ROWX_SPACING; // Width of an enemy sprite + our horizontal spacing
	float enemyWidth = (octopusData->mSpr.GetTexData().dim.x * octopusData->mSpr.GetScale().x) + (float)LuaHelper::LuaGetInt(Game::Get().GetLuaState(), "rowXSpacing", GC::ROWX_SPACING); // Width of an enemy sprite + our horizontal spacing	//float totalRowWidth = (float)GC::ENEMIES_PER_ROW * enemyWidth;
	float totalRowWidth = (float)LuaHelper::LuaGetInt(Game::Get().GetLuaState(), "enemiesPerRow", GC::ENEMIES_PER_ROW) * enemyWidth;
	float initialX = (screenWidth - totalRowWidth) / 2; // Center the row of enemies

	// Now we're done using the octopusData so lets delete it
	delete octopusData;

	// Create and position enemies based on rows and columns
	int numOfRows = LuaHelper::LuaGetInt(Game::Get().GetLuaState(), "numOfRows", GC::NUM_ROWS);
	int enemiesPerRow = LuaHelper::LuaGetInt(Game::Get().GetLuaState(), "enemiesPerRow", GC::ENEMIES_PER_ROW);
	int rowYSpacing = LuaHelper::LuaGetInt(Game::Get().GetLuaState(), "rowYSpacing", GC::ROWY_SPACING);
	int enemyInitialY = LuaHelper::LuaGetInt(Game::Get().GetLuaState(), "enemyInitialY", GC::ENEMY_INITIAL_Y);

	//for (int row = 0; row < GC::NUM_ROWS; row++)
	for (int row = 0; row < numOfRows; row++)
	{
		//for (int col = 0; col < GC::ENEMIES_PER_ROW; col++)
		for (int col = 0; col < enemiesPerRow; col++)
		{
			// Reset to the initial position with some spacing
			//mEnemies[mActiveEnemies]->SetPosition(Vector2(initialX + ((float)(col + 0.5) * enemyWidth), (float)GC::ENEMY_INITIAL_Y + (row * GC::ROWY_SPACING)));
			mEnemies[mActiveEnemies]->SetPosition(Vector2(initialX + ((float)(col + 0.5) * enemyWidth), (float)enemyInitialY + (row * rowYSpacing)));
			mEnemies[mActiveEnemies]->mActive = true; // Make the the enemy active
			mActiveEnemies++;
		}
	}

	mDirection = 1; // Start moving to the left
}

// Update function for EnemyManager: Manages state and movement of all enemies each frame
void EnemyManager::Update(float dTime)
{
	if (mMyMode->IsGameOver())
		return;

	float bottomRowY = 0; // Track the lowest enemy position

	if (mActiveEnemies <= 0)
	{
		Init();  // Re-initialize if no enemies are left
		return;
	}

	// Update and move each active enemy
	for (size_t i = 0; i < GC::NUM_ENEMIES; i++)
	{
		if (mEnemies[i]->mLaser != nullptr)
		{
			if (mEnemies[i]->mLaser->mActive)
				mEnemies[i]->mLaser->Update(dTime);
		}

		if (mEnemies[i]->mActive)
		{
			Vector2 pos = mEnemies[i]->mSpr.mPos;

			// Move horizontally based on direction and speed
			pos.x += mDirection * (mSpeed * dTime);

			// Update the enemy's position
			mEnemies[i]->mSpr.mPos = pos;
			mEnemies[i]->Update(dTime);

			// Check for collision with screen edges and adjust accordingly
			if ((mDirection == 1 && mEnemies[i]->mSpr.mPos.x > mRightLimit) ||
				(mDirection == -1 && mEnemies[i]->mSpr.mPos.x < mLeftLimit))
			{
				float enemyDownstep = (float)LuaHelper::LuaGetInt(Game::Get().GetLuaState(), "enemyDownstep", GC::ENEMY_DOWNSTEP);

				// Move all active enemies down and reverse direction
				for (Enemy* e : mEnemies)
				{
					if (e->mActive)
						//e->mSpr.mPos.y += GC::ENEMY_DOWNSTEP;
						e->mSpr.mPos.y += enemyDownstep;
				}

				// Because we're moving down, lets get a random number and 
				// see if it's with our chance of getting the random ufo
				int random = (int)floor(LuaHelper::LuaFRandomNum(Game::Get().GetLuaState(), "randomNumber", 0, 100));

				if (!mUfoEnemy->mActive && random <= GC::ENEMY_UFO_CHANCE || mUfoActive)
				{
					mUfoEnemy->mActive = true;
					mUfoActive = true;
				}

				mDirection *= -1; // Change direction
				break; // Only one check per frame needed as all move together
			}

			// Track the lowest enemy position
			if (mEnemies[i]->mSpr.mPos.y > bottomRowY)
			{
				bottomRowY = mEnemies[i]->mSpr.mPos.y;
			}
		}
	}

	if (mUfoEnemy->mActive)
	{
		Vector2 pos = mUfoEnemy->mSpr.mPos;

		// Move horizontally based on direction and speed
		pos.x += mUfoDirection * (GC::ENEMY_UFO_SPEED * dTime);

		if (pos.x < 0 || pos.x > WinUtil::Get().GetClientWidth() + (mUfoEnemy->mSpr.GetScreenSize().y / 2))
		{
			mUfoEnemy->mActive = false;
			mUfoDirection *= -1;
			return;
		}

		// Update the enemy's position
		mUfoEnemy->mSpr.mPos = pos;
		mUfoEnemy->Update(dTime);
	}

	// Check if the lowest enemy has reached a critical position indicating game over
	if (bottomRowY >= 600) // 640 should be based on some game-defined boundary
	{
		mMyMode->GameIsOver(); // Tell the game that it is now over
		return;
	}

	if (mUfoEnemy->mActive)
		mUfoEnemy->Update(dTime);

	CheckCollisions(); // Check for collisions between enemies and other objects
}

// Render function for EnemyManager: Draws active enemies each frame
void EnemyManager::Render(float dTime, DirectX::SpriteBatch& batch)
{
	// We want our lasers to render last so they appear on top,
	// so lets hold them in an array tempararily and render them last
	Laser* lasers[GC::ENEMIES_PER_ROW] = { 0 };
	int currCount = 0;

	for (Enemy* e : mEnemies)
	{
		if (e->mActive)
			e->Render(dTime, batch);  // Render each active enemy
		if (e->mLaser != nullptr)
			if (e->mLaser->mActive)
			{
				lasers[currCount] = e->mLaser;
				currCount++;
			}
	}

	// Render the ufo enemy
	if (mUfoEnemy->mActive)
		mUfoEnemy->Render(dTime, batch);

	// Now using our current count, loop through the lasers and render
	for (int i = 0; i < currCount; i++)
	{
		lasers[i]->Render(dTime, batch);  // Render the lasers
	}

	// This was the only efficient and non-performance taxing solution
	// to rendering the lasers on top of the enemies that I could find
}

// CheckCollisions function: Handles collision detection and response for enemies
void EnemyManager::CheckCollisions()
{
	GameObj* m = mMyMode->FindFirst(typeid(Missile), true); // Find the first missile
	Player* p = (Player*)mMyMode->FindFirst(typeid(Player), true);  // Retrieve the player from our PlayMode
	ShelterManager* sM = (ShelterManager*)mMyMode->FindFirst(typeid(ShelterManager), true);  // Retrieve the ShelterManager from our PlayMode

	bool missileExploded = false;

	// Check for collisions through our enemies
	for (Enemy* e : mEnemies)
	{
		// Laser hit shelter check
		if (e->mLaser != nullptr && sM != nullptr &&
			e->mLaser->mActive && sM->AnySheltersLeft())
			sM->CheckLaserCollision(e->mLaser);

		// Laser hit player check
		if (e->mLaser != nullptr && p != nullptr &&
			e->mLaser->mActive && p->IsAlive())
		{
			// Check if one object is to the left of the other
			if (p->mBoundingBox.mRight < e->mLaser->mBoundingBox.mLeft || e->mLaser->mBoundingBox.mRight < p->mBoundingBox.mLeft)
				continue; // No collision on the X axis

			// Check if one object is above the other
			if (p->mBoundingBox.mTop > e->mLaser->mBoundingBox.mBottom || e->mLaser->mBoundingBox.mTop > p->mBoundingBox.mBottom)
				continue; // No collision on the Y axis

			// Anything after this means an enemy and a missile have collided
			LaserHit(p, e->mLaser);
		}

		// Missile hit enemy check
		if (m != nullptr && e->mActive && !missileExploded)
		{
			// Check if one object is to the left of the other
			if (m->mBoundingBox.mRight < e->mBoundingBox.mLeft || e->mBoundingBox.mRight < m->mBoundingBox.mLeft)
				continue; // No collision on the X axis

			// Check if one object is above the other
			if (m->mBoundingBox.mTop > e->mBoundingBox.mBottom || e->mBoundingBox.mTop > m->mBoundingBox.mBottom)
				continue; // No collision on the Y axis

			// Anything after this means an enemy and a missile have collided
			MissileHit(e, m);

			missileExploded = true;
		}
	}

	// Missile hit enemy check
	if (m != nullptr && mUfoEnemy->mActive && !missileExploded)
	{
		// Check if one object is to the left of the other
		if (m->mBoundingBox.mRight < mUfoEnemy->mBoundingBox.mLeft || mUfoEnemy->mBoundingBox.mRight < m->mBoundingBox.mLeft)
			return; // No collision on the X axis

		// Check if one object is above the other
		if (m->mBoundingBox.mTop > mUfoEnemy->mBoundingBox.mBottom || mUfoEnemy->mBoundingBox.mTop > m->mBoundingBox.mBottom)
			return; // No collision on the Y axis

		// Anything after this means an enemy and a missile have collided
		MissileHit(mUfoEnemy, m);
		mUfoEnemy->SetPosition(Vector2(0, mUfoEnemy->mSpr.mPos.y));
		mUfoDirection = -1;
		mUfoActive = false;

		missileExploded = true;
	}
}

// Missilehit function: Handles the logic behind an enemy being hit by a missile
void EnemyManager::MissileHit(Enemy* _enemy, GameObj* _missile)
{
	Game& gm = Game::Get();

	_missile->mActive = false; // Deactivate the missile
	gm.GetAudMgr().CreateSFXInstance(AudioManager::SoundList::EXPLOSION); // Trigger explosion sound

	// Enemy now needs to be gone
	_enemy->mActive = false; // Deactivate the enemy

	if (_enemy->GetEnemyType() != Enemy::EnemyType::UFO)
		mActiveEnemies--;    // Decrease our enemy count by one

	gm.GetAudMgr().CreateSFXInstance(AudioManager::SoundList::HIT); // Trigger hit sound

	if (gm.mGamepad.IsConnected())
		gm.mGamepad.VibrateOnTimer(0.2f, 0.02f, 0.02f); // Provide feedback on destruction

	// Gameplay logic, score/speed
	gm.GetScoreSys().AddCurrentPoints(GetETypePoints(_enemy->GetEnemyType())); // Add points for hitting the enemy
	mSpeed += GC::ENEMY_SPEED_INC; // Increase the speed for remaining enemies
}

// LaserHit function: Handles the logic behind a laser hitting the player
void EnemyManager::LaserHit(Player* _player, Laser* _laser)
{
	Game& gm = Game::Get();

	_laser->mActive = false;  // Deactivate the laser
	_laser->mBoundingBox.UpdateBox(0.0f, 0.0f, 0.0f, 0.0f);

	// Player now needs go into recovery state
	_player->Hit();  // Tell the player it has been hit
	gm.GetAudMgr().CreateSFXInstance(AudioManager::SoundList::HIT); // Trigger hit sound

	if (gm.mGamepad.IsConnected())
		gm.mGamepad.VibrateOnTimer(0.2f, 0.1f, 0.1f);  // Provide feedback on hit
}

// GetETypePoints function: Determines the points value for an enemy based on its type
int EnemyManager::GetETypePoints(Enemy::EnemyType _eType) const
{
	switch (_eType)
	{
	case Enemy::EnemyType::OCTOPUS:
		return GC::OCTOPUS_POINTS; // Points for an octopus type enemy
	case Enemy::EnemyType::CRAB:
		return GC::CRAB_POINTS; // Points for a crab type enemy
	case Enemy::EnemyType::SQUID:
		return GC::SQUID_POINTS; // Points for a squid type enemy
	case Enemy::EnemyType::UFO:
		// Assign random points for a UFO type enemy
		srand((unsigned int)time(0));
		return(GC::UFO_POINTS[rand() % 4]);
	default:
		return 0; // Default case to catch any undefined enemy types
	}
}