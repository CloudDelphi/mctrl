/*
 * Copyright (c) 2008-2012 Martin Mitas
 *
 * This file contains example code for mCtrl library. Code of this example
 * (but not the library itself) has been placed in the public domain.
 */

/* This sample demonstrates basic usage of MDITAB control. */

#include <windows.h>
#include <commctrl.h>  /* image list */
#include <tchar.h>
#include <stdio.h>

#include <mCtrl/mditab.h>
#include "ex_mditab.h"


static HINSTANCE hInst;
static HIMAGELIST hImgList;  /* Few cute images for the tabs */

static HWND hwndMdiTab;      /* The MDITAB control */
static HWND hwndBtn;         /* A button for creating new tabs */


#define MINIMAL_WIDTH      500   /* Minimal (and also default) window size */
#define MINIMAL_HEIGHT     300


/* Adds new tab */
static void
AddNewTab(void)
{
    static UINT uCounter = 0;
    int i, n;
    TCHAR buffer[32];
    MC_MTITEM item = {MC_MTIF_TEXT | MC_MTIF_IMAGE, buffer, 0, 0, 0};

    /* Setup tab icon and label */
    uCounter++;
    item.iImage = uCounter % 11; /* we have 11 icons in the image list */
    _sntprintf(buffer, 32, _T("Tab %u"), uCounter);

    /* Add the new tab as last tab */
    n = SendMessage(hwndMdiTab, MC_MTM_GETITEMCOUNT, 0, 0);
    i = SendMessage(hwndMdiTab, MC_MTM_INSERTITEM, (WPARAM) n, (LPARAM) &item);

    /* Activate the new tab */
    SendMessage(hwndMdiTab, MC_MTM_SETCURSEL, (WPARAM) i, 0);
}


/* Main window procedure */
static LRESULT CALLBACK
WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg) {
        case WM_COMMAND:
            /* Handle clicks to the button: create new tab. */
            if(LOWORD(wParam) == IDC_BUTTON_NEW) {
                AddNewTab();
            }
            break;

        case WM_SIZE:
            if(wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED) {
                WORD width = LOWORD(lParam);
                WORD height = HIWORD(lParam);

                SetWindowPos(hwndMdiTab, NULL, 0, 0, width, 30, SWP_NOZORDER);
                SetWindowPos(hwndBtn, NULL, 10, height - 34, 80, 24, SWP_NOZORDER);
            }
            break;

        case WM_GETMINMAXINFO:
        {
            MINMAXINFO* lpMmi = (MINMAXINFO*) lParam;
            lpMmi->ptMinTrackSize.x = MINIMAL_WIDTH;
            lpMmi->ptMinTrackSize.y = MINIMAL_HEIGHT;
            return 0;
        }

        case WM_CREATE:
        {
            MC_MTITEMWIDTH tw;

            /* Create mditab child window  */
            hwndMdiTab = CreateWindow(MC_WC_MDITAB, _T(""),
                WS_CHILD | WS_VISIBLE | MC_MTS_CLOSEONMCLICK,
                0, 0, 0, 0, hWnd, (HMENU) IDC_MDITAB, hInst, NULL);

            /* Set an imagelist */
            SendMessage(hwndMdiTab, MC_MTM_SETIMAGELIST, 0, (LPARAM) hImgList);

            /* Change minimal tab width */
            SendMessage(hwndMdiTab, MC_MTM_GETITEMWIDTH, 0, (LPARAM) &tw);
            tw.dwMinWidth += 30;
            SendMessage(hwndMdiTab, MC_MTM_SETITEMWIDTH, 0, (LPARAM) &tw);

            /* Create button for creating new tabs */
            hwndBtn = CreateWindow(_T("BUTTON"), _T("New tab"),
                WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
                hWnd, (HMENU) IDC_BUTTON_NEW, hInst, NULL);
            return 0;
        }

        case WM_SETFONT:
            SendMessage(hwndMdiTab, WM_SETFONT, wParam, lParam);
            SendMessage(hwndBtn, WM_SETFONT, wParam, lParam);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


int APIENTRY
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc = { 0 };
    HWND hWnd;
    MSG msg;

    mcMditab_Initialize();

    /* Prevent linker from ignoring comctl32.dll */
    InitCommonControls();

    hInst = hInstance;

    /* Register main window class */
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = _T("main_window");
    RegisterClass(&wc);

    /* Load image list */
    hImgList = ImageList_LoadImage(hInst, MAKEINTRESOURCE(IDL_IMGLIST),
                    16, 1, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);

    /* Create main window */
    hWnd = CreateWindow(_T("main_window"), _T("mCtrl Example: MDITAB Control"),
            WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU,
            CW_USEDEFAULT, CW_USEDEFAULT, MINIMAL_WIDTH, MINIMAL_HEIGHT,
            NULL, NULL, hInst, NULL);
    SendMessage(hWnd, WM_SETFONT, (WPARAM) GetStockObject(DEFAULT_GUI_FONT),
            MAKELPARAM(TRUE, 0));
    ShowWindow(hWnd, nCmdShow);

    /* Message loop */
    while(GetMessage(&msg, NULL, 0, 0)) {
        if(IsDialogMessage(hWnd, &msg))
            continue;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    mcMditab_Terminate();

    /* Return exit code of WM_QUIT */
    return (int)msg.wParam;
}
