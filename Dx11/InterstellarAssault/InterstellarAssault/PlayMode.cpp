#include "PlayMode.h"
#include "Game.h"
#include "WindowUtils.h"
#include "Player.h"
#include "Enemy.h"
#include "GameOverMode.h"
#include "Shelter.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

const std::string PlayMode::MODE_NAME = "PLAY";

// Constructor: Initializes the play mode with background setup and game entities.
PlayMode::PlayMode()
	: mBlackSquareSpr(WinUtil::Get().GetD3D())
{
	InitBgnd();              // Set up parallax background layers.
	mObjects.reserve(1000);  // Reserve space for game objects.
	mTexts.reserve(250);     // Reserve space for text objects.

	MyD3D& d3d = WinUtil::Get().GetD3D();
	int w, h;
	WinUtil::Get().GetClientExtents(w, h);

	// Configure the background black square sprite.
	ID3D11ShaderResourceView* p = d3d.GetTexCache().LoadTexture(&d3d.GetDevice(), "sprites/black_square.dds");
	mBlackSquareSpr.SetTex(*p);
	mBlackSquareSpr.SetScale(Vector2(5.0f, 5.0f));
	mBlackSquareSpr.colour = Vector4(1.0, 1.0, 1.0f, 0.6f);
	mBlackSquareSpr.origin = mBlackSquareSpr.GetTexData().dim / 2.0f;
	mBlackSquareSpr.mPos = Vector2((float)w / 2.0f, (float)h / 2.0f);

	Init();  // Initialize game entities and UI texts.
}

// Destructor: Releases resources of game objects and texts.
PlayMode::~PlayMode()
{
	// Delete all game objects.
	for (auto& obj : mObjects)
		delete obj;
	mObjects.clear();

	// Delete all text objects.
	for (auto& text : mTexts)
		delete text;
	mTexts.clear();
}

// Init function: Sets up game entities like player, enemies, missiles, and UI elements.
void PlayMode::Init()
{
	MyD3D& d3d = WinUtil::Get().GetD3D();

	int w, h;
	WinUtil::Get().GetClientExtents(w, h);

	// Initialize the game objects (Player, Missile, ShelterManager, EnemyManager) 
	// This includes setting their positions, active states, and adding them to their vectors.
	Player* p = new Player(d3d);
	p->SetMode(*this);
	p->mActive = true;
	Add(p);
 
	Missile* m = new Missile(d3d);
	m->SetMode(*this);
	Add(m);

	ShelterManager* sM = new ShelterManager(d3d, *this);
	sM->mActive = true;
	Add(sM);

	EnemyManager* eM = new EnemyManager(d3d);
	eM->SetMode(*this);
	eM->mActive = true;
	Add(eM);
	
	// initialize the UI Text
	SpriteFont* retrotechSF = d3d.GetFontCache().LoadFont(&d3d.GetDevice(), "retrotech.spritefont");
	
	mScoreText = new Text(d3d);
	mScoreText->SetFont(*retrotechSF);
	mScoreText->mPos = Vector2((float)w / 2.0f, (float)(h % 10));
	mScoreText->mActive = true;
	Add(mScoreText);

	mLifesText = new Text(d3d);
	mLifesText->SetFont(*retrotechSF);
	mLifesText->mPos = Vector2((float)(w % 10), (float)(h % 10));
	mLifesText->mActive = true;
	Add(mLifesText);

	SpriteFont* lRetrotechSF = d3d.GetFontCache().LoadFont(&d3d.GetDevice(), "retrotech-60.spritefont");

	mGameOverText = new Text(d3d);
	mGameOverText->SetFont(*lRetrotechSF);
	mGameOverText->mPos = Vector2((float)(w / 2), (float)(h / 2));
	mGameOverText->mString = "GAME OVER";
	mGameOverText->CentreOriginX();
	mGameOverText->CentreOriginY();
	mGameOverText->scale = 2.f;
	Add(mGameOverText);

	mQuitConfirmText = new Text(d3d);
	mQuitConfirmText->SetFont(*lRetrotechSF);
	mQuitConfirmText->mPos = Vector2((float)(w / 2), (float)(h * 0.6));
	mQuitConfirmText->mString = "WOULD YOU LIKE TO EXIT? (Y or N)";
	mQuitConfirmText->CentreOriginX();
	mQuitConfirmText->CentreOriginY();
	mQuitConfirmText->scale = 0.7f;
	Add(mQuitConfirmText);
	
	CallVoidVoidCFunc(Game::Get().GetLuaState(), "setPlayerLifes");
}

// InitBgnd function: Initializes background layers for parallax scrolling effect.
void PlayMode::InitBgnd()
{
	// Fill the array with the right amount of blank sprites
	assert(mBgnd.empty());
	mBgnd.insert(mBgnd.begin(), GC::BGND_LAYERS, Sprite(WinUtil::Get().GetD3D()));

	// Load and set up the background textures and sprites.
	pair<string, string> files[GC::BGND_LAYERS]{
		{ "bgnd0","background_layers/background01_001.dds" },
		{ "bgnd1","background_layers/background01_002.dds" }
	};
	int i = 0;
	for (auto& f : files)
	{
		// Set the texture of the sprite
		ID3D11ShaderResourceView* p = WinUtil::Get().GetD3D().GetTexCache().LoadTexture(&WinUtil::Get().GetD3D().GetDevice(), f.second, f.first);
		if (!p)
			assert(false);
		mBgnd[i++].SetTex(*p);
	}

}

// UpdateBgnd function: Updates background layers for parallax scrolling effect.
void PlayMode::UpdateBgnd(float dTime)
{
	// Iterate through each background layer and scroll them at different speeds for parallax effect.
	int i = 1;
	for (auto& s : mBgnd)
		s.Scroll(0, -(dTime * (i++) * (GC::SCROLL_SPEED * i / 2)));
}

// Update function: Updates the state and logic of the play mode.
void PlayMode::Update(float dTime)
{
	Game& gm = Game::Get();

	// Handle score entry mode separately.
	if (mEnteringScore)
	{
		mScoreData.Update();
		return;
	}
	// Handle quit confirmation logic.
	else if (mWantsToQuit)
	{
		// Update quit confirmation text based on input device.
		if (gm.mGamepad.IsConnected())
			mQuitConfirmText->mString = "WOULD YOU LIKE TO EXIT? (A or B)";
		else
			mQuitConfirmText->mString = "WOULD YOU LIKE TO EXIT? (Y or N)";

		// Handle quit confirmation inputs.
		if (gm.mMKIn.IsUp(VK_Y) || gm.mGamepad.GetButtonUp(XBtns.A))
			gm.GetModeMgr().SwitchMode(GameOverMode::MODE_NAME);
		else if (gm.mMKIn.IsDown(VK_N) || gm.mMKIn.IsDown(VK_ESCAPE) || gm.mGamepad.GetButtonDown(XBtns.B) || gm.mGamepad.GetButtonDown(XBtns.Start))
			mWantsToQuit = false;
		return;
	}

	// Update background and game objects.
	UpdateBgnd(dTime);
	for (auto& obj : mObjects)
		if (obj->mActive)
			obj->Update(dTime);

	// Handle game over transition.
	if (mIsGameOver)
	{
		// Toggle game over text visibility and handle score entry transition.
		mGameOverTimer += dTime;
		if (mGameOverTimer >= 0.5f)
		{
			mTimeSoFar += mGameOverTimer;
			if (mTimeSoFar > GC::GAME_OVER_TRANSITION_TIME)
				mEnteringScore = true;

			mGameOverTimer = 0;
			mGameOverText->mActive = !mGameOverText->mActive;
		}

		// Deactivate life and score texts.
		mLifesText->mActive = false;
		mScoreText->mActive = false;

		return;
	}

	// Update score & life text.
	mScoreText->mString = std::to_string(gm.GetScoreSys().GetCurrentScore());
	mScoreText->CentreOriginX();

	Player* p = (Player*)FindFirst(typeid(Player), true);

	mLifesText->mString = std::string("Lifes: ") + std::to_string(p->GetLifes());

	// Check if player wants to pause to give quit confirmation.
	if (gm.mMKIn.IsDown(VK_ESCAPE) || gm.mGamepad.GetButtonDown(XBtns.B) || gm.mGamepad.GetButtonDown(XBtns.Start))
		mWantsToQuit = true;
}

// Render function: Renders game objects, background, and UI elements.
void PlayMode::Render(float dTime, DirectX::SpriteBatch& batch)
{
	if (mEnteringScore)
	{
		mScoreData.Render(batch); // Render score entry UI if in score entry mode.
		return;
	}

	// Draw background layers for parallax effect.
	for (auto& s : mBgnd)
		s.Draw(batch);

	// Render active game objects.
	for (auto& obj : mObjects)
		if (obj->mActive)
			obj->Render(dTime, batch);

	// Draw text elements.
	for (auto& text : mTexts)
		if (text->mActive)
			text->Draw(batch);

	// Render quit confirmation overlay if needed.
	if (mWantsToQuit)
	{
		mBlackSquareSpr.Draw(batch); // Draw semi-transparent background for readability.
		mQuitConfirmText->Draw(batch); // Draw quit confirmation text.
	}
}

// ProcessKey function: Handles keyboard input during gameplay.
void PlayMode::ProcessKey(char key)
{
	if (mEnteringScore)
		mScoreData.ProcessKey(key); // Delegate to score entry logic if in score entry mode.
}

// Reset function: Resets the play mode to its initial state.
void PlayMode::Reset()
{
	// Clear all objects and texts
	for (auto& obj : mObjects)
		delete obj;
	mObjects.clear();

	for (auto& text : mTexts)
		delete text;
	mTexts.clear();

	// Reset any flags/timers we're using
	mIsGameOver = false;
	mGameOverTimer = 0.0f;
	mTimeSoFar = 0.0f;
	mEnteringScore = false;
	mWantsToQuit = false;

	// Reset our score
	Game::Get().GetScoreSys().ClearCurrentScore();

	// Reinitialize the Game
	Init();
}

// Add function: Adds a new game object to the play mode.
void PlayMode::Add(GameObj* pObj) {
	assert(pObj);
	mObjects.push_back(pObj);
}

// Add function: Adds a new text object to the play mode.
void PlayMode::Add(Text* pTxt)
{
	assert(pTxt);
	mTexts.push_back(pTxt);
}

// Remove function: Removes and deletes a game object from the play mode.
void PlayMode::Remove(GameObj* pObj) {
	size_t sz = mObjects.size();
	assert(sz > 0);
	mObjects.erase(std::remove(mObjects.begin(), mObjects.end(), pObj));
	assert(sz != mObjects.size());
	delete pObj;
}

// FindFirst function: Finds the first game object of a specified type.
GameObj* PlayMode::FindFirst(const std::type_info& type, bool active) {
	size_t i = 0;
	// Iterate through the objects vector to find the first matching game object.
	while (i < mObjects.size() && (typeid(*mObjects[i]) != type || mObjects[i]->mActive != active))
		++i;
	if (i >= mObjects.size())
		return nullptr;
	return mObjects[i];
}

// GameIsOver function: Handles the transition to the game over state.
void PlayMode::GameIsOver()
{
	if (mIsGameOver)
		return; // Prevent multiple triggers of game over logic.

	mIsGameOver = true; // Set the game over flag.

	// Deactivate any missiles in the game.
	GameObj* m = FindFirst(typeid(Missile), true);
	if (m != nullptr)
		m->mActive = false;

	// Prepare for score entry.
	mScoreData.InputScore(Game::Get().GetScoreSys().GetCurrentScore());

	// Handle player's lives and transition to score entry if needed.
	Player* p = (Player*)FindFirst(typeid(Player), true);
	int playerLifes = p->GetLifes();
	if (playerLifes > 0)
		for (int i = 0; i < playerLifes; i++)
			p->Hit();
}

// EnterScoreData Constructor: Initializes UI elements for score entry.
PlayMode::EnterScoreData::EnterScoreData()
{
	mTexts.reserve(250); // Reserve space for text objects.
	MyD3D& d3d = WinUtil::Get().GetD3D();
	int w, h;
	WinUtil::Get().GetClientExtents(w, h);

	// Load fonts and set up text elements for score entry.
	SpriteFont* lRetrotechSF = d3d.GetFontCache().LoadFont(&d3d.GetDevice(), "retrotech-60.spritefont");

	// Set up the prompt text for name entry.
	mEnterNameText = new Text(d3d);
	mEnterNameText->SetFont(*lRetrotechSF);
	mEnterNameText->mPos = Vector2((float)(w / 2), (float)h * 0.1f);
	mEnterNameText->mString = "ENTER YOUR NAME";
	mEnterNameText->CentreOriginX();
	mEnterNameText->mActive = true;
	mTexts.push_back(mEnterNameText);

	mNameText = new Text(d3d);
	mNameText->SetFont(*lRetrotechSF);
	mNameText->mPos = Vector2((float)(w / 2), (float)(h * 0.4f));
	mNameText->scale = 0.75f;
	mNameText->mActive = true;
	mTexts.push_back(mNameText);

	mFormatWarnText = new Text(d3d);
	mFormatWarnText->SetFont(*lRetrotechSF);
	mFormatWarnText->mPos = Vector2((float)(w / 2), (float)(h * 0.5f));
	mFormatWarnText->scale = 0.5f;
	mFormatWarnText->colour = Colors::Red;
	mTexts.push_back(mFormatWarnText);

	mScoreText = new Text(d3d);
	mScoreText->SetFont(*lRetrotechSF);
	mScoreText->mPos = Vector2((float)(w / 2), (float)(h * 0.25f));
	mScoreText->scale = 0.5f;
	mScoreText->mString = "YOUR SCORE: ";
	mScoreText->mActive = true;
	mTexts.push_back(mScoreText);

	isFinished = false;
}

// EnterScoreData Destructor: Cleans up dynamically allocated text objects.
PlayMode::EnterScoreData::~EnterScoreData()
{
	// Iterate and delete each dynamically allocated text object.
	for (auto& text : mTexts)
		delete text;
	mTexts.clear();
}

// Update function: Updates the score entry process.
void PlayMode::EnterScoreData::Update()
{
	Game& gm = Game::Get();

	// Check for submission of the entered name.
	if (gm.mMKIn.IsUp(VK_RETURN) || gm.mGamepad.GetButtonUp(XBtns.A))
	{
		// Validate the entered name.
		if (mNameString.empty())
		{
			mFormatWarnText->mString = "CANNOT INPUT NOTHING";
			mFormatWarnText->CentreOriginX();
			mFormatWarnText->mActive = true;
		}
		else
		{
			Finish(); // Finalize the score entry.
		}
	}
	// Additional update logic can be implemented here.
}

// Render function: Draws UI elements related to score entry.
void PlayMode::EnterScoreData::Render(SpriteBatch& batch)
{
	for (auto& text : mTexts)
		if (text->mActive)
			text->Draw(batch);
}

// ProcessKey function: Handles keyboard input for player name entry.
void PlayMode::EnterScoreData::ProcessKey(char key)
{
	// Skip processing if score entry is finished.
	if (isFinished)
		return;

	mFormatWarnText->mActive = false; // Hide format warning text.

	// Handle backspace key.
	if (key == VK_BACK)
	{
		if (!mNameString.empty())
		{
			mNameString.pop_back(); // Remove last character.
			mNameText->mString = mNameString;
			mNameText->CentreOriginX();
		}
		return;
	}

	// Handle other key inputs.
	switch (key)
	{
	case VK_SPACE:
		mFormatWarnText->mString = "NO SPACES ALLOWED";
		mFormatWarnText->CentreOriginX();
		mFormatWarnText->mActive = true;
		return;
	case VK_RETURN:
		return;
	}

	// Validation for character limits and allowed characters.

	// Check it doesn't exceed the 8 character limit
	if (mNameString.size() >= 8)
	{
		mFormatWarnText->mString = "CANNOT EXCEED 8 CHARACTERS";
		mFormatWarnText->CentreOriginX();
		mFormatWarnText->colour = Colors::Red;
		mFormatWarnText->mActive = true;
		return;
	}

	// Check that this character is alphabetical
	std::string newNameString = mNameString + key;

	for (size_t i = 0; i < newNameString.size(); i++)
	{
		// make sure each character is A-Z or a space
		if (!std::isalpha(newNameString[i]) && !std::isspace(newNameString[i]))
		{
			mFormatWarnText->mString = "ALPHABETICAL CHARACTERS ONLY";
			mFormatWarnText->CentreOriginX();
			mFormatWarnText->colour = Colors::Red;
			mFormatWarnText->mActive = true;
			return; // This one didn't match so return
		}
	}

	// If all else passes then it continues
	mNameString = newNameString;
	std::transform(mNameString.begin(), mNameString.end(), mNameString.begin(), ::toupper);
	mNameText->mString = mNameString;
	mNameText->CentreOriginX();

	if (mNameString.size() > 0)
	{
		mFormatWarnText->mString = "PRESS ENTER OR A (CONTROLLER) TO CONTINUE";
		mFormatWarnText->CentreOriginX();
		mFormatWarnText->colour = Colors::White;
		mFormatWarnText->mActive = true;
	}
}

// Finish function: Finalizes the score entry process.
void PlayMode::EnterScoreData::Finish()
{
	isFinished = true; // Mark the entry as finished.

	Game& gm = Game::Get();
	gm.GetScoreSys().AddCurrentName(mNameString); // Add name to score system.
	mNameString.clear();
	mNameText->mString = mNameString;
	mScoreText->mString = "YOUR SCORE: ";
	mFormatWarnText->mActive = false;

	gm.GetScoreSys().ClearCurrentScore(); // Clear the current score.
	gm.GetScoreSys().SaveScores();        // Save the scores to file.
	gm.GetModeMgr().SwitchMode(GameOverMode::MODE_NAME); // Switch to game over mode.
}