
#pragma comment(lib, "OpenGL32.lib")

#include <windows.h>
#include <tchar.h>
#include <stdint.h>
#include "GL/glew.h"
#include "GL/eglew.h"
#include "GL/wglew.h"

LPCWSTR	gName = L"OpenGLテスト";

// ウィンドウプロシージャ
LRESULT CALLBACK WndProc(HWND hWnd, UINT mes, WPARAM wParam, LPARAM lParam) {
	if (mes == WM_DESTROY || mes == WM_CLOSE) { PostQuitMessage(0); return 0; }
	return DefWindowProc(hWnd, mes, wParam, lParam);
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// アプリケーションの初期化
	MSG msg; HWND hWnd;
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_OWNDC | CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInstance, NULL, NULL, (HBRUSH)(COLOR_WINDOW + 1), NULL, (TCHAR*)gName, NULL };
	if (!RegisterClassEx(&wcex))
		return 0;

	RECT R = { 0, 0, 640, 480 };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, FALSE);
	if (!(hWnd = CreateWindow(gName, gName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, R.right - R.left, R.bottom - R.top, NULL, NULL, hInstance, NULL)))
		return 0;

	// OpenGL初期化
	// ピクセルフォーマット初期化
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, //Flags
		PFD_TYPE_RGBA, //The kind of framebuffer. RGBA or palette.
		32, //Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24, //Number of bits for the depthbuffer
		8, //Number of bits for the stencilbuffer
		0, //Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	HDC dc = GetDC(hWnd);
	int format = ChoosePixelFormat(dc, &pfd);
	if (format == 0)
		return 0; // 該当するピクセルフォーマットが無い

	// OpenGLが使うデバイスコンテキストに指定のピクセルフォーマットをセット
	if (!SetPixelFormat(dc, format, &pfd))
		return 0; // DCへフォーマットを設定するのに失敗

	// OpenGL contextを作成
	HGLRC glRC = wglCreateContext(dc);

	// 作成されたコンテキストがカレント（現在使用中のコンテキスト）か？
	if (!wglMakeCurrent(dc, glRC))
		return 0; // 何か正しくないみたい…


	ShowWindow(hWnd, nCmdShow);

	// メッセージ ループ
	do {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			wglMakeCurrent(dc, glRC);
			glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			glRectf(-0.5f, -0.5f, 0.5f, 0.5f);

			glFlush();
			SwapBuffers(dc);
			wglMakeCurrent(NULL, NULL);
		}
	} while (msg.message != WM_QUIT);

	// 後処理
	// カレントコンテキストを無効にする
	wglMakeCurrent(NULL, NULL);

	// カレントコンテキストを削除
	wglDeleteContext(glRC);

	// デバイスコンテキスト解放
	ReleaseDC(hWnd, dc);

	return 0;
}