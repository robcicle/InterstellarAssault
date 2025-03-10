#include "ScoreMenuMode.h"
#include "Game.h"
#include "WindowUtils.h"
#include "MainMenuMode.h"
#include "D3D.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

const std::string ScoreMenuMode::MODE_NAME = "SCOREMENU";


// ScoreMenuMode Constructor: Initializes the score menu with UI elements and score display.
ScoreMenuMode::ScoreMenuMode()
{
	MyD3D& d3d = WinUtil::Get().GetD3D();
	mTexts.reserve(250);
	int w, h;
	WinUtil::Get().GetClientExtents(w, h);

	// Load fonts for displaying scores and UI elements.
	SpriteFont* retrotechSF = d3d.GetFontCache().LoadFont(&d3d.GetDevice(), "retrotech.spritefont");
	SpriteFont* lRetrotechSF = d3d.GetFontCache().LoadFont(&d3d.GetDevice(), "retrotech-60.spritefont");

	ConvertScoresToString(); // Convert scores to a string format for display.
	mScoresSize = Game::Get().GetScoreSys().GetParsedScoresSize();
	
	// Set up text elements for displaying scores and the score menu title.
	mScoresText = new Text(d3d);
	mScoresText->SetFont(*retrotechSF);
	mScoresText->mActive = true;
	mScoresText->mString = mScoreSStream.str();
	mScoresText->mPos = Vector2((float)w / 2.0f, GC::SCROLL_LIST_MAX * GC::MAX_SCORES_SAVE);
	mScoresText->scale = 1.0f;
	mScoresText->colour = Colors::DarkGray;
	mScoresText->CentreOriginX();
	mTexts.push_back(mScoresText);

	mScoreTitleText = new Text(d3d);
	mScoreTitleText->SetFont(*lRetrotechSF);
	mScoreTitleText->mActive = true;
	mScoreTitleText->mString = "SCORES";
	mScoreTitleText->mPos = Vector2((float)w / 2.0f, (float)h * 0.05f);
	mScoreTitleText->scale = 0.75f;
	mScoreTitleText->CentreOriginX();
	mTexts.push_back(mScoreTitleText);

	// Configure the 'Back to Main Menu' button.
	UIButton backButton(d3d, [&]() {
		Game::Get().GetModeMgr().SwitchMode(MainMenuMode::MODE_NAME);
		});
	backButton.mText.SetFont(*retrotechSF);
	backButton.mText.mString = "BACK TO MAIN MENU";
	Vector2 bBScreenSize = backButton.mText.mSFont->MeasureString(backButton.mText.mString.c_str());
	backButton.mText.CentreOriginX();
	backButton.mText.mPos = Vector2(20 + bBScreenSize.x / 2, 20);
	mUIMgr.AddButton(backButton);
}

// ScoreMenuMode Destructor: Cleans up dynamically allocated text resources.
ScoreMenuMode::~ScoreMenuMode()
{
	// Delete all text objects.
	for (size_t i = 0; i < mTexts.size(); ++i)
		delete mTexts[i];
	mTexts.clear();
}

// Update function: Handles user input and updates the score list position.
void ScoreMenuMode::Update(float dTime)
{
	Game& gm = Game::Get();

	float newY = mScoresText->mPos.y;

	// Handle scrolling of the score list based on user input.
	if (gm.mMKIn.IsPressed(VK_UP) || gm.mMKIn.IsPressed(VK_W) || gm.mGamepad.GetButtonPressed(XBtns.DPadUp) || gm.mGamepad.RightStickY() > 0.5f)
	{
		newY += GC::SCROLL_LIST_INC * dTime;
	}
	else if (gm.mMKIn.IsPressed(VK_DOWN) || gm.mMKIn.IsPressed(VK_S) || gm.mGamepad.GetButtonPressed(XBtns.DPadDown) || gm.mGamepad.RightStickY() < -0.5f)
	{
		newY -= GC::SCROLL_LIST_INC * dTime;
	}

	if (newY < GC::SCROLL_LIST_MAX * GC::MAX_SCORES_SAVE && newY > GC::SCROLL_LIST_MIN * GC::MAX_SCORES_SAVE)
		mScoresText->mPos.y = newY;

	mUIMgr.HandleInput();  // Handle UI input interactions.
}

// Render function: Draws the score menu UI elements.
void ScoreMenuMode::Render(float dTime, DirectX::SpriteBatch& batch)
{
	// Draw active text elements.
	for (auto& text : mTexts)
		if (text->mActive)
			text->Draw(batch);

	mUIMgr.Render(batch); // Render UI manager elements.
}

// Reset function: Resets the score menu state and updates the score display.
void ScoreMenuMode::Reset()
{
	mUIMgr.Reset(); // Reset the UI manager state.

	ConvertScoresToString(); // Update the score string representation.
	mScoresSize = Game::Get().GetScoreSys().GetParsedScoresSize();
	mScoresText->mString = mScoreSStream.str(); // Update the scores text.
	mScoresText->CentreOriginX(); // Re-center the scores text.
}

// ConvertScoresToString function: Converts the stored scores to a string for display.
void ScoreMenuMode::ConvertScoresToString()
{
	Game& gm = Game::Get();
	std::vector<ScoreSystem::Score> scores = gm.GetScoreSys().GetParsedScores();
	mScoreSStream.str("");

	// If we have no scores then just exit the code.
	if (scores.size() <= 0)
		return;

	// Format and append each score to the string stream.
	for (int i = 0; i < GC::MAX_SCORES_SAVE; i++)
	{
		if ((size_t)i < gm.GetScoreSys().GetParsedScoresSize())
		{
			mScoreSStream << scores[i].id + 1 << ": Points - " << scores[i].points << ", Name - " << scores[i].name;
			mScoreSStream << "\n";
		}
		else
		{
			mScoreSStream << i + 1 << ": NO SCORE SAVED FOR SLOT";
			mScoreSStream << "\n";
		}
	}
}
