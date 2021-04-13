// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"

HDC UMyBlueprintFunctionLibrary::mhDC;
HBITMAP UMyBlueprintFunctionLibrary::mhBMP;
HGLRC UMyBlueprintFunctionLibrary::mhRC;

void UMyBlueprintFunctionLibrary::PrintAdress(UObject *texture)
{
	UE_LOG(LogTemp, Log, TEXT("Adress=%llx"), (uint64)texture);
}


void UMyBlueprintFunctionLibrary::GetPicOpenGL(int width, int height, uint8 *buf)
{
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL"));

	PIXELFORMATDESCRIPTOR pfd = {
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
			width,
			height,
			1,
			32,
			BI_RGB,
			0, 0, 0, 0, 0,
		},
	};




	/* ハンドルデバイスコンテキスト(HDC)を作成 */
	HDC hDC = CreateCompatibleDC(NULL);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL %x"), __LINE__, hDC);

	/* 描画領域となるビットマップを作成 */
	void *pvBits;
	HBITMAP hBMP = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL %x"), __LINE__, hBMP);

	/* HDCにビットマップを設定 */
	SelectObject(hDC, hBMP);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL %x"), __LINE__, hBMP);

	/* ピクセルフォーマットを設定 */
	int pixFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, pixFormat, &pfd);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL %x"), __LINE__, pixFormat);

	/* レンダリングコンテキスト(RC)を作成 */
	HGLRC hRC = wglCreateContext(hDC);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL %x"), __LINE__, hRC);

	/* RCを描画対象に設定 */
	wglMakeCurrent(hDC, hRC);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL %x"), __LINE__, hRC);

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
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL"), __LINE__);

	/* 描画したデータを取得 */
	//IplImage *img = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 4);
	//glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, img->imageData);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buf);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL"), __LINE__);


	/* RGB->BGRに変換して画像保存 */
	//cvCvtColor(img, img, CV_RGBA2BGRA);
	//cvSaveImage("opengl.bmp", img);

	wglMakeCurrent(NULL, NULL);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL"), __LINE__);

	/* リソースの破棄 */
	//cvReleaseImage(&img);
	wglDeleteContext(hRC);
	DeleteObject(hBMP);
	DeleteDC(hDC);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL"), __LINE__);

	//return 0;

}
void UMyBlueprintFunctionLibrary::GetDcOpenGL(int width, int height, uint8 *buf)
{
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL"));

	PIXELFORMATDESCRIPTOR pfd = {
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
			width,
			height,
			1,
			32,
			BI_RGB,
			0, 0, 0, 0, 0,
		},
	};




	/* ハンドルデバイスコンテキスト(HDC)を作成 */
	mhDC = CreateCompatibleDC(NULL);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL %x"), __LINE__, mhDC);

	/* 描画領域となるビットマップを作成 */
	void *pvBits;
	mhBMP = CreateDIBSection(mhDC, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL %x"), __LINE__, mhBMP);

	/* HDCにビットマップを設定 */
	SelectObject(mhDC, mhBMP);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL %x"), __LINE__, mhBMP);

	/* ピクセルフォーマットを設定 */
	int pixFormat = ChoosePixelFormat(mhDC, &pfd);
	SetPixelFormat(mhDC, pixFormat, &pfd);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL %x"), __LINE__, pixFormat);

	/* レンダリングコンテキスト(RC)を作成 */
	mhRC = wglCreateContext(mhDC);
	UE_LOG(LogTemp, Log, TEXT("UpdatePicOpenGL %dL %x"), __LINE__, mhRC);

}

void UMyBlueprintFunctionLibrary::UpdatePicOpenGL(int width, int height, uint8 *buf, int cnt)
{
	//UE_LOG(LogTemp, Log, TEXT("UpdatePicOpenGL"));
	cnt %= 256;

	/* RCを描画対象に設定 */
	wglMakeCurrent(mhDC, mhRC);

	/* OpenGLで描画 */
	glBegin(GL_QUADS);
	glColor3f(1.0, 0.0, 0.0);
	glVertex2d(-0.5, -0.5);
	glColor3f(0.0, 1.0, 0.0);
	glVertex2d(-0.5, 0.5);
	glColor3f(0.0, 0.0, 1.0);
	glVertex2d(0.5, 0.5);
	glColor3f(1.0 * cnt / 256, 1.0 * cnt / 256, 1.0);
	glVertex2d(0.5, -0.5);
	glEnd();
	//UE_LOG(LogTemp, Log, TEXT("UpdatePicOpenGL %dL"), __LINE__);

	/* 描画したデータを取得 */
	//IplImage *img = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 4);
	//glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, img->imageData);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buf);
	//UE_LOG(LogTemp, Log, TEXT("UpdatePicOpenGL %dL"), __LINE__);


	/* RGB->BGRに変換して画像保存 */
	//cvCvtColor(img, img, CV_RGBA2BGRA);
	//cvSaveImage("opengl.bmp", img);

	wglMakeCurrent(NULL, NULL);
	//UE_LOG(LogTemp, Log, TEXT("UpdatePicOpenGL %dL"), __LINE__);

	/* リソースの破棄 */
	//cvReleaseImage(&img);

	//return 0;

}

void UMyBlueprintFunctionLibrary::DeleteDcOpenGL()
{
	UE_LOG(LogTemp, Log, TEXT("DeleteDcOpenGL"));

	wglDeleteContext(mhRC);
	DeleteObject(mhBMP);
	DeleteDC(mhDC);
}