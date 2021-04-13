// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Windows.h>
#include <gl/GL.h>
#undef check
//#include "opencv2/core/utility.hpp"
//#include "opencv2/core/core.hpp"
//#include <opencv2/imgproc/imgproc_c.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyBlueprintFunctionLibrary.generated.h"

//#pragma comment(lib, "opengl32.lib")
//#pragma comment(lib, "opencv_world331.lib")

/**
 * 
 */
#define WIDTH 1000
#define HEIGHT 1000

UCLASS()
class UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "MyBP")
	static void PrintAdress(UObject *texture);
	UFUNCTION(BlueprintCallable, Category = "MyBP")
	static void DeleteDcOpenGL();

	static void GetPicOpenGL(int width, int height, uint8 *buf);
	static void GetDcOpenGL(int width, int height, uint8 *buf);
	static void UpdatePicOpenGL(int width, int height, uint8 *buf, int cnt);

	static HDC mhDC;
	static HBITMAP mhBMP;
	static HGLRC mhRC;

};
