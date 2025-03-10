#pragma once

#include <vector>
#include <string>

#include "D3D.h"
#include "SpriteBatch.h"

// AMode Class: Abstract base class representing a game mode (like intro, game, gameOver, highScores, etc.).
class AMode
{
public:
    AMode() {}
    virtual ~AMode() {}

    // Exit method: Called when switching from this mode to another. Allows for effects like fading out.
    // The switch won't complete until this returns true, providing time for transition effects.
    virtual bool Exit() { return true; }

    // Enter method: Called once when the mode is activated for the first time.
    virtual void Enter() {}

    // Update method: Contains the logic of the mode, called repeatedly once active.
    virtual void Update(float dTime) = 0;

    // Render method: Used by the mode to render itself.
    virtual void Render(float dTime, DirectX::SpriteBatch& batch) = 0;

    // GetMName method: Returns the name of the mode.
    virtual std::string GetMName() const = 0;

    // ProcessKey method: Passes WM_CHAR key messages to the mode for processing.
    virtual void ProcessKey(char key) {};

    // Reset method: Used for resetting any variables within the mode.
    virtual void Reset() {};
};

// ModeMgr Class: Manages all game mode instances and facilitates switching between them.
class ModeMgr
{
public:
    ~ModeMgr() {
        Release();  // Release all modes on destruction.
    }

    // SwitchMode method: Changes the active game mode.
    void SwitchMode(const std::string& newMode);

    // Update method: Updates the currently active mode.
    void Update(float dTime);

    // Render method: Renders the currently active mode.
    void Render(float dTime, DirectX::SpriteBatch& batch);

    // ProcessKey method: Sends a key input to the current mode.
    void ProcessKey(char key);

    // AddMode method: Adds a new mode. Modes should be dynamically allocated.
    void AddMode(AMode* p);

    // Release method: Frees all the mode instances. Can be called explicitly or left to the destructor.
    void Release();

    std::string GetCurrentModeStr() { return currentMode; }

private:
    std::vector<AMode*> mModes;  // Container of all game modes.
    int mCurrentMIdx = -1;       // Index of the currently active mode.
    int mDesiredMIdx = -1;       // Index of the desired active mode.
    int mSwitchCount = 0;        // Counter for mode switches.

    std::string currentMode = "";
};