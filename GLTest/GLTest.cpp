
#pragma comment(lib, "OpenGL32.lib")
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tchar.h>
#include <stdint.h>
#include <stdio.h>
#include "GL/glew.h"
#include "GL/eglew.h"
#include "GL/wglew.h"


LPCWSTR	gName = L"OpenGLテスト";
HDC dc;
HGLRC glRC;
int count;

// ウィンドウプロシージャ
LRESULT CALLBACK WndProc(HWND hWnd, UINT mes, WPARAM wParam, LPARAM lParam) {
	if (mes == WM_DESTROY || mes == WM_CLOSE) {
		PostQuitMessage(0);
		return 0; 
	}
	return DefWindowProc(hWnd, mes, wParam, lParam);
}

int
dump_frame()
{
	const int sz = 800 * 600 * 4;
	byte* buf = new byte[sz];

	// サイズ 800, 600 の画面を buf へコピーする
	glReadBuffer(GL_BACK);
	glReadPixels(0, 0, 800, 600, GL_RGBA, GL_UNSIGNED_BYTE, buf);

	// Fileに出力する
	FILE *fp = fopen("capture.raw", "wb");
	fwrite(buf, sz, 1, fp);
	fclose(fp);

	delete buf;
	return 0;
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
	dc = GetDC(hWnd);
	int format = ChoosePixelFormat(dc, &pfd);
	if (format == 0)
		return 0; // 該当するピクセルフォーマットが無い

	// OpenGLが使うデバイスコンテキストに指定のピクセルフォーマットをセット
	if (!SetPixelFormat(dc, format, &pfd))
		return 0; // DCへフォーマットを設定するのに失敗

	// OpenGL contextを作成
	glRC = wglCreateContext(dc);

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
			glClearColor(1.0f, 0.5f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			glRectf(-1.0f, -0.5f, 0.5f, 0.5f);

			glFlush();
			SwapBuffers(dc);
			wglMakeCurrent(NULL, NULL);

			if (count==0) {
				dump_frame();
				count++;
			}
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