#include <windows.h>
#include <mmsystem.h>

#include "resource.h"

const wchar_t ClassName[] = L"IdiotC";

HBITMAP BlackBitmap;
HBITMAP WhiteBitmap;
BOOL IsWhite = TRUE;
int PosX = 100, PosY = 100, VelX = 15, VelY = 15;

LRESULT CALLBACK WindowProcedure(HWND Window, UINT Message, WPARAM Wide, LPARAM Long)
{
	switch (Message)
	{
		case WM_SIZE:
			InvalidateRect(Window, NULL, TRUE);

			return 0;

		case WM_PAINT:
		{
			PAINTSTRUCT PaintStruct;

			HDC WindowDC = BeginPaint(Window, &PaintStruct);
			HDC MemoryDC = CreateCompatibleDC(WindowDC);

			if (MemoryDC)
			{
				HBITMAP CurrentBitmap = IsWhite ? WhiteBitmap : BlackBitmap;
				HGDIOBJ OldBitmap = SelectObject(MemoryDC, CurrentBitmap);

				BITMAP BitmapInfo;
				GetObject(CurrentBitmap, sizeof(BITMAP), &BitmapInfo);

				RECT ClientRect;
				GetClientRect(Window, &ClientRect);

				int DestinationWidth = ClientRect.right - ClientRect.left;
				int DestinationHeight = ClientRect.bottom - ClientRect.top;

				SetStretchBltMode(WindowDC, HALFTONE);

				StretchBlt(
					WindowDC,
					0, 0, DestinationWidth, DestinationHeight,

					MemoryDC,
					0, 0, BitmapInfo.bmWidth, BitmapInfo.bmHeight,

					SRCCOPY
				);

				SelectObject(MemoryDC, OldBitmap);
				DeleteDC(MemoryDC);
			}

			EndPaint(Window, &PaintStruct);

			return 0;
		}

		case WM_CREATE:
			PlaySound(MAKEINTRESOURCE(IDR_YOUARE), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC | SND_LOOP);

			return 0;

		case WM_TIMER:
			switch(Wide)
			{
				case IDT_BITMAP_TOGGLE:
					IsWhite = !IsWhite;
					InvalidateRect(Window, NULL, TRUE);

					break;

				case IDT_BOUNCE:
					int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
        			int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

					RECT Rect;
					GetWindowRect(Window, &Rect);

					int WindowWidth = Rect.right - Rect.left;
        			int WindowHeight = Rect.bottom - Rect.top;

					PosX += VelX;
        			PosY += VelY;

					if (PosX <= 0 || (PosX + WindowWidth) >= ScreenWidth) VelX = -VelX;
					if (PosY <= 0 || (PosY + WindowHeight) >= ScreenHeight) VelY = -VelY;

					SetWindowPos(Window, NULL, PosX, PosY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

					break;
			}

			return 0;

		case WM_DESTROY:
			KillTimer(Window, IDT_BITMAP_TOGGLE);
			KillTimer(Window, IDT_BOUNCE);
			PostQuitMessage(0);

			return 0;

		default:
			return DefWindowProcW(Window, Message, Wide, Long);
	}
}

int main()
{
	HINSTANCE AppInstance = GetModuleHandleW(NULL);

	BlackBitmap = LoadBitmapW(AppInstance, MAKEINTRESOURCEW(IDB_BLACK));
	WhiteBitmap = LoadBitmapW(AppInstance, MAKEINTRESOURCEW(IDB_WHITE));

	if (!BlackBitmap || !WhiteBitmap)
		return -1;

	WNDCLASSW WindowClass = { 0 };
	WindowClass.lpfnWndProc = WindowProcedure;
	WindowClass.hInstance = AppInstance;
	WindowClass.lpszClassName = ClassName;
	WindowClass.hCursor = LoadCursorW(NULL, MAKEINTRESOURCEW(IDC_ARROW));
	WindowClass.hbrBackground = GetSysColorBrush(COLOR_WINDOW);

	if (!RegisterClassW(&WindowClass))
		return -1;

	HWND Window = CreateWindowExW(
		0,
		ClassName,
		L"idiotc",
		WS_OVERLAPPEDWINDOW,

		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,

		NULL,
		NULL,
		AppInstance,
		NULL
	);

	if (!Window)
		return -1;

	ShowWindow(Window, SW_SHOWDEFAULT);
	UpdateWindow(Window);

	RECT Rect;
	GetWindowRect(Window, &Rect);
	PosX = Rect.left, PosY = Rect.top;

	SetTimer(Window, IDT_BITMAP_TOGGLE, 333, NULL);
	SetTimer(Window, IDT_BOUNCE, 10, NULL);

	MSG Message;
	while (GetMessageW(&Message, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Message);
		DispatchMessageW(&Message);
	}

	DeleteObject(BlackBitmap);
	DeleteObject(WhiteBitmap);
	PlaySound(NULL, NULL, 0);

	return (int)Message.wParam;
}
