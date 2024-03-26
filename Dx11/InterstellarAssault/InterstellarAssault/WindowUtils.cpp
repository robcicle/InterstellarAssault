#include "WindowUtils.h"
#include "D3D.h"
#include "D3DUtil.h"

#include <cassert>

using namespace std;


// GetAppInst: Returns the application instance handle.
HINSTANCE WinUtil::GetAppInst() const
{
	return mWinData.hAppInst;
}

// GetMainWnd: Returns the main window handle.
HWND WinUtil::GetMainWnd() const
{
	return mWinData.hMainWnd;
}

// GetClientExtents: Retrieves the width and height of the client area of the main window.
void WinUtil::GetClientExtents(int& width, int& height) const
{
	width = mWinData.clientWidth; // Set output width to the client width.
	height = mWinData.clientHeight; // Set output height to the client height.
}

// GetAspectRatio: Calculates and returns the aspect ratio of the client area.
float WinUtil::GetAspectRatio() const
{
	int w, h;
	GetClientExtents(w, h); // Get current client extents.
	return static_cast<float>(w) / h; // Return aspect ratio (width / height).
}

// RealDefaultMssgHandler: Internal message handler for the main window.
LRESULT WinUtil::RealDefaultMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// Handle window activation/deactivation.
	case WM_ACTIVATE:
		mWinData.appPaused = LOWORD(wParam) == WA_INACTIVE;
		return 0;

		// Handle window resizing.
	case WM_SIZE:
		// Update client dimensions.
		mWinData.clientWidth = LOWORD(lParam);
		mWinData.clientHeight = HIWORD(lParam);

		// Handle window state changes (minimized, maximized, restored).
		if (mpMyD3D && mpMyD3D->GetDeviceReady())
		{
			if (wParam == SIZE_MINIMIZED)
				mWinData.appPaused = mWinData.minimized = true, mWinData.maximized = false;
			else if (wParam == SIZE_MAXIMIZED)
				mWinData.appPaused = mWinData.minimized = false, mWinData.maximized = true, mpMyD3D->OnResize(mWinData.clientWidth, mWinData.clientHeight, *mpMyD3D);
			else if (wParam == SIZE_RESTORED)
			{
				mWinData.appPaused = false;
				if (mWinData.minimized || mWinData.maximized)
					mpMyD3D->OnResize(mWinData.clientWidth, mWinData.clientHeight, *mpMyD3D);
				mWinData.minimized = mWinData.maximized = false;
			}
		}
		return 0;

		// Handle enter/exit of resize mode.
	case WM_ENTERSIZEMOVE:
		mWinData.appPaused = mWinData.resizing = true;
		return 0;
	case WM_EXITSIZEMOVE:
		mWinData.appPaused = mWinData.resizing = false;
		if (mpMyD3D)
			mpMyD3D->OnResize(mWinData.clientWidth, mWinData.clientHeight, *mpMyD3D);
		return 0;

		// Handle window focus changes.
	case WM_KILLFOCUS:
		mWinData.appPaused = true;
		return 0;
	case WM_SETFOCUS:
		mWinData.appPaused = false;
		return 0;

		// Handle window destruction.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// Handle system commands.
	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);

		// Handle minimum window size.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
	}

	// Default message handling.
	return DefWindowProc(hwnd, msg, wParam, lParam);
}


// InitMainWindow: Initializes the main application window with specified dimensions, instance, name, and message handler.
bool WinUtil::InitMainWindow(int width, int height, HINSTANCE hInstance, const std::string& appName, WNDPROC mssgHandler, bool centred)
{
    // Basic run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    assert(mssgHandler); // Ensure a valid message handler is provided.
    mWinData.mainWndCaption = appName;
    mWinData.hAppInst = hInstance;
    mWinData.clientWidth = width;
    mWinData.clientHeight = height;

    // Window class definition.
    WNDCLASS wc = { 0 };
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = mssgHandler;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = mWinData.hAppInst;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = "D3DWndClassName";

    // Register window class.
    if (!RegisterClass(&wc))
    {
        MessageBox(0, "RegisterClass Failed.", 0, 0);
        return false;
    }

    // Compute window rectangle dimensions based on requested client area dimensions.
    RECT R = { 0, 0, mWinData.clientWidth, mWinData.clientHeight };
    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
    int w = R.right - R.left;
    int h = R.bottom - R.top;

    // Determine window position.
    int offsetX = CW_USEDEFAULT, offsetY = CW_USEDEFAULT;
    if (centred) {
        offsetY = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;
        offsetX = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
    }

    // Create window with specified properties.
    mWinData.hMainWnd = CreateWindow("D3DWndClassName", mWinData.mainWndCaption.c_str(),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        offsetX, offsetY, w, h, 0, 0, mWinData.hAppInst, 0);
    if (!mWinData.hMainWnd)
    {
        MessageBox(0, "CreateWindow Failed.", 0, 0);
        return false;
    }

    ShowWindow(mWinData.hMainWnd, SW_SHOW);
    UpdateWindow(mWinData.hMainWnd);

    return true;
}

// BeginLoop: Starts the message loop for the application. Checks for window messages and updates render state.
bool WinUtil::BeginLoop(bool& canUpdateRender) const
{
    MSG msg = { 0 };
    canUpdateRender = false;

    // Process any window messages.
    while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT)
            return false;
    }

    // Check if application is paused.
    if (!mWinData.appPaused)
        canUpdateRender = true;

    return true;
}

// EndLoop: Ends the message loop for the application. Returns the time delta for the frame.
float WinUtil::EndLoop(bool didUpdateRender) const
{
    if (!didUpdateRender)
        return 0;
    float deltaTime = 0;

    // Calculate time delta if application is not paused.
    if (!mWinData.appPaused)
    {
        __int64 countsPerSec = 0;
        QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
        double secondsPerCount = 1.0 / (double)countsPerSec;
        static __int64 sTime1 = 0;
        __int64 time2 = 0;
        QueryPerformanceCounter((LARGE_INTEGER*)&time2);
        if (sTime1 != 0)
            deltaTime = (float)((time2 - sTime1) * secondsPerCount);
        sTime1 = time2;
        AddSecToClock(deltaTime);
    }
    else
        Sleep(100);

    return deltaTime;
}

// Run: Main application loop function. Handles update and render callbacks.
int WinUtil::Run(void(*pUpdate)(float), void(*pRender)(float)) const
{
    MSG msg = { 0 };
    assert(pUpdate && pRender);

    __int64 countsPerSec = 0;
    QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
    double secondsPerCount = 1.0 / (double)countsPerSec;

    float deltaTime = 0;
    while (msg.message != WM_QUIT)
    {
        // Process window messages.
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // Update and render if application is not paused.
            if (!mWinData.appPaused)
            {
                pUpdate(deltaTime);
                pRender(deltaTime);

                static __int64 sTime1 = 0;
                __int64 time2 = 0;
                QueryPerformanceCounter((LARGE_INTEGER*)&time2);
                if (sTime1 != 0)
                    deltaTime = (float)((time2 - sTime1) * secondsPerCount);
                sTime1 = time2;
            }
            else
                Sleep(100);
        }
    }
    return (int)msg.wParam;
}

// ChooseRes: Determines the screen resolution from a list of defaults.
void WinUtil::ChooseRes(int& w, int& h, int defaults[], int numPairs) const
{
    assert(defaults && numPairs > 0);

    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);

    w = 0;
	h = 0;
	for (int i = 0; i < numPairs; ++i)
	{
		int newW = defaults[i * 2], newH = defaults[i*2+1];

		if (w < newW && newW < sw && newH < sh)
		{
			w = newW;
			h = newH;
		}
	}
	assert(w > 0);
}

