#pragma comment(lib, "opengl32.lib")
//#pragma comment(lib, "opencv_core330.lib")
//#pragma comment(lib, "opencv_highgui330.lib")
//#pragma comment(lib, "opencv_imgcodecs330.lib")
//#pragma comment(lib, "opencv_imgproc330.lib")
#pragma comment(lib, "opencv_world331.lib")

#include <Windows.h>
#include <gl/GL.h>
#include "opencv/cv.hpp"

#define WIDTH 1000
#define HEIGHT 1000

static PIXELFORMATDESCRIPTOR pfd = {
	sizeof(PIXELFORMATDESCRIPTOR),
	1,
	PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL,
	PFD_TYPE_RGBA,
	32,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,
	24,
	0,
	0,
	PFD_MAIN_PLANE,
	0,
	0, 0, 0,
};

BITMAPINFO bmi = {
	{
		sizeof(BITMAPINFOHEADER),
		WIDTH,
		HEIGHT,
		1,
		32,
		BI_RGB,
		0, 0, 0, 0, 0,
	},
};

int main(void)
{
	/* ハンドルデバイスコンテキスト(HDC)を作成 */
	HDC hDC = CreateCompatibleDC(NULL);

	/* 描画領域となるビットマップを作成 */
	void *pvBits;
	HBITMAP hBMP = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);

	/* HDCにビットマップを設定 */
	SelectObject(hDC, hBMP);

	/* ピクセルフォーマットを設定 */
	int pixFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, pixFormat, &pfd);

	/* レンダリングコンテキスト(RC)を作成 */
	HGLRC hRC = wglCreateContext(hDC);

	/* RCを描画対象に設定 */
	wglMakeCurrent(hDC, hRC);

	/* OpenGLで描画 */
	glBegin(GL_QUADS);
	glColor3f(1.0, 0.0, 0.0);
	glVertex2d(-0.5, -0.5);
	glColor3f(0.0, 1.0, 0.0);
	glVertex2d(-0.5, 0.5);
	glColor3f(0.0, 0.0, 1.0);
	glVertex2d(0.5, 0.5);
	glColor3f(1.0, 1.0, 1.0);
	glVertex2d(0.5, -0.5);
	glEnd();

	/* 描画したデータを取得 */
	IplImage *img = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 4);
	glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, img->imageData);

	/* RGB->BGRに変換して画像保存 */
	cvCvtColor(img, img, CV_RGBA2BGRA);
	cvSaveImage("opengl.bmp", img);

	wglMakeCurrent(NULL, NULL);

	/* リソースの破棄 */
	cvReleaseImage(&img);
	wglDeleteContext(hRC);
	DeleteObject(hBMP);
	DeleteDC(hDC);

	return 0;
}