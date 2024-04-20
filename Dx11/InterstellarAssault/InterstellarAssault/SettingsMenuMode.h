#pragma once

#include "ModeMgr.h"
#include "Text.h"
#include "UIManager.h"

#include <sstream>

// SettingsMenuMode class: Manages the settings menu in the game.
class SettingsMenuMode : public AMode
{
public:
    static const std::string MODE_NAME; // Static constant for the mode name.

    // Constructor: Initializes the settings menu mode.
    SettingsMenuMode();

    // Destructor: Cleans up resources used by the settings menu mode.
    ~SettingsMenuMode();

    // Update function: Updates the settings menu mode logic.
    void Update(float dTime) override;

    // Render function: Renders the settings menu UI elements.
    void Render(float dTime, DirectX::SpriteBatch& batch) override;

    // GetMName function: Returns the name of the mode.
    std::string GetMName() const override {
        return "SETTINGSMENU";
    }

    // Reset function: Resets the settings menu to its initial state.
    void Reset() override;

private:
    void AdjustMasterVolume();
    void AdjustMusicVolume();
    void AdjustGameVolume();

    std::vector<Text*> mTexts;       // Collection of Text objects for rendering.

    Text* mSettingsTitleText = nullptr;  // Text object for the settings menu title.

    UIManager mUIMgr;                  // UI Manager for managing UI elements in the settings menu.

    float mCurrentMasterVolume = 0;      // Current master volume setting.
    Text* mMasterVolumeText = nullptr;   // Text object for displaying the master volume.

    float mCurrentMusicVolume = 0;      // Current music volume setting.
    Text* mMusicVolumeText = nullptr;   // Text object for displaying the music volume.

    float mCurrentGameVolume = 0;      // Current game volume setting.
    Text* mGameVolumeText = nullptr;   // Text object for displaying the game volume.
};