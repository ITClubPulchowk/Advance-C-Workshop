#include <Windows.h>
#include <avrt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#pragma comment(lib, "Kernel32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Avrt.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "Gdi32.lib")

static WINDOWPLACEMENT g_window_placement;

static void toggle_fullscreen(HWND hwnd) {
	DWORD dwStyle = GetWindowLongW(hwnd, GWL_STYLE);
	if (dwStyle & WS_OVERLAPPEDWINDOW) {
		MONITORINFO mi = { sizeof(mi) };
		if (GetWindowPlacement(hwnd, &g_window_placement) &&
			GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi)) {
			SetWindowLongW(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(hwnd, HWND_TOP,
				mi.rcMonitor.left, mi.rcMonitor.top,
				mi.rcMonitor.right - mi.rcMonitor.left,
				mi.rcMonitor.bottom - mi.rcMonitor.top,
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
	}
	else {
		SetWindowLongW(hwnd, GWL_STYLE,
			dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(hwnd, &g_window_placement);
		SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
}

static LRESULT CALLBACK window_proc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	LRESULT result = 0;

	switch (msg) {
	case WM_DPICHANGED:
	{
		RECT *const suggested_rect = (RECT *)lparam;
		int        left = suggested_rect->left;
		int        top = suggested_rect->top;
		int        width = suggested_rect->right - suggested_rect->left;
		int        height = suggested_rect->bottom - suggested_rect->top;
		SetWindowPos(wnd, 0, left, top, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
	} break;

	case WM_CLOSE:
	{
		PostQuitMessage(0);
	} break;

	case WM_KEYDOWN:
	{
		if (wparam == VK_F11) {
			toggle_fullscreen(wnd);
		}
		else if (wparam == VK_ESCAPE) {
			PostQuitMessage(0);
		}
	} break;

	default:
	{
		result = DefWindowProcW(wnd, msg, wparam, lparam);
	}
	}

	return result;
}

static void present(HWND window, int framebuffer_w, int framebuffer_h, uint32_t *pixels) {
	RECT rect = { 0 };
	GetClientRect(window, &rect);

	int w = rect.right - rect.left;
	int h = rect.bottom - rect.top;

	BITMAPINFO info;
	info.bmiHeader.biSize = sizeof(info.bmiHeader);
	info.bmiHeader.biWidth = framebuffer_w;
	info.bmiHeader.biHeight = framebuffer_h;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = 32;
	info.bmiHeader.biCompression = BI_RGB;
	info.bmiHeader.biSizeImage = 0;

	HDC hdc = GetDC(window);

	StretchDIBits(hdc, 0, 0, w, h,
		0, 0, framebuffer_w, framebuffer_h, pixels, &info,
		DIB_RGB_COLORS, SRCCOPY);

	ReleaseDC(window, hdc);
}

float time_spend_since(LARGE_INTEGER old_counter, LARGE_INTEGER frequency) {
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return ((1000000.0f * (float)(counter.QuadPart - old_counter.QuadPart)) / (float)frequency.QuadPart) / 10000.0f;
}

void platform_init() {
	DWORD task_index = 0;
	HANDLE task_handle = AvSetMmThreadCharacteristicsW(L"Games", &task_index);

	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}

void paltform_render_loop(int framebuffer_w, int framebuffer_h, uint32_t *pixels) {
	HINSTANCE instance = GetModuleHandleW(NULL);

	WNDCLASSEXW window_class = { sizeof(window_class) };
	window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc = window_proc;
	window_class.hInstance = instance;
	window_class.hIcon = NULL;
	window_class.hCursor = LoadCursorW(NULL, IDC_ARROW);
	window_class.lpszClassName = L"MandelWindowClass";

	RegisterClassExW(&window_class);
 
	int window_width  = framebuffer_w;
	int window_height = framebuffer_h;

	RECT  wrc;
	ZeroMemory(&wrc, sizeof(wrc));

	wrc.right  = window_width;
	wrc.bottom = window_height;
	AdjustWindowRectExForDpi(&wrc, WS_OVERLAPPEDWINDOW, FALSE, 0, DPI_AWARENESS_PER_MONITOR_AWARE);
	window_width = wrc.right - wrc.left;
	window_height = wrc.bottom - wrc.top;

	HWND window = CreateWindowExW(0, L"MandelWindowClass", L"Mandelbrot",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, window_width, window_height,
		NULL, NULL, instance, NULL);

	if (window == NULL) {
		MessageBoxW(NULL, L"Window could not be created", L"Fatal Error", MB_ICONERROR | MB_OK);
		ExitProcess(1);
	}

	const int hz = 60;
	const float update_rate_ms = 1000.0f / (float)hz;

	ShowWindow(window, SW_SHOWNORMAL);
	UpdateWindow(window);

	g_window_placement.length = sizeof(g_window_placement);
	GetWindowPlacement(window, &g_window_placement);

	bool sleep_is_granular = (timeBeginPeriod(1) == TIMERR_NOERROR);

	LARGE_INTEGER counter, frequency;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&counter);

	bool running = true;
	bool print_time = true;

	while (running) {
		MSG msg;
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				running = false;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		present(window, framebuffer_w, framebuffer_h, pixels);

		if (print_time && ChunkIndex >= MaxChunkCount) {
			float time = clock_time(TimeCounts);
			printf("Render Time: %f ms\n", time);
			print_time = false;
		}

		float time_taken_ms = time_spend_since(counter, frequency);
		while (time_taken_ms < update_rate_ms) {
			if (sleep_is_granular) {
				uint32_t time_to_wait_ms = (uint32_t)(update_rate_ms - time_taken_ms);
				if (time_to_wait_ms > 0) {
					Sleep(time_to_wait_ms);
				}
			}
			time_taken_ms = time_spend_since(counter, frequency);
		}

		QueryPerformanceCounter(&counter);
	}

}
