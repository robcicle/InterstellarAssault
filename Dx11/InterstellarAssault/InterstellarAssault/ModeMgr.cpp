#include "ModeMgr.h"
#include "PlayMode.h"


// SwitchMode: Switches to a new game mode based on the provided mode name.
void ModeMgr::SwitchMode(const std::string& newMode) {
    int idx = 0;
    assert(!mModes.empty()); // Ensure there are modes available.
    // Find the index of the mode with the matching name.
    while (idx < (int)mModes.size() && mModes[idx]->GetMName() != newMode)
        ++idx;

    assert(idx < (int)mModes.size()); // Ensure the mode is found.
    mDesiredMIdx = idx; // Set the desired mode index.
    mModes[mDesiredMIdx]->Reset();

    currentMode = newMode;
}

// Update: Updates the current mode or handles the mode transition.
void ModeMgr::Update(float dTime) {
    if (mDesiredMIdx != mCurrentMIdx)
        if (mCurrentMIdx == -1 || mModes[mCurrentMIdx]->Exit())
        {
            mCurrentMIdx = mDesiredMIdx; // Switch to the desired mode.
            mModes[mCurrentMIdx]->Enter(); // Enter the new mode.
        }
    mModes[mCurrentMIdx]->Update(dTime); // Update the active mode.
}

// Render: Renders the current mode.
void ModeMgr::Render(float dTime, DirectX::SpriteBatch& batch) {
    if (mCurrentMIdx >= 0 && mCurrentMIdx < (int)mModes.size())
        mModes[mCurrentMIdx]->Render(dTime, batch); // Render the active mode.
}

// ProcessKey: Passes key input to the current mode for processing.
void ModeMgr::ProcessKey(char key) {
    if (mCurrentMIdx >= 0 && mCurrentMIdx < (int)mModes.size())
        mModes[mCurrentMIdx]->ProcessKey(key); // Process key in the active mode.
}

// AddMode: Adds a new mode to the manager.
void ModeMgr::AddMode(AMode* p) {
    assert(p); // Ensure the mode is valid.
    mModes.push_back(p); // Add the mode to the list.
}

// Release: Frees all modes and clears the mode list.
void ModeMgr::Release() {
    for (size_t i = 0; i < mModes.size(); ++i)
        delete mModes[i]; // Delete each mode.

    mModes.clear(); // Clear the list of modes.
}