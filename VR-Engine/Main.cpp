//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************



#include "stdafx.h"
#include "Input.h"

int Input::keys[INPUT_SIZE];
int Input::keysDown[INPUT_SIZE];
int Input::mouse[MOUSE_SIZE];
int Input::mouseDown[MOUSE_SIZE];
int Input::mouseWheel;
bool Input::gLoopMouse;
b32 Input::gCursorHidden;
ST_Vector2Integer Input::gDeltaMousePosition;
ST_Vector2Integer Input::gMouseDownPosition;
ST_Vector2Integer Input::gMousePosition;
ST_IndexList Input::gKeysDownList;

void inputInitialize()
{
    memset(Input::keys, FALSE, INPUT_SIZE * sizeof(int));
    memset(Input::keysDown, FALSE, INPUT_SIZE * sizeof(int));
    memset(Input::mouse, FALSE, MOUSE_SIZE * sizeof(int));
    memset(Input::mouseDown, FALSE, MOUSE_SIZE * sizeof(int));
    Input::gKeysDownList = sphereTraceIndexListConstruct();
    Input::mouseWheel = 0;
}

void inputContinuousReset()
{
    memset(Input::keys, FALSE, INPUT_SIZE * sizeof(int));
    memset(Input::mouse, FALSE, MOUSE_SIZE * sizeof(int));
    memset(Input::keysDown, FALSE, INPUT_SIZE * sizeof(int));
    memset(Input::mouseDown, FALSE, MOUSE_SIZE * sizeof(int));
    sphereTraceIndexListFree(&Input::gKeysDownList);
    Input::mouseWheel = 0;
}

void inputReset()
{
    memset(Input::keysDown, FALSE, INPUT_SIZE * sizeof(int));
    memset(Input::mouseDown, FALSE, MOUSE_SIZE * sizeof(int));
    sphereTraceIndexListFree(&(Input::gKeysDownList));
    Input::mouseWheel = 0;
}

void windowUpdateCursor(HWND hwnd)
{

    POINT p;
    if (GetCursorPos(&p))
    {
        ScreenToClient(hwnd, &p);
        Input::gMousePosition = { (int)p.x, (int)p.y };
    }

    //get cursor position
    if (Input::mouseDown[MOUSE_RIGHT])
    {

        Input::gMouseDownPosition = Input::gMousePosition;
        if (!Input::gCursorHidden)
        {
            ShowCursor(FALSE);
            Input::gCursorHidden = 1;
        }
    }
    else if (Input::mouse[MOUSE_RIGHT])
    {
        p = { (int)Input::gMouseDownPosition.x, (int)Input::gMouseDownPosition.y };
        Input::gDeltaMousePosition = sphereTraceVector2IntegerSubtract(Input::gMousePosition, Input::gMouseDownPosition);
        ClientToScreen(hwnd, &p);
        SetCursorPos(p.x, p.y);
    }
    else if (Input::gCursorHidden)
    {
        ShowCursor(TRUE);
        Input::gCursorHidden = 0;
    }
}



#include "DXSample.h"
#include "Renderer.h"

HWND hwnd;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    //Renderer sample(1280, 720, L"D3D12");
    // Parse the command line parameters
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    Renderer::instance.ParseCommandLineArgs(argv, argc);
    LocalFree(argv);

    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

    // Initialize the window class.
    WNDCLASSEX windowClass = { 0 };
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = L"DXSampleClass";
    RegisterClassEx(&windowClass);

    RECT windowRect = { 0, 0, static_cast<LONG>(Renderer::instance.GetWidth()), static_cast<LONG>(Renderer::instance.GetHeight()) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    // Create the window and store a handle to it.
    hwnd = CreateWindow(
        windowClass.lpszClassName,
        Renderer::instance.GetTitle(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,        // We have no parent window.
        nullptr,        // We aren't using menus.
        hInstance,
        nullptr);

    //Initialize spheretrace allocator
    sphereTraceAllocatorInitialize();

    // Initialize the sample. OnInit is defined in each child-implementation of DXSample.
    Renderer::instance.OnInit();
    inputInitialize();

    ShowWindow(hwnd, nCmdShow);

    // Main sample loop.
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {

        inputReset();

        // Process any messages in the queue.
        while (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        windowUpdateCursor(hwnd);

        Renderer::instance.OnUpdate();
        Renderer::instance.OnRender();
    }

    Renderer::instance.OnDestroy();

    // Return this part of the WM_QUIT message to Windows.
    return static_cast<char>(msg.wParam);
}


// Main message handler for the sample.
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //DXSample* pSample = reinterpret_cast<DXSample*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    //case WM_CREATE:
    //{
    //    // Save the DXSample* passed in to CreateWindow.
    //    LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
    //    SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
    //}
    //return 0;

    case WM_KEYDOWN:
    {
        if (!Input::keys[wParam])
        {
            Input::keysDown[wParam] = TRUE;
            sphereTraceIndexListAddFirst(&Input::gKeysDownList, wParam);
        }
        Input::keys[wParam] = TRUE;
        //if (wparam == VK_ESCAPE)
        //	DestroyWindow(hwnd);
        break;
    }
    case WM_KEYUP:
    {
        Input::keys[wParam] = FALSE;
        break;
    }
    case WM_LBUTTONDOWN:
    {
        if (!Input::mouse[MOUSE_LEFT])
            Input::mouseDown[MOUSE_LEFT] = TRUE;
        Input::mouse[MOUSE_LEFT] = TRUE;
        break;
    }
    case WM_LBUTTONUP:
    {
        Input::mouse[MOUSE_LEFT] = FALSE;
        break;
    }
    case WM_RBUTTONDOWN:
    {
        if (!Input::mouse[MOUSE_RIGHT])
            Input::mouseDown[MOUSE_RIGHT] = TRUE;
        Input::mouse[MOUSE_RIGHT] = TRUE;
        break;
    }
    case WM_RBUTTONUP:
    {
        Input::mouse[MOUSE_RIGHT] = FALSE;
        break;
    }
    case WM_MBUTTONDOWN:
    {
        if (!Input::mouse[MOUSE_MIDDLE])
            Input::mouseDown[MOUSE_MIDDLE] = TRUE;
        Input::mouse[MOUSE_MIDDLE] = TRUE;
        break;
    }
    case WM_MBUTTONUP:
    {
        Input::mouse[MOUSE_MIDDLE] = FALSE;
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    // Handle any messages the switch statement didn't.
    return DefWindowProc(hWnd, message, wParam, lParam);
}