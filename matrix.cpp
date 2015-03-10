#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <windowsx.h>
#include <string.h>

#include "res/resource.h"


#define MB(txt) (MessageBox(NULL, txt, "debug", MB_OK))

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
void CheckUserDefinedValues();
void CreateDestroyStreams();
void UpdateStreams();
void DisplayStreams(HWND hwnd);
int OnCtlColor(HWND, HDC);

char szWinName[] = "MatrixSS";


// the max possible size for all streams
#define MAX 10000
// konstantStringSize
#define kSS 256

char szDebug[kSS];

unsigned long MaxStream = 1000; // max number of streams to use
unsigned long BackTrace = 40; // number of characters behind to erase the trail
unsigned long Leading = 10; // minumum number of characters to display before erasing begins
unsigned long SpacePad = 30; // number of characters to randomly delete from +/- SPACEPAD
unsigned long SpeedDelay = 5; // What is the maximum number of cycles to randomly wait for

unsigned long r = 150;
unsigned long g = 255;
unsigned long b = 100;

unsigned long datatype;
unsigned long datasize;

static int startX[MAX];
static int startY[MAX];
static int streamSpeed[MAX];
static int streamOrigSpeed[MAX];
static bool streamStatus[MAX];
static unsigned long streamCount = 0;


// hfont to diplay on screen
HFONT hfont;
// store the old font handle which we restore when we end he paint session
HFONT hOldFont;

//used to calculate the offsets needed to draw the next character below or across in a new stream
static int textHeight = 12; // place holders recalculated in code
static int textWidth = 8; // place holders recalculated in code

static int screenWidth;
static int screenHeight;

// handle to registry to store user defined values
HKEY hRegKey;
unsigned long result;
HDC hdc;

static HBITMAP hbitmap;
static HBITMAP hbitmapOk, hbitmapOk1, hbitmapCancel, hbitmapCancel1;
static HINSTANCE ghInstance;

static TEXTMETRIC tm;

// number of charsacters used
#define COUNT 254
static char szBuffer[COUNT] = {
      1,   2,   3,   4,   5,   6,   7,   8,   9,
     10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
     20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
     30,  31,  32,  33,  34,  35,  36,  37,  38,  39,
     40,  41,  42,  43,  44,  45,  46,  47,  48,  49,
     50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
     60,  61,  62,  63,  64,  65,  66,  67,  68,  69,
     70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
     80,  81,  82,  83,  84,  85,  86,  87,  88,  89,
     90,  91,  92,  93,  94,  95,  96,  97,  98,  99,
    100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 127, 128, 129,
    130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
    140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
    150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169,
    170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
    180, 181, 182, 183, 184, 185, 186, 187, 188, 189,
    190, 191, 192, 193, 194, 195, 196, 197, 198, 199,
    200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
    210, 211, 212, 213, 214, 215, 216, 217, 218, 219,
    220, 221, 222, 223, 224, 225, 226, 227, 228, 229,
    230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249,
    250, 251, 252, 253, 254
};


// We have 3 types of inputs command line arguments
// - /c:dddd  configuration dialog
// - /p dddd  preview, when we first switch to the window properties screen savers dialog
// - /s       starts the screensaver
// -          no command line starts the config dialog
int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR lpszArgs, int nWinMode)
{
    HWND hwnd;
    MSG msg;
    WNDCLASSEX wc;

    ghInstance = hThisInst;

    // set the values
    RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\louai\\Screensaver\\MatrixCode",
                    0, "MatrixSS", 0, KEY_ALL_ACCESS, NULL, &hRegKey, &result);

    // if key was created
    if (result == REG_CREATED_NEW_KEY) {
        RegSetValueEx(hRegKey, "MaxStream", 0, REG_DWORD, (LPBYTE) &MaxStream, sizeof(DWORD));
        RegSetValueEx(hRegKey, "BackTrace", 0, REG_DWORD, (LPBYTE) &BackTrace, sizeof(DWORD));
        RegSetValueEx(hRegKey, "Leading", 0, REG_DWORD, (LPBYTE) &Leading, sizeof(DWORD));
        RegSetValueEx(hRegKey, "SpacePad", 0, REG_DWORD, (LPBYTE) &SpacePad, sizeof(DWORD));
        RegSetValueEx(hRegKey, "SpeedDelay", 0, REG_DWORD, (LPBYTE) &SpeedDelay, sizeof(DWORD));
        RegSetValueEx(hRegKey, "Red", 0, REG_DWORD, (LPBYTE) &r, sizeof(DWORD));
        RegSetValueEx(hRegKey, "Green", 0, REG_DWORD, (LPBYTE) &g, sizeof(DWORD));
        RegSetValueEx(hRegKey, "Blue", 0, REG_DWORD, (LPBYTE) &b, sizeof(DWORD));
    } else {
        datasize = sizeof(DWORD);
        RegQueryValueEx(hRegKey, "MaxStream", NULL, &datatype, (LPBYTE) &MaxStream, &datasize);
        RegQueryValueEx(hRegKey, "BackTrace", NULL, &datatype, (LPBYTE) &BackTrace, &datasize);
        RegQueryValueEx(hRegKey, "Leading", NULL, &datatype, (LPBYTE) &Leading, &datasize);
        RegQueryValueEx(hRegKey, "SpacePad", NULL, &datatype, (LPBYTE) &SpacePad, &datasize);
        RegQueryValueEx(hRegKey, "SpeedDelay", NULL, &datatype, (LPBYTE) &SpeedDelay, &datasize);
        // version 1.1 did not have red green and blue keys
        if (RegQueryValueEx(hRegKey, "Red", NULL, &datatype, (LPBYTE) &r, &datasize))
            RegSetValueEx(hRegKey, "Red", 0, REG_DWORD, (LPBYTE) &r, sizeof(DWORD));
        if (RegQueryValueEx(hRegKey, "Green", NULL, &datatype, (LPBYTE) &g, &datasize))
            RegSetValueEx(hRegKey, "Green", 0, REG_DWORD, (LPBYTE) &g, sizeof(DWORD));
        if (RegQueryValueEx(hRegKey, "Blue", NULL, &datatype, (LPBYTE) &b, &datasize))
            RegSetValueEx(hRegKey, "Blue", 0, REG_DWORD, (LPBYTE) &b, sizeof(DWORD));
    }

    // check to make sure values are within reason
    CheckUserDefinedValues();

    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // _snprintf(szDebug, 256, "CommandLine:> %s", lpszArgs);
    // MB(szDebug);

    // I need this because windows continously calls the screen saver to start the program
    // in no time at all I would have 2000 instances of this code *shiver*
    if (!(FindWindow(szWinName, NULL) == NULL))
        return 0;

    if (_strnicmp("/s", lpszArgs, 2) == 0) {
        goto startApp; // ignore, we will start normally
    } else if (_strnicmp("/c", lpszArgs, 2) == 0) {
        // note to self, add a dialog here to configure the screensaver, hook to the registry
        DialogBox(hThisInst, MAKEINTRESOURCE(IDD_DIALOG_MATRIX), NULL, (DLGPROC) DialogProc);
        return 0;
    } else  if (strlen(lpszArgs) > 0) {
        return 0; // exit
    }

    DialogBox(hThisInst, MAKEINTRESOURCE(IDD_DIALOG_MATRIX), NULL, (DLGPROC) DialogProc);
    return 0;

startApp:

    ShowCursor(false);
    // Define a window class
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hInstance = hThisInst;
    wc.lpszClassName = szWinName;
    wc.lpfnWndProc = WindowProc;
    wc.style = 0;

    wc.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON_MATRIX));
    wc.hIconSm = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON_MATRIX));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    wc.lpszMenuName = NULL;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;

    wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);

    if (!RegisterClassEx(&wc)) return 0;

    hwnd = CreateWindow(szWinName, "Matrix Code", WS_POPUP, 0, 0, screenWidth,
                        screenHeight, HWND_DESKTOP, NULL, hThisInst, NULL);
    ShowWindow(hwnd,nWinMode);
    UpdateWindow(hwnd);

    // _snprintf(szDebug, kSS, "MaxStream: %d\nBackTrace: %d\nLeading: %d\nSpacePad: %d\nSpeedDelay %d", MaxStream, BackTrace, Leading, SpacePad, SpeedDelay);
    // MB(szDebug);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int mousecounter = 0;
    static int mouseDelay = 2; // how many clicks/keypresses to wait before killing app

    switch(message) {
        case WM_CREATE:
            hfont = (HFONT)GetStockObject(OEM_FIXED_FONT);
            /*
            hfont = CreateFont(12, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                               CLIP_DEFAULT_PRECIS, DRAFT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "System");
            */
            SetTimer(hwnd, 309, 50, NULL);
            break;

        case WM_TIMER:
            CreateDestroyStreams();
            UpdateStreams();
            DisplayStreams(hwnd);
            break;

        case WM_KEYDOWN:
        case WM_MOUSEMOVE:
            mousecounter++;
            if (mousecounter > mouseDelay) SendMessage(hwnd, WM_CLOSE, 7777, 0);
            break;

        case WM_CLOSE:
            // windows has this annoying habit of randomly closing the screen saver
            // thus if the WM_CLOSE is not sent by me I will ignore it
            // if windows sends a WM_DESTROY I let it pass since this is a more extreme form
            // of closure
            if (wParam != 7777) return 0;
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
            ShowCursor(true);
            DeleteObject(hfont);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}


BOOL CALLBACK DialogProc(HWND hdlg, UINT imsg, WPARAM wparam, LPARAM lparam)
{
    int w, h;
    RECT rect;
    int cx, cy, x, y;
    int xpos, ypos;
    int fwkeys;
    HDC hmemdc;
    PAINTSTRUCT ps;

    // state 1=ok sel, 2=cancel sel, 3=ok unsel, 4=cancel unsel
    static int buttonState = 0;

    switch (imsg) {
        case WM_INITDIALOG:
            w = GetSystemMetrics(SM_CXSCREEN);
            h = GetSystemMetrics(SM_CYSCREEN);
            GetWindowRect(hdlg, &rect);
            cx = rect.right - rect.left;
            cy = rect.bottom - rect.top;
            x = w / 2 - cx / 2;
            y = h / 2 - cy / 2;

            SetWindowPos(hdlg, HWND_NOTOPMOST, x, y, cx, cy, SWP_SHOWWINDOW);
            // center the dialog box

            // _snprintf(szDebug, kSS, "x, y, x1, y1 %d, %d, %d, %d", rect.left, rect.top, rect.right, rect.bottom);
            // MB(szDebug);

            _snprintf(szDebug, kSS, "%d", MaxStream);
            Edit_SetText(GetDlgItem(hdlg, IDC_EDIT_MAXSTREAMS), szDebug);

            _snprintf(szDebug, kSS, "%d", BackTrace);
            Edit_SetText(GetDlgItem(hdlg, IDC_EDIT_BACKTRACE), szDebug);

            _snprintf(szDebug, kSS, "%d", Leading);
            Edit_SetText(GetDlgItem(hdlg, IDC_EDIT_LEADING), szDebug);

            _snprintf(szDebug, kSS, "%d", SpacePad);
            Edit_SetText(GetDlgItem(hdlg, IDC_EDIT_SPACEPAD), szDebug);

            _snprintf(szDebug, kSS, "%d", SpeedDelay);
            Edit_SetText(GetDlgItem(hdlg, IDC_EDIT_SPEEDDELAY), szDebug);

            _snprintf(szDebug, kSS, "%d", r);
            Edit_SetText(GetDlgItem(hdlg, IDC_EDIT_RED), szDebug);

            _snprintf(szDebug, kSS, "%d", g);
            Edit_SetText(GetDlgItem(hdlg, IDC_EDIT_GREEN), szDebug);

            _snprintf(szDebug, kSS, "%d", b);
            Edit_SetText(GetDlgItem(hdlg, IDC_EDIT_BLUE), szDebug);

            hbitmap = LoadBitmap(ghInstance, MAKEINTRESOURCE(IDB_BITMAP_TEST));
            hbitmapOk = LoadBitmap(ghInstance, MAKEINTRESOURCE(IDB_BITMAP_TEST_OK));
            hbitmapOk1 = LoadBitmap(ghInstance, MAKEINTRESOURCE(IDB_BITMAP_TEST_OK1));
            hbitmapCancel = LoadBitmap(ghInstance, MAKEINTRESOURCE(IDB_BITMAP_TEST_CANCEL));
            hbitmapCancel1 = LoadBitmap(ghInstance, MAKEINTRESOURCE(IDB_BITMAP_TEST_CANCEL1));

            return TRUE;

        case WM_CTLCOLORDLG:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORSTATIC:
            return OnCtlColor((HWND)lparam, (HDC) wparam);

        case WM_PAINT:
            hdc = BeginPaint(hdlg, &ps);
            x = ps.rcPaint.left;
            y = ps.rcPaint.top;
            cx = ps.rcPaint.right - ps.rcPaint.left;
            cy = ps.rcPaint.bottom - ps.rcPaint.top;
            if (hmemdc = CreateCompatibleDC(hdc))
                if (SelectObject(hmemdc, hbitmap))
                    BitBlt(hdc, x, y, cx, cy, hmemdc, x, y, SRCCOPY);

            DeleteDC(hmemdc);
            EndPaint(hdlg,&ps);
            break;

        case WM_APP: // paint the buttons
            hdc = GetDC(hdlg);
            hmemdc = CreateCompatibleDC(hdc);

            if (buttonState == 1) {
                if (SelectObject(hmemdc, hbitmapOk))
                    BitBlt(hdc, 12, 186, 57, 32, hmemdc, 0, 0, SRCCOPY);
            } else if (buttonState == 2) {
                if (SelectObject(hmemdc, hbitmapCancel))
                    BitBlt(hdc, 71, 186, 114, 32, hmemdc, 1, 0, SRCCOPY);
            } else if (buttonState == 3) {
                if (SelectObject(hmemdc, hbitmapOk1))
                    BitBlt(hdc, 12, 186, 57, 32, hmemdc, 0, 0, SRCCOPY);
                buttonState = 0;
            } else if (buttonState == 4) {
                if (SelectObject(hmemdc, hbitmapCancel1))
                    BitBlt(hdc, 71, 186, 114, 32, hmemdc, 1, 0, SRCCOPY);
                buttonState = 0;
            }
            DeleteDC(hmemdc);
            ReleaseDC(hdlg,hdc);
            break;

        case WM_MOUSEMOVE:
            xpos = LOWORD(lparam);
            ypos = HIWORD(lparam);
            fwkeys = wparam;

            if (buttonState == 1) {
                if (!((xpos >= 12) && (xpos <= 70) && (ypos >= 186) && (ypos <= 218))) {
                    buttonState = 3;
                    SendMessage(hdlg, WM_APP, 0, 0);
                }
            } else if (buttonState == 2) {
                if (!((xpos >= 70) && (xpos <= 184) && (ypos >= 186) && (ypos <= 218))) {
                    buttonState = 4;
                    SendMessage(hdlg, WM_APP, 0, 0);
                }
            }
            break;

        case WM_LBUTTONDOWN:
            xpos = LOWORD(lparam);
            ypos = HIWORD(lparam);
            fwkeys = wparam;

            if ((xpos >= 12) && (xpos <= 70) && (ypos >= 186) && (ypos <= 218)) {
                buttonState = 1;
                SendMessage(hdlg, WM_APP, 0, 0);
            } else if ((xpos >= 70) && (xpos <= 184) && (ypos >= 186) && (ypos <= 218)) {
                buttonState = 2;
                SendMessage(hdlg, WM_APP, 0, 0);
            } else
                buttonState = 0;
            break;

        case WM_LBUTTONUP:
            if (buttonState == 1) {
                SendMessage(hdlg, WM_COMMAND, IDC_BUTTON_OK, 0);
            }
            if (buttonState == 2) {
                SendMessage(hdlg, WM_COMMAND, IDC_BUTTON_CANCEL, 0);
            }
            break;

        case WM_COMMAND:
            switch (LOWORD(wparam)) {
                case IDC_BUTTON_OK:

                    // save the values to the registry
                    MaxStream = GetDlgItemInt(hdlg, IDC_EDIT_MAXSTREAMS, NULL, false);
                    BackTrace = GetDlgItemInt(hdlg, IDC_EDIT_BACKTRACE, NULL, false);
                    Leading = GetDlgItemInt(hdlg, IDC_EDIT_LEADING, NULL, false);
                    SpacePad = GetDlgItemInt(hdlg, IDC_EDIT_SPACEPAD, NULL, false);
                    SpeedDelay = GetDlgItemInt(hdlg, IDC_EDIT_SPEEDDELAY, NULL, false);

                    r = GetDlgItemInt(hdlg, IDC_EDIT_RED, NULL, false);
                    g = GetDlgItemInt(hdlg, IDC_EDIT_GREEN, NULL, false);
                    b = GetDlgItemInt(hdlg, IDC_EDIT_BLUE, NULL, false);

                    CheckUserDefinedValues();

                    RegSetValueEx(hRegKey, "MaxStream", 0, REG_DWORD, (LPBYTE) &MaxStream, sizeof(DWORD));
                    RegSetValueEx(hRegKey, "BackTrace", 0, REG_DWORD, (LPBYTE) &BackTrace, sizeof(DWORD));
                    RegSetValueEx(hRegKey, "Leading", 0, REG_DWORD, (LPBYTE) &Leading, sizeof(DWORD));
                    RegSetValueEx(hRegKey, "SpacePad", 0, REG_DWORD, (LPBYTE) &SpacePad, sizeof(DWORD));
                    RegSetValueEx(hRegKey, "SpeedDelay", 0, REG_DWORD, (LPBYTE) &SpeedDelay, sizeof(DWORD));
                    RegSetValueEx(hRegKey, "Red", 0, REG_DWORD, (LPBYTE) &r, sizeof(DWORD));
                    RegSetValueEx(hRegKey, "Green", 0, REG_DWORD, (LPBYTE) &g, sizeof(DWORD));
                    RegSetValueEx(hRegKey, "Blue", 0, REG_DWORD, (LPBYTE) &b, sizeof(DWORD));

                    SendMessage(hdlg, WM_CLOSE, 7777, 0);
                    return TRUE;

                case IDC_BUTTON_CANCEL:
                    SendMessage(hdlg, WM_CLOSE, 7777, 0);
                    return TRUE;
            }
            break;

        case WM_CLOSE:
            // windows has this annoying habit of randomly closing the screen saver
            // thus if the WM_CLOSE is not sent by me I will ignore it
            // if windows sends a WM_DESTROY I let it pass since this is a more extreme form
            // of closure
            if (wparam != 7777) return FALSE;
            EndDialog(hdlg, 0);
            DeleteObject(hbitmap);
            DeleteObject(hbitmapOk);
            DeleteObject(hbitmapCancel);
            return TRUE;
    }

    return FALSE;
}


// global settings for the screen saver
void CheckUserDefinedValues()
{
    if (MaxStream > MAX) MaxStream = MAX;
    if (MaxStream < 1) MaxStream = 1;
    if (BackTrace < (Leading + SpacePad)) BackTrace = (Leading + SpacePad);
    if (SpeedDelay > 10) SpeedDelay = 10;
    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;
    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;
}


void CreateDestroyStreams()
{
    if (streamCount < MaxStream) {
        for (int i = 0; i < MaxStream; i++) {
            if (streamStatus[i] == false) {
                streamCount++;
                streamStatus[i] = true;
                startY[i] = 0;
                startX[i] = rand() * (screenWidth / textWidth) / 32767 * textWidth;
                streamOrigSpeed[i] = rand() * SpeedDelay / 32767;
                streamSpeed[i] = streamOrigSpeed[i];
                break;
            }
        }
    }

    for (int i = 0; i < MaxStream; i++) {
        if (startY[i] > (screenHeight + BackTrace * textHeight)) {
            streamStatus[i] = false;
            streamCount--;
            startY[i] = 0;
        }
    }
}


void UpdateStreams()
{
    for (int i = 0; i < MaxStream; i++) {
        if (streamStatus[i]) {
            if (streamSpeed[i] == 0) {
                startY[i] += textHeight;
                streamSpeed[i] = streamOrigSpeed[i];
            } else {
                streamSpeed[i]--;
            }
        }
    }
}


void DisplayStreams(HWND hwnd)
{
    int x;
    int y;

    hdc = GetDC(hwnd);

    if (hOldFont = (HFONT) SelectObject(hdc, hfont)) {
        GetTextMetrics(hdc, &tm);
        textHeight = tm.tmHeight;
        textWidth = tm.tmMaxCharWidth;

        SetBkColor(hdc, RGB(0, 0, 0));
        SetTextColor(hdc, RGB(255, 255, 255));

        for (UINT i = 0; i < MaxStream; i++) {
            if (streamStatus[i]) {
                x = startX[i];
                y = startY[i];

                for (int j = 0; j < 3; j++) {
                    if (j==0) {
                        int incR = r / (SpeedDelay + 1);
                        int incG = g / (SpeedDelay + 1);
                        int incB = b / (SpeedDelay + 1);

                        SetTextColor(hdc, RGB(
                                              r - streamOrigSpeed[i] * incR,
                                              g - streamOrigSpeed[i] * incG,
                                              b - streamOrigSpeed[i] * incB)
                        );
                        TextOut(hdc, x , y - (j * textHeight), (szBuffer + (rand() * COUNT / 32767)), 1);
                    } else if (j==1) {
                        int incR = r / 3 / (SpeedDelay + 1);
                        int incG = g / 3 / (SpeedDelay + 1);
                        int incB = b / 3 / (SpeedDelay + 1);

                        SetTextColor(hdc, RGB(
                                              r / 3 - streamOrigSpeed[i] * incR,
                                              g / 3 - streamOrigSpeed[i] * incG,
                                              b / 3 - streamOrigSpeed[i] * incB)
                        );
                        TextOut(hdc, x, y - (j * textHeight), (szBuffer + (rand() * COUNT / 32767)), 1);
                    } else {
                        SetTextColor(hdc,RGB(0,0,0)); // erase
                        TextOut(hdc, x, y - (rand() * SpacePad / 32767 + Leading) * textHeight, "W", 1);
                        TextOut(hdc, x, y - (BackTrace) * textHeight, "W", 1);
                    }
                }
            }
        }

        SelectObject(hdc, hOldFont);
    }
    ReleaseDC(hwnd, hdc);
}


int OnCtlColor(HWND hDlg, HDC hDC)
{
    SetTextColor (hDC, RGB(255, 255, 255));
    SetBkColor (hDC, RGB(0, 0, 0));
    return ((int) GetStockObject(BLACK_BRUSH));
}
