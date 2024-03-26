#pragma once

#include "ModeMgr.h"
#include "Sprite.h"
#include "SpriteFont.h"

class IntroMode : public AMode
{
public:
	static const std::string MODE_NAME;
	IntroMode();
	~IntroMode() {};
	void Update(float dTime) override;
	void Render(float dTime, DirectX::SpriteBatch& batch) override;
	std::string GetMName() const override {
		return "INTRO";
	}
#if defined(DEBUG) || defined(_DEBUG)
	void ProcessKey(char key) override;
#endif

private:
	float mTimer = 0;
	float mCameraTimer = 0;
	float mTransitionTimer = 0;
	bool mIsStartupFinished = false;
};