#include "Input.h"
#include "D3D.h"
#include "D3DUtil.h"
#include "WindowUtils.h"

#include <math.h>
#include <cassert>
#include <sstream>

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;


// IsPressed: Checks if a key is currently pressed.
bool MouseAndKeys::IsPressed(unsigned short vkeyCode) const
{
	assert(vkeyCode < KEYBUFF_SIZE); // Ensure key code is within buffer size.
	return mKeyBuffer[vkeyCode] != 0; // Return true if key is pressed.
}

// IsDown: Checks if a key was pressed in the current frame but not in the previous frame.
bool MouseAndKeys::IsDown(unsigned short vkeyCode) const
{
	assert(vkeyCode < KEYBUFF_SIZE); // Ensure key code is within buffer size.
	return !mPrevKeyBuffer[vkeyCode] && mKeyBuffer[vkeyCode];
}

// IsUp: Checks if a key was released in the current frame.
bool MouseAndKeys::IsUp(unsigned short vkeyCode) const
{
	assert(vkeyCode < KEYBUFF_SIZE); // Ensure key code is within buffer size.
	return mPrevKeyBuffer[vkeyCode] && !mKeyBuffer[vkeyCode];
}

// ProcessKeys: Processes keyboard input messages.
void MouseAndKeys::ProcessKeys(RAWINPUT* raw)
{
	// Standard key code processing.
	unsigned short flags = raw->data.keyboard.Flags;
	unsigned short vkey = raw->data.keyboard.VKey;
	//the scan code might be useful, but we won't use it here
	unsigned short scanCode = raw->data.keyboard.MakeCode;

	//ignore anything larger than this, escape codes, fake keys or weird keys
	if (vkey >= 255)
		return;

	// Handling of special key codes and escape sequences.
	if (vkey == VK_SHIFT)
	{
		// correct left-hand / right-hand SHIFT (VK_LSHIFT / VK_RSHIFT)
		vkey = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);
	}
	else if (vkey == VK_NUMLOCK)
	{
		// correct PAUSE/BREAK and NUM LOCK silliness, and set the extended bit
		scanCode = (MapVirtualKey(vkey, MAPVK_VK_TO_VSC) | 0x100);
	}

	// e0 and e1 are escape sequences used for certain special keys, such as PRINT and PAUSE/BREAK.
	// see http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
	const bool isE0 = ((flags & RI_KEY_E0) != 0);
	const bool isE1 = ((flags & RI_KEY_E1) != 0);

	if (isE1)
	{
		// for escaped sequences, turn the virtual key into the correct scan code using MapVirtualKey.
		// however, MapVirtualKey is unable to map VK_PAUSE (this is a known bug), hence we map that by hand.
		if (vkey == VK_PAUSE)
			scanCode = 0x45;
		else
			scanCode = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	}


	switch (vkey)
	{
		// right-hand CONTROL and ALT have their e0 bit set
	case VK_CONTROL:
		if (isE0)
			vkey = VK_RCONTROL; //right control
		else
			vkey = VK_LCONTROL;  //left control
		break;

	case VK_MENU:
		if (isE0)
			vkey = VK_RALT;  //right alt
		else
			vkey = VK_LALT;	//left alt
		break;

		// NUMPAD ENTER has its e0 bit set
	case VK_RETURN:
		if (isE0)
			vkey = VK_NUMPAD_ENTER; //numpad enter
		break;
	}
	//ignore anything larger than this, escape codes, fake keys or weird keys
	if (vkey >= 255)
		return;

	// Updating key buffer states.
	if (flags & RI_KEY_BREAK) //key up
		mKeyBuffer[vkey] = 0;
	else
		mKeyBuffer[vkey] = scanCode;
}

// ProcessMouse: Processes mouse input messages.
void MouseAndKeys::ProcessMouse(RAWINPUT* raw)
{
	// Process mouse button state changes.
	unsigned short flags = raw->data.mouse.usButtonFlags;

	if (flags & RI_MOUSE_LEFT_BUTTON_DOWN)
		mButtons[LBUTTON] = true;
	if (flags & RI_MOUSE_LEFT_BUTTON_UP)
		mButtons[LBUTTON] = false;
	if (flags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
		mButtons[MBUTTON] = true;
	if (flags & RI_MOUSE_MIDDLE_BUTTON_UP)
		mButtons[MBUTTON] = false;
	if (flags & RI_MOUSE_RIGHT_BUTTON_DOWN)
		mButtons[RBUTTON] = true;
	if (flags & RI_MOUSE_RIGHT_BUTTON_UP)
		mButtons[RBUTTON] = false;

	Vector2 last(mMouseScreen);
	GetMousePosAbsolute(mMouseScreen);
	mMouseMove = mMouseScreen - last;
}

// GetMousePosAbsolute: Retrieves the current absolute mouse position.
void MouseAndKeys::GetMousePosAbsolute(Vector2& pos)
{
	// Retrieve and convert the current mouse position to a Vector2.
	POINT mpos;
	if (GetCursorPos(&mpos))
	{
		if (ScreenToClient(mHwnd, &mpos))
		{
			pos=Vector2((float)mpos.x,(float)mpos.y);
		}
	}
		
}

// MessageEvent: Handles raw input messages for keyboard and mouse.
void MouseAndKeys::MessageEvent(HRAWINPUT rawInput)
{
	// Process raw input data for keyboard and mouse.

	UINT dwSize;

	UINT res = GetRawInputData(rawInput, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	if (res != 0)
		return;//bad input so ignore it
	
	if (dwSize >= RAWBUFF_SIZE)
	{
		DBOUT("Buffer too small. Is " << RAWBUFF_SIZE << " wants " << dwSize);
		assert(false);
	}

	if (GetRawInputData(rawInput, RID_INPUT, mInBuffer, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
	{
		DBOUT("GetRawInputData failed");
		assert(false);
	}

	RAWINPUT* raw = (RAWINPUT*)mInBuffer;

	if (raw->header.dwType == RIM_TYPEKEYBOARD)
	{
		ProcessKeys(raw);
	}
	else if (raw->header.dwType == RIM_TYPEMOUSE)
	{
		ProcessMouse(raw);
	}
}

// GetPressedKeyNames: Constructs a string with names of currently pressed keys.
void MouseAndKeys::GetPressedKeyNames(wstring& mssg)
{
	// Construct a string listing all currently pressed keys.

	mssg = L"";
	for (int i = 0; i < KEYBUFF_SIZE; ++i)
	{
		if (mKeyBuffer[i] == 0)
			continue;
		//numeric pad and arrow keys conflict, ise0 is set for arrow keys
		//here we just assume the codes refer to arrow keys
		switch (i)
		{
		case VK_RIGHT:
			mssg += L"RIGHT ARROW ";
			break;
		case VK_LEFT:
			mssg += L"LEFT ARROW ";
			break;
		case VK_UP:
			mssg += L"UP ARROW ";
			break;
		case VK_DOWN:
			mssg += L"DOWN ARROW ";
			break;
		default:
			wchar_t key[256];
			if (GetKeyNameTextW(mKeyBuffer[i] << 16, key, sizeof(key)))
			{
				mssg += key;
				mssg += L" ";
			}
		}

	}
}

// GetMouseNames: Constructs a string with current mouse state information.
void MouseAndKeys::GetMouseNames(wstring& mssg)
{
	// Construct a string detailing the current state of the mouse.
	mssg = L"";
	static float oldX = 0, oldY = 0;
	if (!Equals(mMouseMove.x, 0))
		oldX = mMouseMove.x;
	if (!Equals(mMouseMove.y, 0))
		oldY = mMouseMove.y;
	wstringstream str;
	str.precision(0);
	str << L"x=" << mMouseScreen.x << L" y=" << mMouseScreen.y;
	str << L" xinc=" << oldX << L" yinc=" << oldY;
	if (mButtons[LBUTTON])
		str << L" LButton";
	if (mButtons[MBUTTON])
		str << L" MButton";
	if (mButtons[RBUTTON])
		str << L" RButton";

	mssg = str.str();
}

// Initialize: Sets up the raw input devices for capturing mouse and keyboard input.
void MouseAndKeys::Initialize(HWND hwnd, bool showMouse, bool confineMouse)
{
	mHwnd = hwnd; // Store the window handle.

	// Setup for capturing raw mouse and keyboard data.
	RAWINPUTDEVICE Rid[2];

	// Setup for mouse input.
	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x02;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = hwnd;

	// Setup for keyboard input.
	Rid[1].usUsagePage = 0x01;
	Rid[1].usUsage = 0x06;
	Rid[1].dwFlags = RIDEV_INPUTSINK;
	Rid[1].hwndTarget = hwnd;

	// Register the raw input devices.
	if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE) {
		MessageBox(0, "Cannot get keyboard and mouse input", 0, 0);
		assert(false);
	}

	ShowCursor(showMouse); // Show or hide the mouse cursor.
	mConfineMouse = confineMouse; // Set whether the mouse should be confined to the window.

	// Confine the cursor to the window if required.
	if (mConfineMouse)
	{
		GetWindowRect(hwnd, &mNewClip);
		ClipCursor(&mNewClip);
	}

	GetMousePosAbsolute(mMouseScreen); // Get the initial mouse position.
}

// RefreshState: Updates the previous state buffers for keys and mouse buttons.
void MouseAndKeys::RefreshState()
{
	memcpy(mPrevKeyBuffer, mKeyBuffer, 
		sizeof(mPrevKeyBuffer)); // Update previous key buffer.
	memcpy(mPrevButtons, mButtons, 
		sizeof(mPrevButtons));   // Update previous mouse button buffer.
}

// OnLost: Handles loss of input focus, resets input states and releases cursor confinement.
void MouseAndKeys::OnLost()
{
	Reset();               // Reset key and mouse states.
	ClipCursor(&mOldClip); // Release cursor confinement.
}

// OnReset: Handles regaining of input focus, resets input states and re-applies cursor confinement.
void MouseAndKeys::OnReset()
{
	Reset();               // Reset key and mouse states.
	ClipCursor(&mNewClip); // Re-apply cursor confinement.
}

// PostProcess: Resets mouse movement deltas after each frame.
void MouseAndKeys::PostProcess()
{
	mMouseMove.x = 0; // Reset horizontal mouse movement delta.
	mMouseMove.y = 0; // Reset vertical mouse movement delta.
}

//-------------------------------------------------------------
// Gamepad Class: Handles Gamepad input.

// Define the XButtonIDs struct as XBtns
XButtonIDs XBtns;

// XButtonIDs Default constructor
XButtonIDs::XButtonIDs()
{
	// These values are used to index the XINPUTbtns array,
	// using the matching XINPUT button value

	A = 0;
	B = 1;
	X = 2;
	Y = 3;

	DPadUp = 4;
	DPadDown = 5;
	DPadLeft = 6;
	DPadRight = 7;

	LShoulder = 8;
	RShoulder = 9;

	LThumbstick = 10;
	RThumbstick = 11;

	Start = 12;
	Back = 13;
}

// Default constructor for Gamepad: Initializes a gamepad for player 1.
Gamepad::Gamepad()
{
	mGamepadIndex = 0; // Default gamepad index for player 1.

	// Initialize button states for all buttons.
	for (int i = 0; i < mBtnCount; i++)
	{
		mPrevBtnStates[i] = false;
		mBtnStates[i] = false;
		mGamepadBtnsDown[i] = false;
	}
}

// Overloaded constructor for Gamepad: Initializes a gamepad for a specific player index.
Gamepad::Gamepad(int a_iIndex)
{
	// Set gamepad index based on provided player index.
	mGamepadIndex = a_iIndex - 1;

	// Initialize button states for all buttons.
	for (int i = 0; i < mBtnCount; i++)
	{
		mPrevBtnStates[i] = false;
		mBtnStates[i] = false;
		mGamepadBtnsDown[i] = false;
	}
}

// Update: Updates the state of the gamepad.
void Gamepad::Update(float dTime)
{
	mState = GetState(); // Retrieve current state of the gamepad.

	// Loop through all controller buttons to update their states.
	for (int i = 0; i < mBtnCount; i++)
	{
		// Update button state for the current frame.
		mBtnStates[i] = (mState.Gamepad.wButtons & XINPUT_Btns[i]) == XINPUT_Btns[i];

		// Set button down and up states for the current frame.
		mGamepadBtnsDown[i] = !mPrevBtnStates[i] && mBtnStates[i];
		mGamepadBtnsUp[i] = mPrevBtnStates[i] && !mBtnStates[i];
	}

	VibrateLogic(dTime); // Handle gamepad vibration logic.
}

// RefreshState: Updates the previous state of the buttons for the next frame.
void Gamepad::RefreshState()
{
	memcpy(mPrevBtnStates, mBtnStates, 
		sizeof(mPrevBtnStates)); // Update previous button states.
}

// Deadzone check for the Left Thumbstick
bool Gamepad::LStickInDeadzone() const
{
	// Get the X & Y axes of the stick
	short xAxis = mState.Gamepad.sThumbLX;
	short yAxis = mState.Gamepad.sThumbLY;

	// X axis is out of deadzone
	if (xAxis > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
		xAxis < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		return false;

	// Y axis is out of deadzone
	if (yAxis > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
		yAxis < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		return false;

	// One or more axis is within the deadzone
	return true;
}

// Deadzone check for the Right Thumbstick
bool Gamepad::RStickInDeadzone() const
{
	// Get the X & Y axes of the stick
	short xAxis = mState.Gamepad.sThumbRX;
	short yAxis = mState.Gamepad.sThumbRY;

	// X axis is out of deadzone
	if (xAxis > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
		xAxis < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		return false;

	// Y axis is out of deadzone
	if (yAxis > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
		yAxis < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		return false;

	// One or more axis is within the deadzone
	return true;
}

// Returns X axis of left stick
float Gamepad::LeftStickX() const
{
	// Get X axis of the left stick
	short xAxis = mState.Gamepad.sThumbLX;

	// Return axis value, casted to a float
	return (static_cast<float>(xAxis) / 32768.0f);
}

// Returns Y axis of left stick
float Gamepad::LeftStickY() const
{
	// Get Y axis of the left stick
	short yAxis = mState.Gamepad.sThumbLY;

	// Return axis value, casted to a float
	return (static_cast<float>(yAxis) / 32768.0f);
}

// Return X axis of the right stick
float Gamepad::RightStickX() const
{
	// Get X axis of right stick
	short xAxis = mState.Gamepad.sThumbRX;

	// Return axis value, casted to a float
	return (static_cast<float>(xAxis) / 32768.0f);
}

// Return Y axis of the right stick
float Gamepad::RightStickY() const
{
	// Get the Y axis of the left stick
	short yAxis = mState.Gamepad.sThumbRY;

	// Return axis value, casted to a float
	return (static_cast<float>(yAxis) / 32768.0f);
}

// Return value of the left trigger
float Gamepad::LeftTrigger() const
{
	// Get value of the left trigger
	BYTE trigger = mState.Gamepad.bLeftTrigger;

	if (trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		return trigger / 255.0f;

	return 0.0f; // Trigger is not being pressed
}

// Return value of the right trigger
float Gamepad::RightTrigger() const
{
	// Get value of the right trigger
	BYTE trigger = mState.Gamepad.bRightTrigger;

	if (trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		return trigger / 255.0f;

	return 0.0f; // Trigger is not being pressed
}

// GetButtonPressed: Returns true if the specified button is being held down.
bool Gamepad::GetButtonPressed(int a_iButton) const
{
	// Check if the specified button is being held down.
	return (mState.Gamepad.wButtons & XINPUT_Btns[a_iButton]) != 0;
}

// GetState: Returns the current state of the gamepad.
XINPUT_STATE Gamepad::GetState()
{
	XINPUT_STATE gamepadState;
	ZeroMemory(&gamepadState, sizeof(XINPUT_STATE)); // Clear memory for the state.

	// Get the state of the gamepad.
	XInputGetState(mGamepadIndex, &gamepadState);

	return gamepadState; // Return the gamepad state.
}

// IsConnected: Returns true if the gamepad is connected.
bool Gamepad::IsConnected()
{
	ZeroMemory(&mState, sizeof(XINPUT_STATE)); // Clear memory for the state.

	// Get the state of the gamepad.
	DWORD result = XInputGetState(mGamepadIndex, &mState);

	return result == ERROR_SUCCESS; // Return true if connected, else false.
}

// Vibrate: Controls the vibration motors of the gamepad.
void Gamepad::Vibrate(float a_fLeftMotor, float a_fRightMotor)
{
	XINPUT_VIBRATION vibrationState;
	ZeroMemory(&vibrationState, sizeof(XINPUT_VIBRATION)); // Clear memory for the vibration state.

	// Calculate vibration values and set them.
	vibrationState.wLeftMotorSpeed = int(a_fLeftMotor * 65535.0f);
	vibrationState.wRightMotorSpeed = int(a_fRightMotor * 65535.0f);

	// Set the vibration state of the gamepad.
	XInputSetState(mGamepadIndex, &vibrationState);
}

// VibrateLogic: Handles logic for vibrating the gamepad based on timer.
void Gamepad::VibrateLogic(float dTime)
{
	// If controller isn't connected or no vibration time is set, return.
	if (!IsConnected() || mTimeToVibrate <= 0)
		return;

	// Update the timer for vibration.
	mTimeToVibrate -= dTime;

	// Stop vibration if the time has elapsed.
	if (mTimeToVibrate <= 0)
	{
		Vibrate(0.0f, 0.0f); // Stop vibration.
		mIsVibrating = false;
	}
	else if (!mIsVibrating)
	{
		// Start vibration with the desired intensity.
		Vibrate(mVibrationIntensity[0], mVibrationIntensity[1]);
		mIsVibrating = true;
	}
	// BUG: The isVibrating check has to happen, for some reason
	// whenever you make the controller vibrate ALOT of frames
	// it just tanks performance, so by doing this it means
	// we're checking
}