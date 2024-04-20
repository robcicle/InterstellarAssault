#pragma once

#include <Audio.h>
#include "D3D.h"

// AudioManager Class: Manages all audio-related functionality in the game, including sound effects and volume control.
class AudioManager
{
public:
    // Constructor: Initializes the audio engine.
    AudioManager();

    // Destructor: Handles cleanup of the audio engine.
    ~AudioManager();

    // Update function: Updates the state of the audio engine.
    void Update();

    // SoundList Enum: Enumerates all different types of sound effects used in the game.
    enum SoundList {
        MISSILE_SHOOT,
        LAZER_SHOOT,
        EXPLOSION,
        HIT,
        ENTER,
        SELECT,
        STOTAL  // Number of sound effects.
    };

    // MusicList Enum: Enumerates all different types of music used in the game.
    enum MusicList {
        AUTOMATION_SONG,
        TRASHY_SONG,
        MTOTAL  // Number of songs.
    };

    // PlaySoundEffect function: Plays the specified sound effect at the given volume.
    void PlaySoundEffect(SoundList soundToPlay, float volume = 1.0f) {
        mSoundEffects[soundToPlay]->Play(volume * mGameVolume, 0.0f, 0.0f);
    }

    // CreateSFXInstance function: Creates an instance of a sound effect that can be controlled.
    DirectX::SoundEffectInstance* CreateSFXInstance(SoundList soundToPlay, float volume = 1.0f);

    // CreateSFXInstance function: Creates an instance of a song that can be controlled.
    DirectX::SoundEffectInstance* CreateMusicInstance(MusicList songToPlay, float volume = 1.0f);

    // Functions to adjust master, game, and music volumes.
    void AdjustMasterVolume(float newMasterVol) { mAudEngine->SetMasterVolume(newMasterVol); }
    void AdjustGameVolume(float newGameVol) { mGameVolume = newGameVol; }
    void AdjustMusicVolume(float newMusicVol) { mMusicVolume = newMusicVol; }

    // Getters for current volume settings.
    float GetMasterVolume() { return mAudEngine->GetMasterVolume(); }
    float GetGameVolume() const { return mGameVolume; }
    float GetMusicVolume() const { return mMusicVolume; }

private:
    float mGameVolume = 1.0f;   // Default game volume.
    float mMusicVolume = 1.0f;  // Default music volume.

    std::unique_ptr<DirectX::AudioEngine> mAudEngine;                        // The audio engine.
    std::unique_ptr<DirectX::SoundEffect> mSoundEffects[SoundList::STOTAL];  // Array of sound effects.
    std::unique_ptr<DirectX::SoundEffect> mMusicList[MusicList::MTOTAL];     // Array of sound effects.
};