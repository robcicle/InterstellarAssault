#include "TutorialMenuMode.h"
#include "Game.h"
#include "WindowUtils.h"
#include "MainMenuMode.h"
#include "D3D.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;


const std::string TutorialMenuMode::MODE_NAME = "TUTORIALMENU";

// TutorialMenuMode Constructor: Initializes the tutorial menu UI elements.
TutorialMenuMode::TutorialMenuMode()
{
	MyD3D& d3d = WinUtil::Get().GetD3D();
	AudioManager& aM = Game::Get().GetAudMgr();

	int w, h;
	WinUtil::Get().GetClientExtents(w, h);

	if (Game::Get().mGamepad.IsConnected())
		mIsKeyboardSelected = false;

	// Load fonts for text display in the tutorial menu.
	SpriteFont* retrotechSF = d3d.GetFontCache().LoadFont(&d3d.GetDevice(), "retrotech.spritefont");
	SpriteFont* lRetrotechSF = d3d.GetFontCache().LoadFont(&d3d.GetDevice(), "retrotech-60.spritefont");

	// Set up the title text for the tutorial menu.
	mTutorialTitleText = new Text(d3d);
	mTutorialTitleText->SetFont(*lRetrotechSF);
	mTutorialTitleText->mActive = true;
	mTutorialTitleText->mString = "HOW TO PLAY";
	mTutorialTitleText->mPos = Vector2((float)w / 2.0f, (float)h * 0.05f);
	mTutorialTitleText->scale = 0.75f;
	mTutorialTitleText->CentreOriginX();

	// Set up the title text for the tutorial menu.
	mKeyboardText = new Text(d3d);
	mKeyboardText->SetFont(*retrotechSF);
	mKeyboardText->mActive = true;
	mKeyboardText->mString = GC::UI_TUTORIAL_KEYBOARD_GUIDE;
	mKeyboardText->mPos = Vector2((float)w / 2.0f, (float)h / 2.0f);
	mKeyboardText->scale = 1.0f;
	mKeyboardText->CentreOriginX();
	mKeyboardText->CentreOriginY();

	// Set up the title text for the tutorial menu.
	mControllerText = new Text(d3d);
	mControllerText->SetFont(*retrotechSF);
	mControllerText->mActive = true;
	mControllerText->mString = GC::UI_TUTORIAL_CONTROLLER_GUIDE;
	mControllerText->mPos = Vector2((float)w / 2.0f, (float)h / 2.0f);
	mControllerText->scale = 1.0f;
	mControllerText->CentreOriginX();
	mControllerText->CentreOriginY();

	// Configure and add the 'Back to Main Menu' button.
	UIButton backButton(d3d, [&]() {
		Game::Get().GetModeMgr().SwitchMode(MainMenuMode::MODE_NAME);
		});
	backButton.mText.SetFont(*retrotechSF);
	backButton.mText.mString = "BACK TO MAIN MENU";
	Vector2 bBScreenSize = backButton.mText.mSFont->MeasureString(backButton.mText.mString.c_str());
	backButton.mText.CentreOriginX();
	backButton.mText.mPos = Vector2(20 + bBScreenSize.x / 2, 20);
	mUIMgr.AddButton(backButton);

	UIButton keyboardButton(d3d, [&]() {
		mIsKeyboardSelected = true;
		});
	keyboardButton.mText.SetFont(*retrotechSF);
	keyboardButton.mText.mString = "KEYBOARD";
	keyboardButton.mText.mPos = Vector2((float)w * 0.4f, (float)h * 0.15f);
	keyboardButton.mText.CentreOriginX();
	mUIMgr.AddButton(keyboardButton);

	UIButton controllerButton(d3d, [&]() {
		mIsKeyboardSelected = false;
		});
	controllerButton.mText.SetFont(*retrotechSF);
	controllerButton.mText.mString = "CONTROLLER";
	controllerButton.mText.mPos = Vector2((float)w * 0.6f, (float)h * 0.15f);
	controllerButton.mText.CentreOriginX();
	mUIMgr.AddButton(controllerButton);
}

// TutorialMenuMode Destructor: Cleans up the text elements created for the tutorial menu.
TutorialMenuMode::~TutorialMenuMode()
{
	// Delete our text pointers
	delete mTutorialTitleText;
	delete mKeyboardText;
	delete mControllerText;
}

// Update function: Handles updates for the tutorial menu, more specifically UI interaction.
void TutorialMenuMode::Update(float dTime)
{
	Game& gm = Game::Get();
	
	mUIMgr.HandleInput();
}

// Render function: Draws the tutorial menu elements.
void TutorialMenuMode::Render(float dTime, DirectX::SpriteBatch& batch)
{
	// Draw our tutorial title text
	mTutorialTitleText->Draw(batch);

	// Depending on if we want keyboard or controller controls
	// render the correct text.
	if (mIsKeyboardSelected)
		mKeyboardText->Draw(batch);
	else
		mControllerText->Draw(batch);

	// Render the UI manager elements.
	mUIMgr.Render(batch);
}

// Reset function: Resets the tutorial menu state, particularly the UI manager.
void TutorialMenuMode::Reset()
{
	mUIMgr.Reset();
}