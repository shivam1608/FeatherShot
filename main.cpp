/* 
 Shivzee : using win32 for first time 
 Working Logic : (yea ik its weird but works)
 Application Takes a Screenshot of full Screen then we display full Screenshot
 and give user to choose coordinates of the area he wants to capture
 then we take a screenshot of that area and copy it to clipboard
*/

#include <stdio.h>
#include <iostream>
#include <tchar.h>

/*
  Including Win32 API Headers
  https://learn.microsoft.com/en-us/windows/win32/api/
*/
#include <wingdi.h>
#include <windows.h>
#include <winuser.h>

using namespace std;


/*
  Default Variables
*/
HBITMAP iScreen;
bool open = false;

/*
  Function to Capture Screenshot of Rectange Drawn
  and copy it to clipboard
  ....... x1 ___________________ y1 .......
  .......... ___________________ ..........
  .......... ____Screenshot_____ ..........
  .......... ___________________ ..........
  ........y2 ___________________ x2........
*/
void GetScreenShot(int x1 , int x2 , int y1 , int y2 , bool save){
  int width,heigth;
  width = x2 - x1;  // Calculating Width
  heigth = y2 - y1; // Calculating Heigth

  // Creating a Device Context
  HDC hScreen = GetDC(NULL);    
  HDC hDc = CreateCompatibleDC(hScreen);                    // Creating new Screen Device Context
  iScreen = CreateCompatibleBitmap(hScreen, width, heigth); // Creating new HBitmap Object
  HGDIOBJ old_obj = SelectObject(hDc, iScreen);
  BOOL bRet = BitBlt(hDc, 0, 0, width, heigth, hScreen, x1, y1, SRCCOPY); // Getting Rectangle Screenshot 

  // Copying Screenshot to Clipboard
  OpenClipboard(NULL);
  EmptyClipboard();
  SetClipboardData(CF_BITMAP, iScreen);
  CloseClipboard();

  // Cleaning up memory
  SelectObject(hDc, old_obj);
  DeleteDC(hDc);
  ReleaseDC(NULL, hScreen);
  DeleteObject(iScreen);

}

/* Function to Capture Screenshot of Full Screen */

void GetScreenShot(){
  int x1 , x2 , y1 , y2;
  // Determine System Screen Size
  x1 = GetSystemMetrics(SM_XVIRTUALSCREEN);
  x2 = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  y1 = GetSystemMetrics(SM_YVIRTUALSCREEN);
  y2 = GetSystemMetrics(SM_CYVIRTUALSCREEN);
  GetScreenShot(x1,x2,y1,y2,false);
}

// Declaring Windows Procedure Callback TODO -> Shift it to header file
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/* Cpp utility for Array Long Strings */
TCHAR szClassName[] = _T("FeatherShot");

/*Main Function according to windows.h (idk im just guessing)*/
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow){
  
  HWND hwnd; // Creating a Window Handle
  MSG messages; // Creating a Message Handle
  WNDCLASSEX wc; // Creating a Window Class

  // IDK WTF are these
  wc.hInstance = hInstance;
  wc.lpszClassName = szClassName;
  wc.lpfnWndProc = WindowProcedure;
  wc.style = CS_DBLCLKS;
  wc.cbSize = sizeof(WNDCLASSEX);

  // Window Manager Styles
  wc.hIcon = (HICON)LoadImage(hInstance, "icon.ico", IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_LOADFROMFILE);
  wc.hIconSm = (HICON)LoadImage(hInstance, "icon.ico", IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_LOADFROMFILE);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
  wc.lpszMenuName = NULL; // Disabling Menu on Window
  wc.cbClsExtra = 0; // No Extra Window Data
  wc.cbWndExtra = 0; // Something extra idk
  
  if(!RegisterClassEx(&wc))
    return 0;


  // Creating Visiable Window
  hwnd = CreateWindowEx(0 , szClassName , _T("Capture") , WS_OVERLAPPEDWINDOW , CW_USEDEFAULT , CW_USEDEFAULT , 500 , 500 , HWND_DESKTOP , NULL , hInstance , NULL);
  ShowWindow(hwnd , nCmdShow);


  // Creating Hidden Window 
  HWND hWnd = CreateWindowEx( 0 , szClassName , _T("Hidden") , WS_OVERLAPPEDWINDOW, 100, 100, 500, 500, NULL, NULL, NULL, NULL);
  RegisterHotKey(NULL, 1, MOD_ALT, VK_F2);
  
  NOTIFYICONDATA niData;
  ZeroMemory(&niData,sizeof(NOTIFYICONDATA));
  niData.cbSize = sizeof(NOTIFYICONDATA);

  niData.uID = 0;
  niData.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
  niData.hIcon = (HICON)LoadImage(hInstance, "icon.ico", IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_LOADFROMFILE);
  niData.hWnd = hWnd;
  niData.uCallbackMessage = 0x42069;
  if(!Shell_NotifyIconA(NIM_ADD, &niData))
    return 0;

  while (GetMessage(&messages, NULL, 0, 0))
  {
    TranslateMessage(&messages); // Virtual Key to Character
    DispatchMessage(&messages); // Sending Message to Window Procedure

    switch (messages.message)
    {
    case WM_HOTKEY:
     // Open Screenshot Window if ALT + F2 is pressed
      if(!open){
        GetScreenShot();
        ShowWindow(hwnd, SW_SHOW);
        open = true;
      }
      break;
    }
  }
  return messages.wParam;
}

/*
 Catching all message events dispatched by DispatchMessage()
*/
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
  
  /* Declaring Variables */
  HDC hdc;                 // device context (DC) for window
  RECT rcTmp;              // temporary rectangle
  PAINTSTRUCT ps;          // for BeginPaint() and EndPaint()
  POINT upperLeft;         // upper left corner
  POINT lowerRight;        // lower right corner
  static HDC hdcCompat;    // DC for copying bitmap
  static POINT point;      // x and y coordinates of cursor
  static RECT rcBmp;       // rectangle that encloses bitmap
  static RECT rcTarget;    // rectangle to receive bitmap
  static RECT rcClient;    // client-area rectangle
  static BOOL fDragRect;   // TRUE if bitmap rect. is dragged
  static HBITMAP hbmp;     // handle of bitmap to display
  static HBRUSH hbrBkgnd;  // handle of background-color brush
  static COLORREF crBkgnd; // color of client-area background
  static HPEN hpenDot;     // handle of dotted pen

  switch (msg) {
    case WM_CREATE:
      // TODO -> Shift this to WM_HOTKEY EVENT
      GetScreenShot();

      // Load the bitmap.
      hbmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(1));
      
      // This device context belong to the window
      hdc = GetDC(hWnd);
      hdcCompat = CreateCompatibleDC(hdc);
      SelectObject(hdcCompat, hbmp);
      crBkgnd = GetBkColor(hdc);
      hbrBkgnd = CreateSolidBrush(crBkgnd);
      ReleaseDC(hWnd, hdc);

      hpenDot = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
      SetRect(&rcBmp, 1, 1, 0, 0);
      return 0;
    case WM_PAINT:
      // Draws the sceenshot on the window
      PAINTSTRUCT ps;
      HDC hdc;
      BITMAP bitmap;
      HDC hdcMem;
      HGDIOBJ oldBitmap;

      hdc = BeginPaint(hWnd, &ps);

      hdcMem = CreateCompatibleDC(hdc);
      oldBitmap = SelectObject(hdcMem, iScreen);

      GetObject(iScreen, sizeof(bitmap), &bitmap);
      BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

      SelectObject(hdcMem, oldBitmap);
      DeleteDC(hdcMem);

      EndPaint(hWnd, &ps);
      break;
    
    case WM_MOVE:
    case WM_SIZE:
      // Stores Coordinates of Reactange when the Mouse Moves
      GetClientRect(hWnd, &rcClient);
      upperLeft.x = rcClient.left;
      upperLeft.y = rcClient.top;
      lowerRight.x = rcClient.right;
      lowerRight.y = rcClient.bottom;
      ClientToScreen(hWnd, &upperLeft);
      ClientToScreen(hWnd, &lowerRight);
      SetRect(&rcClient , upperLeft.x , upperLeft.y , lowerRight.x , lowerRight.y);
      
      return 0;
    case WM_LBUTTONDOWN:
      // Limits the cursor within the window
      ClipCursor(&rcClient);
      point.x = (LONG)LOWORD(lp);
      point.y = (LONG)HIWORD(lp);
      return 0;
    case WM_MOUSEMOVE:
      // Update the rectangle when the mouse moves
      /*
      TODO -> Tough Part Bruh
      */

      if ((wp && MK_LBUTTON) && !fDragRect)
      {
        hdc = GetDC(hWnd);
        SetROP2(hdc, R2_NOTXORPEN);

        if (!IsRectEmpty(&rcTarget))
        {
          Rectangle(hdc, rcTarget.left, rcTarget.top,
                    rcTarget.right, rcTarget.bottom);
        }

        if ((point.x < (LONG)LOWORD(lp)) &&
            (point.y > (LONG)HIWORD(lp)))
        {
          SetRect(&rcTarget, point.x, HIWORD(lp),
                  LOWORD(lp), point.y);
        }
        else if ((point.x > (LONG)LOWORD(lp)) &&
                 (point.y > (LONG)HIWORD(lp)))
        {
          SetRect(&rcTarget, LOWORD(lp),
                  HIWORD(lp), point.x, point.y);
        }
        else if ((point.x > (LONG)LOWORD(lp)) &&
                 (point.y < (LONG)HIWORD(lp)))
        {
          SetRect(&rcTarget, LOWORD(lp), point.y,
                  point.x, HIWORD(lp));
        }
        else
        {
          SetRect(&rcTarget, point.x, point.y, LOWORD(lp),
                  HIWORD(lp));
        }

        // Draw the new target rectangle.

        Rectangle(hdc, rcTarget.left, rcTarget.top,
                  rcTarget.right, rcTarget.bottom);
        ReleaseDC(hWnd, hdc);
      }
      else if ((wp && MK_LBUTTON) && fDragRect)
      {
        hdc = GetDC(hWnd);
        SetROP2(hdc, R2_NOTXORPEN);
        SelectObject(hdc, hpenDot);
        Rectangle(hdc, rcBmp.left, rcBmp.top,
                  rcBmp.right, rcBmp.bottom);

        // Set the new coordinates of the bitmap rectangle,
        // then redraw it.

        OffsetRect(&rcBmp, LOWORD(lp) - point.x,
                   HIWORD(lp) - point.y);
        Rectangle(hdc, rcBmp.left, rcBmp.top,
                  rcBmp.right, rcBmp.bottom);
        ReleaseDC(hWnd, hdc);

        // Save the coordinates of the mouse cursor.

        point.x = (LONG)LOWORD(lp);
        point.y = (LONG)HIWORD(lp);
      }

     return 0;
    case WM_LBUTTONUP:
      // Capture the screenshot of specified area

      // DEBUG TESTING
      cout << "Left:" << rcTarget.left << " Top:" << rcTarget.top << " Right:" << rcTarget.right << " Bottom:" << rcTarget.bottom << endl;
      // ...........................

      CopyRect(NULL , &rcTarget);
      GetScreenShot(rcTarget.left, rcTarget.right, rcTarget.top, rcTarget.bottom , true);
      ClipCursor((LPRECT)NULL);
      // Hiding the window after the screenshot is taken
      open = false;
      ShowWindow(hWnd, SW_HIDE);
      return 0;
    case WM_DESTROY:
      DeleteObject(hbmp);
      DeleteDC(hdcCompat);
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hWnd, msg, wp, lp);
  }
}