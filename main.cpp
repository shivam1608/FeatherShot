#include <windows.h>
#include <stdio.h>

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
  WNDCLASSW wc = {0};
  wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hInstance = hInstance;
  wc.lpszClassName = L"HiddenWindow";
  wc.lpfnWndProc = WindowProcedure;
  if(!RegisterClassW(&wc))
    return -1;

  HWND hWnd = CreateWindow("HiddenWindow", "Hidden", WS_OVERLAPPEDWINDOW, 100, 100, 500, 500, NULL, NULL, NULL, NULL);
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

  MSG msg = {0};

  while (GetMessage(&msg, NULL, NULL, NULL))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    
    switch(msg.message) {
      case WM_HOTKEY:
        MessageBox(NULL, "intense thinking intensifies", "HMMMMMMMMM", MB_OK);
        break;
    }
  }
  return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
  switch (msg) {
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProcW(hWnd, msg, wp, lp);
  }
}