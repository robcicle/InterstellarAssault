#include "MainMenuMode.h"
#include "Game.h"
#include "WindowUtils.h"
#include "PlayMode.h"
#include "D3D.h"
#include "ScoreMenuMode.h"
#include "SettingsMenuMode.h"
#include "TutorialMenuMode.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;


const std::string MainMenuMode::MODE_NAME = "MAINMENU";

MainMenuMode::MainMenuMode()
{
	MyD3D& d3d = WinUtil::Get().GetD3D();

	mTexts.reserve(250);

	int w, h;
	WinUtil::Get().GetClientExtents(w, h);

	// SpriteFont initialisation
	SpriteFont* retrotechSF = d3d.GetFontCache().LoadFont(&d3d.GetDevice(), "retrotech.spritefont");
	SpriteFont* lRetrotechSF = d3d.GetFontCache().LoadFont(&d3d.GetDevice(), "retrotech-60.spritefont");

	// Initialize and set up various text elements for the score entry UI.
	// Including inputs, format warnings, and the actual score and title text.

	mTitleText = new Text(d3d);
	mTitleText->SetFont(*lRetrotechSF);
	mTitleText->mActive = true;
	mTitleText->mString = "Interstellar Assault";
	mTitleText->mPos = Vector2((float)w / 2.0f, (float)h * 0.1f);
	mTitleText->CentreOriginX();
	mTexts.push_back(mTitleText);

	UIButton playButton(d3d, [&]() {
		Game::Get().GetModeMgr().SwitchMode(PlayMode::MODE_NAME);
		});
	playButton.mText.SetFont(*retrotechSF);
	playButton.mText.mString = "PLAY";
	playButton.mText.mPos = Vector2((float)w / 2.0f, (float)h * 0.3f);
	playButton.mText.CentreOriginX();
	mUIMgr.AddButton(playButton);

	UIButton scoresButton(d3d, [&]() {
		Game::Get().GetModeMgr().SwitchMode(ScoreMenuMode::MODE_NAME);
		});
	scoresButton.mText.SetFont(*retrotechSF);
	scoresButton.mText.mString = "VIEW SCORES";
	scoresButton.mText.mPos = Vector2((float)w / 2.0f, (float)h * 0.4f);
	scoresButton.mText.CentreOriginX();
	mUIMgr.AddButton(scoresButton);

	UIButton settingsButton(d3d, [&]() {
		Game::Get().GetModeMgr().SwitchMode(SettingsMenuMode::MODE_NAME);
		});
	settingsButton.mText.SetFont(*retrotechSF);
	settingsButton.mText.mString = "SETTINGS";
	settingsButton.mText.mPos = Vector2((float)w / 2.0f, (float)h * 0.5f);
	settingsButton.mText.CentreOriginX();
	mUIMgr.AddButton(settingsButton);

	UIButton tutorialButton(d3d, [&]() {
		Game::Get().GetModeMgr().SwitchMode(TutorialMenuMode::MODE_NAME);
		});
	tutorialButton.mText.SetFont(*retrotechSF);
	tutorialButton.mText.mString = "HOW TO PLAY";
	tutorialButton.mText.mPos = Vector2((float)w / 2.0f, (float)h * 0.6f);
	tutorialButton.mText.CentreOriginX();
	mUIMgr.AddButton(tutorialButton);

	UIButton quitButton(d3d, [&]() {
		PostQuitMessage(0);
		});
	quitButton.mText.SetFont(*retrotechSF);
	quitButton.mText.mString = "QUIT";
	quitButton.mText.mPos = Vector2((float)w / 2.0f, (float)h * 0.7f);
	quitButton.mText.CentreOriginX();
	mUIMgr.AddButton(quitButton);
}

MainMenuMode::~MainMenuMode()
{
	for (size_t i = 0; i < mTexts.size(); ++i)
		delete mTexts[i];
	mTexts.clear();
}

void MainMenuMode::Update(float dTime)
{
	Game& gm = Game::Get();

	mUIMgr.HandleInput();
}

void MainMenuMode::Render(float dTime, DirectX::SpriteBatch& batch)
{
	for (auto& text : mTexts)
		if (text->mActive)
			text->Draw(batch);
	
	mUIMgr.Render(batch);
}

void MainMenuMode::Reset()
{
	mUIMgr.Reset();
}
