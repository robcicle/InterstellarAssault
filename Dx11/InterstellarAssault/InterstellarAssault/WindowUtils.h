#ifndef WINDOWSUTILS
#define WINDOWSUTILS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <sstream>
#include <assert.h>

#include "Singleton.h"


class MyD3D;

// WinUtil Class: Singleton class for handling Windows utility functions.
class WinUtil : public Singleton<WinUtil>
{
public:
    WinUtil()
        : mpMyD3D(nullptr) // Initialize D3D pointer to nullptr.
    {}

    // DefaultMssgHandler: Static method for default message handling.
    static LRESULT DefaultMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        return WinUtil::Get().RealDefaultMssgHandler(hwnd, msg, wParam, lParam);
    }

    // InitMainWindow: Initializes the main application window.
    bool InitMainWindow(int width, int height, HINSTANCE hInstance, const std::string& appName, WNDPROC mssgHandler, bool centred = true);

    // SetD3D: Sets the MyD3D instance for this utility class.
    void SetD3D(MyD3D& d3d) {
        assert(mpMyD3D == nullptr); // Ensure no previous D3D instance is set.
        mpMyD3D = &d3d; // Set the D3D instance.
    }

    // GetD3D: Retrieves the MyD3D instance.
    MyD3D& GetD3D() {
        assert(mpMyD3D); // Ensure D3D instance is set.
        return *mpMyD3D; // Return the D3D instance.
    }

    // Run: Main loop wrapper for simple function calls.
    int Run(void(*pUpdate)(float), void(*pRender)(float)) const;

    // BeginLoop: Starts the loop for class member functions.
    bool BeginLoop(bool& canUpdateRender) const;

    // EndLoop: Ends the loop for class member functions.
    float EndLoop(bool didUpdateRender) const;

    // Getters for application instance, main window handle, and client dimensions.
    HINSTANCE GetAppInst() const;
    HWND GetMainWnd() const;
    void GetClientExtents(int& width, int& height) const;
    int GetClientWidth() const {
        return mWinData.clientWidth;
    }
    int GetClientHeight() const {
        return mWinData.clientHeight;
    }

    // GetAspectRatio: Returns the aspect ratio of the client area.
    float GetAspectRatio() const;

    // ChooseRes: Allows selection of screen resolution from a list of defaults.
    void ChooseRes(int& w, int& h, int defaults[], int numPairs) const;

private:
    // WinData: Struct to store window-related data.
    struct WinData
    {
        // Handles and flags for the window.
        HINSTANCE hAppInst = 0;
        HWND      hMainWnd = 0;
        bool      appPaused = false;
        bool      minimized = false;
        bool      maximized = false;
        bool      resizing = false;
        std::string mainWndCaption;
        int clientWidth = 0;
        int clientHeight = 0;
    };

    WinData mWinData; // Instance of WinData to store window information.
    MyD3D* mpMyD3D;   // Pointer to MyD3D instance.

    // RealDefaultMssgHandler: Internal method for handling window messages.
    LRESULT RealDefaultMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif