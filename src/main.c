#include <windows.h>

const wchar_t CLASS_NAME[] = L"IdiotC";

LRESULT CALLBACK WndProc(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		default:
			return DefWindowProcW(hWindow, uMsg, wParam, lParam);
	}
}

int main()
{
	HINSTANCE hInstance = GetModuleHandleW(NULL);

	WNDCLASSW wc = { 0 };
	wc.lpfnWndProc   = WndProc;
	wc.hInstance     = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor       = LoadCursorW(NULL, MAKEINTRESOURCEW(IDC_ARROW));
	wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);

	if (!RegisterClassW(&wc))
		return -1;

	HWND hWindow = CreateWindowExW(
		0,
		CLASS_NAME,
		L"idiotc",
		WS_OVERLAPPEDWINDOW,

		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,

		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWindow)
		return -1;

	ShowWindow(hWindow, SW_SHOWDEFAULT);
	UpdateWindow(hWindow);

	MSG msg;
	while (GetMessageW(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return (int)msg.wParam;
}
