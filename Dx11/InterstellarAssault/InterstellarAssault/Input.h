#ifndef INPUT_H
#define INPUT_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#include <string>

#include "D3D.h"
#include "SimpleMath.h"

/*ideally we'd create a complete set of our key codes
completely independently of windows, but as I'm lazy
I'll just add a few extra codes to the VK_ list windows
provides in "winuser.h", to make it more consistent.
*/
#define VK_NUMPAD_ENTER 0xF0		//these three are very naughty
#define VK_LALT			0xF1		//they are actually some oem
#define VK_RALT			0xF2		//keys ATTN/FINISH/COPY - but lets pinch them :)


#define VK_0	0x30
#define VK_1	0x31
#define VK_2	0x32
#define VK_3	0x33
#define VK_4	0x34
#define VK_5	0x35
#define VK_6	0x36
#define VK_7	0x37
#define VK_8	0x38
#define VK_9	0x39
//these codes match ascii
#define VK_A	0x41
#define VK_B	0x42
#define VK_C	0x43
#define VK_D	0x44
#define VK_E	0x45
#define VK_F	0x46
#define VK_G	0x47
#define VK_H	0x48
#define VK_I	0x49
#define VK_J	0x4a
#define VK_K	0x4b
#define VK_L	0x4c
#define VK_M	0x4d
#define VK_N	0x4e
#define VK_O	0x4f
#define VK_P	0x50
#define VK_Q	0x51
#define VK_R	0x52
#define VK_S	0x53
#define VK_T	0x54
#define VK_U	0x55
#define VK_V	0x56
#define VK_W	0x57
#define VK_X	0x58
#define VK_Y	0x59
#define VK_Z	0x5a

/*
MouseAndKeys class: Simple class for handling inputs and movements
between the mouse and keyboard. Need to bypass the normal windows 
messages as we don't want repeat delays and other user-configurable 
interference. This class uses rawinput of keyboard and mouse data
*/
class MouseAndKeys
{
public:
	MouseAndKeys()
	{
		Reset();

	}
	void Reset()
	{
		ZeroMemory(mInBuffer, sizeof(mInBuffer));
		ZeroMemory(mKeyBuffer, sizeof(mKeyBuffer));
		ZeroMemory(mPrevKeyBuffer, sizeof(mPrevKeyBuffer));

		mButtons[0] = mButtons[1] = mButtons[2] = false;
		mMouseScreen = mMouseMove = DirectX::SimpleMath::Vector2(0, 0);
	}
	// start up the mouse/keys system, call once only
	// hwnd - main windows handle
	// showMouse - hide the mouse cursor
	// confineMouse - prevent the mouse leaving the window
	void Initialize(HWND hwnd, bool showMouse = true, bool confineMouse = false);

	// Refresh the buttons specific states
	void RefreshState();
	// Text list of all keys being held down
	void GetPressedKeyNames(std::wstring& mssg);
	// Is a specific key being pressed?
	bool IsPressed(unsigned short vkeyCode) const;
	// Is a specific key down?, for one frame
	bool IsDown(unsigned short vkeyCode) const;
	// Is a specific key up?, for one frame
	bool IsUp(unsigned short vkeyCode) const;
	// Text list of all Mouse buttons and cursor position info
	void GetMouseNames(std::wstring& mssg);
	// Check if a button is currently down
	typedef enum { LBUTTON = 0, MBUTTON = 1, RBUTTON = 2 } ButtonT;
	bool GetMouseButton(ButtonT b) const {
		return mButtons[b];
	}
	bool MouseAndKeys::GetMouseButtonDown(ButtonT b) const {
		return mButtons[b] && !mPrevButtons[b];
	}
	bool MouseAndKeys::GetMouseButtonUp(ButtonT b) const {
		return !mButtons[b] && mPrevButtons[b];
	}

	//Get relative or absolute mouse position
	//absolute=true, 2D Windows coordinates of cursor
	//absolute=false, 2D Windows coordinate change in position since last update (push)
	DirectX::SimpleMath::Vector2 GetMousePos(bool absolute) const {
		if (absolute)
			return mMouseScreen;
		return mMouseMove;
	}

	// Call this after game has finished using the mouse, probably at end of render
	void PostProcess();
	// For consistency we can reset mouse input after task switch (window<->fullscreen), optional
	void OnLost();
	void OnReset();

	// Add to your windows message pump
	// rawInput comes from lparam after a WM_INPUT message
	// case WM_INPUT:
	//	 input.MessageEvent((HRAWINPUT)lParam);
	void MessageEvent(HRAWINPUT rawInput);

private:
	// Copy of main window handle
	HWND mHwnd;

	enum { RAWBUFF_SIZE = 512, KEYBUFF_SIZE = 255, KMASK_IS_DOWN = 1, MAX_BUTTONS = 3 };
	// Raw input buffer
	BYTE mInBuffer[RAWBUFF_SIZE];
	unsigned short mPrevKeyBuffer[KEYBUFF_SIZE];  // Virtual key codes, but only the standard ones (first 256
	unsigned short mKeyBuffer[KEYBUFF_SIZE];      // Virtual key codes, but only the standard ones (first 256)

	// Support for three mouse buttons
	bool mButtons[MAX_BUTTONS];
	bool mPrevButtons[MAX_BUTTONS];
	// Track mouse position changes (2D)
	DirectX::SimpleMath::Vector2 mMouseScreen, mMouseMove;
	//if the mouse is confined, track the size of the window
	bool mConfineMouse;
	RECT mNewClip;           // new area for ClipCursor
	RECT mOldClip;        // previous area for ClipCursor

	void ProcessKeys(RAWINPUT* raw);
	void ProcessMouse(RAWINPUT* raw);
	void GetMousePosAbsolute(DirectX::SimpleMath::Vector2& pos);
};

// Gamepad Input Handlers

// Simple array of xInput buttons to easily
// find what value we need with just a matching
// integer which will be determined in a struct
// known as XButtonIDs
static const WORD XINPUT_Btns[] = {
	  XINPUT_GAMEPAD_A,
	  XINPUT_GAMEPAD_B,
	  XINPUT_GAMEPAD_X,
	  XINPUT_GAMEPAD_Y,
	  XINPUT_GAMEPAD_DPAD_UP,
	  XINPUT_GAMEPAD_DPAD_DOWN,
	  XINPUT_GAMEPAD_DPAD_LEFT,
	  XINPUT_GAMEPAD_DPAD_RIGHT,
	  XINPUT_GAMEPAD_LEFT_SHOULDER,
	  XINPUT_GAMEPAD_RIGHT_SHOULDER,
	  XINPUT_GAMEPAD_LEFT_THUMB,
	  XINPUT_GAMEPAD_RIGHT_THUMB,
	  XINPUT_GAMEPAD_START,
	  XINPUT_GAMEPAD_BACK
};

// XInput Button IDs
// Struct which holds integers that represent
// an XInput button for our XINPUT_Btns array
struct XButtonIDs
{
	XButtonIDs();  // Default constructor

	int A, B, X, Y;  // Action buttons

	int DPadUp, DPadDown, DPadLeft, DPadRight;  // D-Pad buttons

	int LShoulder, RShoulder;  // Bumper buttons

	int LThumbstick, RThumbstick;  // Thumbstick buttons

	int Start;  // Start button
	int Back;   // Back button
};

/*
Gamepad class: Simple class for handling the needed inputs
and movements of our GamePad/controllers for the game to
use and implement for its features
*/
class Gamepad
{
public:
	// Constructors
	Gamepad();
	Gamepad(int a_iIndex);

	void Update(float dTime);  // Updates the controller state
	void RefreshState();       // Updates individual button states for next frame

	// Thumbstick functions
	// If stick is in deadzone = true, otherwise = false
	bool LStickInDeadzone() const;
	bool RStickInDeadzone() const;

	float LeftStickX() const;   // Return the X axis of left stick
	float LeftStickY() const;   // Return the Y axis of left stick
	float RightStickX() const;  // Return the X axis of right stick
	float RightStickY() const;  // Return the Y axis of right stick

	// Trigger functions
	float LeftTrigger() const;   // Return value of the left trigger
	float RightTrigger() const;  // Return value of the right trigger

	// Button functions
	bool GetButtonPressed(int a_iButton) const;  // If button is held then return true

	// Frame-specific version of GetButtonPressed function. Is only 
	// captured in one frame until it is released and then pressed again
	bool GetButtonDown(int a_iButton) const { return mGamepadBtnsDown[a_iButton]; };  // If button is pressed down, for one frame, return true

	// Frame-specific version of GetButtonPressed function. Is only 
	// captured in one frame until it is released and then pressed again
	bool GetButtonUp(int a_iButton) const { return mGamepadBtnsUp[a_iButton]; };  // If button is pressed up, for one frame, return true

	// Utility functions
	XINPUT_STATE GetState();  // Return controller state
	int GetIndex() const { return mGamepadIndex; }           // Return controller index
	bool IsConnected();       // Return true if controller is connected

	// Vibrate the gamepad
	// First variable is left motor, second is right motor
	// 0.0f-1.0f, none-max speed
	void Vibrate(float a_fLeftMotor = 0.0f, float a_fRightMotor = 0.0f);
	// Vibrate the gamepad over a certain amount of time
	void VibrateOnTimer(float _timeToVibrate, float _leftMotor = 0.0f, float _rightMotor = 0.0f) {
		// Set our class' variables as the ones passed through
		mTimeToVibrate = _timeToVibrate;
		mVibrationIntensity[0] = _leftMotor;
		mVibrationIntensity[1] = _rightMotor;
	};

private:
	// Logic behind vibrating the gamepad over an amount of time
	void VibrateLogic(float dTime);

	XINPUT_STATE mState; // Current controller state
	int mGamepadIndex;   // Current controller index
	// e.g. 1st,2nd,3rd&4th player's controller

	static const int mBtnCount = 14;  // Total controller buttons
	bool mPrevBtnStates[mBtnCount];    // Previous frames button states
	bool mBtnStates[mBtnCount];        // Current frames button states

	float mTimeToVibrate = 0.0f;                // Time the gamepad vibrates for
	float mVibrationIntensity[2] = { 0.0f,0.0f };  // Array of two floats which hold desired intensity for vibration

	// If true then button was pressed down
	bool mGamepadBtnsDown[mBtnCount];  // Buttons pressed down on current frame
	bool mGamepadBtnsUp[mBtnCount];    // Buttons released for current frame

	bool mIsVibrating = false;
};

#endif

// Externally define the XButtonIDs struct as XBtns
extern XButtonIDs XBtns;