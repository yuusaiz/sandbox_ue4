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




	/* �n���h���f�o�C�X�R���e�L�X�g(HDC)���쐬 */
	HDC hDC = CreateCompatibleDC(NULL);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL %x"), __LINE__, hDC);

	/* �`��̈�ƂȂ�r�b�g�}�b�v���쐬 */
	void *pvBits;
	HBITMAP hBMP = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL %x"), __LINE__, hBMP);

	/* HDC�Ƀr�b�g�}�b�v��ݒ� */
	SelectObject(hDC, hBMP);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL %x"), __LINE__, hBMP);

	/* �s�N�Z���t�H�[�}�b�g��ݒ� */
	int pixFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, pixFormat, &pfd);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL %x"), __LINE__, pixFormat);

	/* �����_�����O�R���e�L�X�g(RC)���쐬 */
	HGLRC hRC = wglCreateContext(hDC);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL %x"), __LINE__, hRC);

	/* RC��`��Ώۂɐݒ� */
	wglMakeCurrent(hDC, hRC);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL %x"), __LINE__, hRC);

	/* OpenGL�ŕ`�� */
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

	/* �`�悵���f�[�^���擾 */
	//IplImage *img = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 4);
	//glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, img->imageData);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buf);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL"), __LINE__);


	/* RGB->BGR�ɕϊ����ĉ摜�ۑ� */
	//cvCvtColor(img, img, CV_RGBA2BGRA);
	//cvSaveImage("opengl.bmp", img);

	wglMakeCurrent(NULL, NULL);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL"), __LINE__);

	/* ���\�[�X�̔j�� */
	//cvReleaseImage(&img);
	wglDeleteContext(hRC);
	DeleteObject(hBMP);
	DeleteDC(hDC);
	UE_LOG(LogTemp, Log, TEXT("GetPicOpenGL %dL"), __LINE__);

	//return 0;

}

HWND UMyBlueprintFunctionLibrary::GetWindowDc(int width, int height)
{
	// �A�v���P�[�V�����̏�����
	LPCWSTR	gName = L"OpenGL�e�X�g";

	//MSG msg;
	HWND hWnd;
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_OWNDC | CS_HREDRAW | CS_VREDRAW, DefWindowProc, 0, 0, hInstance, NULL, NULL, (HBRUSH)(COLOR_WINDOW + 1), NULL, (TCHAR*)gName, NULL };
	if (!RegisterClassEx(&wcex)) {
		UE_LOG(LogTemp, Log, TEXT("RegisterClassEx fail %dL"), __LINE__);
		return 0;
	}

	RECT R = { 0, 0, width, height };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, FALSE);
	hWnd = CreateWindow(gName, gName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, R.right - R.left, R.bottom - R.top, NULL, NULL, hInstance, NULL);
	if (!hWnd) {
		UE_LOG(LogTemp, Log, TEXT("CreateWindow fail %dL"), __LINE__);
		return 0;
	}
	return hWnd;
}

void UMyBlueprintFunctionLibrary::GetDcOpenGL(int width, int height, uint8 *buf, bool createwin)
{
	UE_LOG(LogTemp, Log, TEXT("GetDcOpenGL"));

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

	if (createwin) {
		HWND tmphDC = GetWindowDc(width, height);
		mhDC = GetDC(tmphDC);
	}
	else {
		/* �n���h���f�o�C�X�R���e�L�X�g(HDC)���쐬 */
		mhDC = CreateCompatibleDC(NULL);
		UE_LOG(LogTemp, Log, TEXT("GetDcOpenGL %dL %x"), __LINE__, mhDC);

	}



	/* �`��̈�ƂȂ�r�b�g�}�b�v���쐬 */
	void *pvBits;
	mhBMP = CreateDIBSection(mhDC, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
	UE_LOG(LogTemp, Log, TEXT("GetDcOpenGL %dL %x"), __LINE__, mhBMP);

	/* HDC�Ƀr�b�g�}�b�v��ݒ� */
	SelectObject(mhDC, mhBMP);
	UE_LOG(LogTemp, Log, TEXT("GetDcOpenGL %dL %x"), __LINE__, mhBMP);

	/* �s�N�Z���t�H�[�}�b�g��ݒ� */
	int pixFormat = ChoosePixelFormat(mhDC, &pfd);
	SetPixelFormat(mhDC, pixFormat, &pfd);
	UE_LOG(LogTemp, Log, TEXT("GetDcOpenGL %dL %x"), __LINE__, pixFormat);

	/* �����_�����O�R���e�L�X�g(RC)���쐬 */
	mhRC = wglCreateContext(mhDC);
	UE_LOG(LogTemp, Log, TEXT("GetDcOpenGL %dL %x"), __LINE__, mhRC);

}

void UMyBlueprintFunctionLibrary::UpdatePicOpenGL(int width, int height, uint8 *buf, int cnt)
{
	UE_LOG(LogTemp, Log, TEXT("UpdatePicOpenGL"));
	cnt %= 256;

	/* RC��`��Ώۂɐݒ� */
	wglMakeCurrent(mhDC, mhRC);

	/* OpenGL�ŕ`�� */
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

	/* �`�悵���f�[�^���擾 */
	//IplImage *img = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 4);
	//glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, img->imageData);
	uint8 tmp = buf[100];
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buf);
	UE_LOG(LogTemp, Log, TEXT("UpdatePicOpenGL %dL [100] %d -> %d"), __LINE__, tmp, buf[100]);


	/* RGB->BGR�ɕϊ����ĉ摜�ۑ� */
	//cvCvtColor(img, img, CV_RGBA2BGRA);
	//cvSaveImage("opengl.bmp", img);

	wglMakeCurrent(NULL, NULL);
	//UE_LOG(LogTemp, Log, TEXT("UpdatePicOpenGL %dL"), __LINE__);

	/* ���\�[�X�̔j�� */
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