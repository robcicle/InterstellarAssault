#pragma once

#include <vector>

#include "D3D.h"
#include "SpriteBatch.h"
#include "Sprite.h"
#include "Singleton.h"
#include "ModeMgr.h"
#include "GameObj.h"
#include "Text.h"


// PlayMode class: Manages the gameplay mechanics for a space invaders styled game.
class PlayMode : public AMode
{
public:
    static const std::string MODE_NAME; // Static constant for the mode name.

    // Constructor: Initializes the gameplay mode with necessary setups.
    PlayMode();

    // Destructor: Cleans up dynamically allocated resources in the gameplay mode.
    ~PlayMode();

    // Init function: Sets up initial state and objects for the gameplay mode.
    void Init();

    // Update function: Handles update logic of game objects and gameplay state.
    void Update(float dTime) override;

    // Render function: Draws game objects, background elements, and UI texts.
    void Render(float dTime, DirectX::SpriteBatch& batch) override;

    // GetMName function: Returns the name of the mode.
    std::string GetMName() const override { return "PLAY"; }

    // ProcessKey function: Processes key inputs during gameplay.
    void ProcessKey(char key) override;

    // Reset function: Resets the game mode to its initial state.
    void Reset() override;

    // Add function: Adds a GameObj to the gameplay mode.
    void Add(GameObj* pObj);

    // Add function: Adds a Text object to the gameplay mode.
    void Add(Text* pTxt);

    // Remove function: Removes and deletes a specified GameObj from the game mode.
    void Remove(GameObj* pObj);

    // FindFirst function: Finds the first matching GameObj based on type and active state.
    GameObj* FindFirst(const std::type_info& type, bool active);

    // GameIsOver function: Marks the game as over and handles related logic.
    void GameIsOver();

    // IsGameOver function: Checks if the game is over.
    bool IsGameOver() const { return mIsGameOver; }

private:
    std::vector<Sprite> mBgnd;       // Background sprites for parallax effect.
    std::vector<GameObj*> mObjects;  // Game objects for update and render.
    std::vector<Text*> mTexts;       // Text objects for rendering.

    Text* mScoreText = nullptr;  // Text object for displaying score.
    Text* mLifesText = nullptr;  // Text object for displaying player lives.

    Text* mGameOverText = nullptr;     // Text object for displaying game over message.
    Text* mQuitConfirmText = nullptr;  // Text object for displaying "Would you like to exit? (Y/N)"
    Sprite mBlackSquareSpr;

    bool mIsGameOver = false;     // Flag to indicate if the game is over.
    float mGameOverTimer = 0.0f;  // Timer to track game over state.
    float mTimeSoFar = 0.0f;      // Tracks total time elapsed in the mode.

    bool mWantsToQuit = false;  // Flag for if the game is wanting to quit.

    // InitBgnd function: Initializes background elements for the parallax effect.
    void InitBgnd();

    // UpdateBgnd function: Updates background elements for the parallax scrolling effect.
    void UpdateBgnd(float dTime);

    // EnterScoreData struct: Manages score entry process after game session.
    struct EnterScoreData
    {
    public:
        // Constructor: Sets up UI for score entry.
        EnterScoreData();

        // Destructor: Cleans up text elements used in score entry.
        ~EnterScoreData();

        // Update function: Handles logic for mode transition based on key input.
        void Update();

        // Render function: Draws text elements for score entry.
        void Render(DirectX::SpriteBatch& batch);

        // ProcessKey function: Handles keyboard input for name entry.
        void ProcessKey(char key);

        // InputScore function: Prepares score entry interface with current score.
        void InputScore(int _currScore) { 
            mScoreText->mString += std::to_string(_currScore);  // Update the score text
            mScoreText->CentreOriginX();                        // Center the score text

            isFinished = false;  // Reset the finished flag
        }

        // Finish function: Finalizes score entry and prepares for subsequent steps.
        void Finish();

    private:
        std::vector<Text*> mTexts;  // Collection of text objects used in score entry UI.

        Text* mEnterNameText = nullptr;   // Prompt text for entering the player's name.
        Text* mScoreText = nullptr;       // Displays the player's score.
        Text* mNameText = nullptr;        // Shows the name being entered by the player.
        Text* mFormatWarnText = nullptr;  // Warning text for incorrect format or invalid input.

        std::string mNameString = "";  // Stores the player's entered name.

        bool isFinished = false;  // Flag to indicate completion of name entry. 
        // BUG: "/r" keeps sneaking its way into the name string so by using isFinished
        // as a flag for indication of wether the use is done or not we can stop the
        // ProcessKey function from adding that to the string.       07/01/24 - 16:11PM
    };

    EnterScoreData mScoreData;  // Instance of EnterScoreData for handling score entry.
    bool mEnteringScore;        // Flag indicating if score is currently being entered.
};