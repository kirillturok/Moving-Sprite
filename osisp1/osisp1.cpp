#include <windows.h>

#define FONT_COLOR RGB(255,255,230)
#define TRANSPARENT_MASK RGB(255,255,255)
#define RECT_COLOR RGB(0, 128 ,0)
#define STEP 20
#define REBOUND 50

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

RECT windowSize;
HBITMAP hBitmap = NULL;
BITMAP bitmap;

POINT spritePos = { };

bool isEllipse = true;


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	const wchar_t CLASS_NAME[] = L"Sample Window Class";
	WNDCLASS wc = { };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);
	HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"OSaSP1", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);
	if (hwnd == NULL)
	{
		return 0;
	}
	ShowWindow(hwnd, nCmdShow);

	MSG msg = { };
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}

void bounceSprite() {
	if (spritePos.x < 0) {
		spritePos.x = REBOUND;
	}
	if (spritePos.x + bitmap.bmWidth > windowSize.right) {
		spritePos.x = windowSize.right - bitmap.bmWidth - REBOUND;
	}
	if (spritePos.y < 0) {
		spritePos.y = REBOUND;
	}
	if (spritePos.y + bitmap.bmHeight > windowSize.bottom) {
		spritePos.y = windowSize.bottom - bitmap.bmHeight - REBOUND;
	}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		hBitmap = (HBITMAP)LoadImage(NULL, L"D:\\tst.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (!hBitmap) {
			MessageBox(NULL, L"Load image failed!", L"Error", NULL);
			exit(1);
		}
		GetObject(hBitmap, sizeof(bitmap), &bitmap);
		GetClientRect(hwnd, &windowSize);
		break;
	case WM_DESTROY:
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:
			spritePos.x -= STEP;
			break;
		case VK_RIGHT:
			spritePos.x += STEP;
			break;
		case VK_UP:
			spritePos.y -= STEP;
			break;
		case VK_DOWN:
			spritePos.y += STEP;
			break;
		case VK_CONTROL:
			isEllipse = !isEllipse;
			InvalidateRect(hwnd, NULL, false);
			break;
		}
		bounceSprite();
		InvalidateRect(hwnd, NULL, true);
		break;
	case WM_MOUSEWHEEL:
		if (wParam & MK_SHIFT) {
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
				spritePos.x -= STEP;
			}
			else {
				spritePos.x += STEP;
			}
		}
		else {
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
				spritePos.y += STEP;
			}
			else {
				spritePos.y -= STEP;
			}
		}
		bounceSprite();
		InvalidateRect(hwnd, NULL, true);
		break;
	case WM_SIZE:
		GetClientRect(hwnd, &windowSize);
		break;
	case WM_PAINT:
		PAINTSTRUCT     ps;
		HDC             hdc;
		HDC             hdcMem;
		HBITMAP memBmp;

		hdc = BeginPaint(hwnd, &ps);

		hdcMem = CreateCompatibleDC(hdc);
		memBmp = CreateCompatibleBitmap(hdc, windowSize.right - windowSize.left, windowSize.bottom - windowSize.top);
		SelectObject(hdcMem, memBmp);
		FillRect(hdcMem, &ps.rcPaint, (HBRUSH)(CreateSolidBrush(FONT_COLOR)));

		if (!isEllipse) {
			HDC hdcBmp;
			hdcBmp = CreateCompatibleDC(hdc);
			SelectObject(hdcBmp, hBitmap);
			TransparentBlt(hdcMem, spritePos.x, spritePos.y, bitmap.bmWidth, bitmap.bmHeight, hdcBmp, 0, 0, bitmap.bmWidth, bitmap.bmHeight, TRANSPARENT_MASK);
			DeleteDC(hdcBmp);
		}
		else {
			HBRUSH brush = CreateSolidBrush(RECT_COLOR);
			SelectObject(hdcMem, brush);
			Rectangle(hdcMem, spritePos.x, spritePos.y, spritePos.x + bitmap.bmWidth, spritePos.y + bitmap.bmHeight);
			DeleteObject(brush);
		}

		BitBlt(hdc, 0, 0, windowSize.right - windowSize.left, windowSize.bottom - windowSize.top, hdcMem, 0, 0, SRCCOPY);

		DeleteDC(hdcMem);
		DeleteObject(memBmp);

		EndPaint(hwnd, &ps);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 1;
}