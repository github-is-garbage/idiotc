#include <windows.h>
#include <mmsystem.h>

#include "resource.h"

const wchar_t ClassName[] = L"IdiotC";

HBITMAP BlackBitmap;
HBITMAP WhiteBitmap;
BOOL IsWhite = TRUE;

LRESULT CALLBACK WindowProcedure(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam)
{
	switch (Message)
	{
		case WM_SIZE:
			InvalidateRect(WindowHandle, NULL, TRUE);

			return 0;

		case WM_PAINT:
		{
			PAINTSTRUCT PaintStruct;

			HDC WindowDC = BeginPaint(WindowHandle, &PaintStruct);
			HDC MemoryDC = CreateCompatibleDC(WindowDC);

			if (MemoryDC)
			{
				HBITMAP CurrentBitmap = IsWhite ? WhiteBitmap : BlackBitmap;
				HGDIOBJ OldBitmap = SelectObject(MemoryDC, CurrentBitmap);

				BITMAP BitmapInfo;
				GetObject(CurrentBitmap, sizeof(BITMAP), &BitmapInfo);

				RECT ClientRect;
				GetClientRect(WindowHandle, &ClientRect);

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

			EndPaint(WindowHandle, &PaintStruct);

			return 0;
		}

		case WM_CREATE:
			PlaySound(MAKEINTRESOURCE(IDR_YOUARE), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC | SND_LOOP);

			return 0;

		case WM_TIMER:
			if (WParam == IDT_BITMAP_TOGGLE)
			{
				IsWhite = !IsWhite;
				InvalidateRect(WindowHandle, NULL, TRUE);
			}

			return 0;

		case WM_DESTROY:
			KillTimer(WindowHandle, IDT_BITMAP_TOGGLE);
			PostQuitMessage(0);

			return 0;

		default:
			return DefWindowProcW(WindowHandle, Message, WParam, LParam);
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

	HWND MainWindow = CreateWindowExW(
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

	if (!MainWindow)
		return -1;

	ShowWindow(MainWindow, SW_SHOWDEFAULT);
	UpdateWindow(MainWindow);
	SetTimer(MainWindow, IDT_BITMAP_TOGGLE, 333, NULL);

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
