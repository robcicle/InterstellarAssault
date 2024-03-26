#include "IntroMode.h"
#include "Game.h"
#include "WindowUtils.h"
#include "PlayMode.h"
#include "D3D.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;


const std::string IntroMode::MODE_NAME = "INTRO";

// Constructor: Sets up the initial lighting and other configurations for the intro mode.
IntroMode::IntroMode()
{
	MyD3D& d3d = WinUtil::Get().GetD3D();

	// Setting up directional light for the scene.
	d3d.GetFX().SetupDirectionalLight(0, true, Vector3(-1.2f, -0.7f, 0.7f), Vector3(0.75f, 0.75f, 0.75f), Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f));
}

// Update function: Manages the transition sequence and camera movement for the intro.
void IntroMode::Update(float dTime)
{
	Game& gm = Game::Get();
	mTimer += dTime;

	// Move the camera smoothly to a target position over a period of time.
	if (mTimer >= 2.5f && gm.mCamPos != GC::CAMERA_END_POS)
	{
		mCameraTimer += dTime;
		gm.mCamPos = DirectX::SimpleMath::Vector3::SmoothStep(GC::DEF_CAM_POS, GC::CAMERA_END_POS, mCameraTimer / 5);
	}
	// Transition to the main menu once the camera has reached its end position.
	else if (gm.mCamPos == GC::CAMERA_END_POS)
	{
		mTransitionTimer += dTime;
		if (mTransitionTimer >= 1.0f)
		{
			gm.GetModeMgr().SwitchMode("MAINMENU");
			gm.ChangeBackgroundColour((Vector4)Colors::DarkBlue);
		}
	}
}

// Render function: Renders the 3D models specific to the intro mode.
void IntroMode::Render(float dTime, DirectX::SpriteBatch& batch)
{
	MyD3D& d3d = WinUtil::Get().GetD3D();
	Game& gm = Game::Get();

	// Render the arcade machine and warehouse models.
	d3d.GetFX().Render(gm.mModels[Game::Modelid::ARCADEMACHINE]);
	d3d.GetFX().Render(gm.mModels[Game::Modelid::WAREHOUSE]);
}

#if defined(DEBUG) || defined(_DEBUG)
#include "MainMenuMode.h"
// ProcessKey function: Handles debug camera controls and the escape key functionality.
void IntroMode::ProcessKey(char key)
{
	// Debugging controls for adjusting camera position.
	switch (key)
	{
	case 'a': // Move camera up.
		Game::Get().mCamPos.y += GC::DEBUG_CAM_INC;
		break;
	case 'z': // Move camera down.
		Game::Get().mCamPos.y -= GC::DEBUG_CAM_INC;
		break;
	case 'd': // Move camera left.
		Game::Get().mCamPos.x -= GC::DEBUG_CAM_INC;
		break;
	case 'f': // Move camera right.
		Game::Get().mCamPos.x += GC::DEBUG_CAM_INC;
		break;
	case 'w': // Move camera forward.
		Game::Get().mCamPos.z += GC::DEBUG_CAM_INC;
		break;
	case 's': // Move camera backward.
		Game::Get().mCamPos.z -= GC::DEBUG_CAM_INC;
		break;
	case 'x': // Reset camera position.
		Game::Get().mCamPos = GC::DEF_CAM_POS;
		break;
	case 'q': // Switch to play mode.
	case 'Q':
		Game::Get().GetModeMgr().SwitchMode(MainMenuMode::MODE_NAME);
		Game::Get().ChangeBackgroundColour((Vector4)Colors::DarkBlue);
		break;
	}

	// Exit the game when the escape key is pressed.
	switch (key)
	{
	case VK_ESCAPE:
		PostQuitMessage(0);
		break;
	}
}
#endif