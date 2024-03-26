#include "SettingsMenuMode.h"
#include "Game.h"
#include "WindowUtils.h"
#include "MainMenuMode.h"
#include "D3D.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;


const std::string SettingsMenuMode::MODE_NAME = "SETTINGSMENU";

// SettingsMenuMode Constructor: Initializes the settings menu UI elements.
SettingsMenuMode::SettingsMenuMode()
{
	MyD3D& d3d = WinUtil::Get().GetD3D();
	AudioManager& aM = Game::Get().GetAudMgr();

	mTexts.reserve(250);

	int w, h;
	WinUtil::Get().GetClientExtents(w, h);

	// Load fonts for text display in the settings menu.
	SpriteFont* retrotechSF = d3d.GetFontCache().LoadFont(&d3d.GetDevice(), "retrotech.spritefont");
	SpriteFont* lRetrotechSF = d3d.GetFontCache().LoadFont(&d3d.GetDevice(), "retrotech-60.spritefont");

	// Set up the title text for the settings menu.
	mSettingsTitleText = new Text(d3d);
	mSettingsTitleText->SetFont(*lRetrotechSF);
	mSettingsTitleText->mActive = true;
	mSettingsTitleText->mString = "SETTINGS";
	mSettingsTitleText->mPos = Vector2((float)w / 2.0f, (float)h * 0.05f);
	mSettingsTitleText->scale = 0.75f;
	mSettingsTitleText->CentreOriginX();
	mTexts.push_back(mSettingsTitleText);

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

	// Load and configure the up and down arrow sprites for the counter.
	ID3D11ShaderResourceView* p = d3d.GetTexCache().LoadTexture(&d3d.GetDevice(), "ui/arrows.dds");
	Sprite upArrowSprite(d3d);
	upArrowSprite.SetTex(*p);
	RECTF arrowsRect = upArrowSprite.GetTexRect();
	arrowsRect.right /= 2.0f;
	upArrowSprite.SetTexRect(arrowsRect);
	upArrowSprite.SetScale(Vector2(0.2f, 0.2f));
	upArrowSprite.origin = (upArrowSprite.GetTexData().dim / 2.0f) / 2.0f;
	upArrowSprite.rotation = PI * 10.0f;

	Sprite downArrowSprite(d3d);
	downArrowSprite.SetTex(*p);
	arrowsRect = downArrowSprite.GetTexRect();
	arrowsRect.left = arrowsRect.right / 2.0f;
	downArrowSprite.SetTexRect(arrowsRect);
	downArrowSprite.SetScale(Vector2(0.2f, 0.2f));
	downArrowSprite.origin = (downArrowSprite.GetTexData().dim / 2.0f) / 2.0f;
	downArrowSprite.rotation = PI * 10.0f;

	// Configure and initialize the game volume counter.
	Text counterText(d3d);
	counterText.SetFont(*retrotechSF);
	counterText.mActive = true;
	counterText.scale = 1.0f;
	counterText.CentreOriginX();
	counterText.CentreOriginY();

	// Initialize the game volume counter.
	UICounter gameVolumeCounter(d3d, counterText, upArrowSprite, downArrowSprite, (int)(aM.GetMasterVolume() * 10.0f), 0, 10, [&]() {
		aM.AdjustMasterVolume(mCurrentGameVolume);
		});

	// Set the position of the counter and add it to the UI manager.
	gameVolumeCounter.SetPosition(Vector2((float)w / 2.0f, (float)h / 2.0f)); // Example position
	mUIMgr.AddCounter(gameVolumeCounter);

	// Configure and add the text label for the game volume.
	mGameVolumeText = new Text(d3d);
	mGameVolumeText->SetFont(*retrotechSF);
	mGameVolumeText->mActive = true;
	mGameVolumeText->mString = "GAME VOLUME: ";
	mGameVolumeText->mPos = Vector2(gameVolumeCounter.GetText().mPos.x - mGameVolumeText->GetSize().x, gameVolumeCounter.GetText().mPos.y);
	mGameVolumeText->scale = 1.0f;
	mGameVolumeText->CentreOriginX();
	mTexts.push_back(mGameVolumeText);

}

// SettingsMenuMode Destructor: Cleans up the text elements created for the settings menu.
SettingsMenuMode::~SettingsMenuMode()
{
	for (size_t i = 0; i < mTexts.size(); ++i)
		delete mTexts[i];

	mTexts.clear();
}

// Update function: Handles updates for the settings menu, such as user input and UI interaction.
void SettingsMenuMode::Update(float dTime)
{
	Game& gm = Game::Get();

	// Update the game volume based on the counter value.
	mCurrentGameVolume = mUIMgr.GetCounterValue(0) / 10.0f;
	mUIMgr.HandleInput();
	mCurrentGameVolume = mUIMgr.GetCounterValue(0) / 10.0f;
}

// Render function: Draws the settings menu elements.
void SettingsMenuMode::Render(float dTime, DirectX::SpriteBatch& batch)
{
	for (auto& text : mTexts)
		if (text->mActive)
			text->Draw(batch);

	// Render the UI manager elements.
	mUIMgr.Render(batch);
}

// Reset function: Resets the settings menu state, particularly the UI manager.
void SettingsMenuMode::Reset()
{
	mUIMgr.Reset();
}