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
	
	// Load music from files into music objects.
	mMusicList[AUTOMATION_SONG] = std::make_unique<SoundEffect>(mAudEngine.get(), L"data/audio/music/automation_song.wav");
	mMusicList[TRASHY_SONG] = std::make_unique<SoundEffect>(mAudEngine.get(), L"data/audio/music/trashy_song.wav");

#if defined(DEBUG) || defined(_DEBUG)
	AdjustMasterVolume(0.1f);
#endif
}

// AudioManager Destructor: Handles the cleanup of audio engine and sound effects.
AudioManager::~AudioManager()
{
	if (mAudEngine)
		mAudEngine->Suspend();  // Suspend the audio engine.

	mAudEngine = nullptr;  // Reset the audio engine pointer.

	// Reset sound effects.
	for (int i = 0; i < SoundList::STOTAL; i++)
		mSoundEffects[i] = nullptr;

	// Reset music.
	for (int i = 0; i < MusicList::MTOTAL; i++)
		mMusicList[i] = nullptr;
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
	instance->SetVolume(volume * mGameVolume);  // Adjust volume based on game settings.
	instance->Play();  // Play the sound effect instance.
	return instance.release();  // Release and return the instance.
}

SoundEffectInstance* AudioManager::CreateMusicInstance(MusicList songToPlay, float volume)
{
	// Create a new instance of the specified song.
	std::unique_ptr<SoundEffectInstance> instance = mMusicList[songToPlay]->CreateInstance();
	instance->SetVolume(volume * mMusicVolume);  // Adjust volume based on game settings.
	return instance.release();  // Release and return the instance.
}
