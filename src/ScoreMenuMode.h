#pragma once

#include "ModeMgr.h"
#include "Text.h"
#include "UIManager.h"

#include <sstream>


// ScoreMenuMode class: Manages the score menu of the game.
class ScoreMenuMode : public AMode
{
public:
    static const std::string MODE_NAME; // Static constant for the mode name.

    // Constructor: Initializes the score menu with necessary UI elements.
    ScoreMenuMode();

    // Destructor: Cleans up resources used by the score menu mode.
    ~ScoreMenuMode();

    // Update function: Handles the update logic of the score menu mode.
    void Update(float dTime) override;

    // Render function: Renders the score menu UI elements.
    void Render(float dTime, DirectX::SpriteBatch& batch) override;

    // GetMName function: Returns the name of the mode.
    std::string GetMName() const override {
        return "SCOREMENU";
    }

    // Reset function: Resets the score menu to its initial state.
    void Reset() override;

    // ConvertScoresToString function: Converts the stored scores to a string format for display.
    void ConvertScoresToString();

private:
    std::vector<Text*> mTexts;       // Collection of Text objects for rendering.

    Text* mScoreTitleText = nullptr;  // Text object for displaying the score menu title.
    Text* mScoresText = nullptr;      // Text object for displaying the scores.

    UIManager mUIMgr;                 // UI Manager for managing UI elements in the score menu.

    std::stringstream mScoreSStream;  // Stream to format and store score text.
    size_t mScoresSize;               // Size of the scores to be displayed.
};