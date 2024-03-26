#include "AudioManager.h"
#include "D3DUtil.h"

using namespace DirectX;


// AudioManager Constructor: Initializes the audio engine and loads sound effects.
AudioManager::AudioManager()
{
	// Set up the audio engine with appropriate flags.
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#if defined(DEBUG) || defined(_DEBUG)
	eflags |= AudioEngine_Debug;  // Enable debug mode for audio in debug builds.
#endif
	mAudEngine = std::make_unique<AudioEngine>(eflags);

	// Load sound effects from files into sound effect objects.
	mSoundEffects[MISSILE_SHOOT] = std::make_unique<SoundEffect>(mAudEngine.get(), L"data/audio/missile_shoot.wav");
	mSoundEffects[LAZER_SHOOT] = std::make_unique<SoundEffect>(mAudEngine.get(), L"data/audio/laser_shoot.wav");
	mSoundEffects[EXPLOSION] = std::make_unique<SoundEffect>(mAudEngine.get(), L"data/audio/explosion.wav");
	mSoundEffects[HIT] = std::make_unique<SoundEffect>(mAudEngine.get(), L"data/audio/hit.wav");
	mSoundEffects[ENTER] = std::make_unique<SoundEffect>(mAudEngine.get(), L"data/audio/enter.wav");
	mSoundEffects[SELECT] = std::make_unique<SoundEffect>(mAudEngine.get(), L"data/audio/select.wav");
}

// AudioManager Destructor: Handles the cleanup of audio engine and sound effects.
AudioManager::~AudioManager()
{
	if (mAudEngine)
		mAudEngine->Suspend();  // Suspend the audio engine.

	mAudEngine = nullptr;  // Reset the audio engine pointer.

	// Reset sound effects.
	for (int i = 0; i < SoundList::TOTAL; i++)
		mSoundEffects[i] = nullptr;
}

// Update method: Updates the audio engine state.
void AudioManager::Update()
{
	// Assert if audio engine fails to update.
	if (!mAudEngine->Update())
		assert(mAudEngine);
}

// CreateSFXInstance method: Creates and plays a sound effect instance.
SoundEffectInstance* AudioManager::CreateSFXInstance(SoundList soundToPlay, float volume)
{
	// Create a new instance of the specified sound effect.
	std::unique_ptr<SoundEffectInstance> instance = mSoundEffects[soundToPlay]->CreateInstance();
	instance->SetVolume(volume / mGameVolume);  // Adjust volume based on game settings.
	instance->Play();  // Play the sound effect instance.
	return instance.release();  // Release and return the instance.
}