// Fill out your copyright notice in the Description page of Project Settings.


#include "TextureUtil.h"
#include "MyBlueprintFunctionLibrary.h"

#undef UpdateResource


uint8 UTextureUtil::buf[BUFSIZE];
uint8 UTextureUtil::bufred[BUFSIZE];
uint8 UTextureUtil::bufGL[BUFSIZE];
int UTextureUtil::count;

UTexture2D* UTextureUtil::CreateTexture(int32 Width, int32 Height)
{
	auto Texture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
	Texture->UpdateResource();

	return Texture;
}

void UTextureUtil::UpdateTexture(UTexture2D* Texture, const TArray<uint8>& PixelsBuffer)
{
	auto Region = new FUpdateTextureRegion2D(0, 0, 0, 0, Texture->GetSizeX(), Texture->GetSizeY());
	Texture->UpdateTextureRegions(0, 1, Region, 4 * Texture->GetSizeX(), 4, (uint8*)PixelsBuffer.GetData());
}

void UTextureUtil::UpdateTextureWhiteNoise(UTexture2D* Texture)
{
	auto Region = new FUpdateTextureRegion2D(0, 0, 0, 0, Texture->GetSizeX(), Texture->GetSizeY());

//	uint8 buf[BUFSIZE];
	memset(buf, 100, sizeof buf);

#if 1
	//for (int i = 0; i < BUFSIZE; i++) {
	for (int i = 500 * 540 * 4; i < 510 * 540 * 4; i++) {
		buf[i] = rand() % 256;
	}
#endif

	Texture->UpdateTextureRegions(0, 1, Region, 4 * Texture->GetSizeX(), 4, buf, [](auto InTextureData, auto InRegions)
	{
		//delete InTextureData;
		delete InRegions;
	});
}

void UTextureUtil::UpdateTextureWhiteNoiseRed(UTexture2D* Texture)
{
	auto Region = new FUpdateTextureRegion2D(0, 0, 0, 0, Texture->GetSizeX(), Texture->GetSizeY());

//#define BUFSIZE 960*540*4
//	uint8 buf[BUFSIZE];
	memset(bufred, count % 256, sizeof buf);
	count++;

#if 1
	//for (int i = 0; i < BUFSIZE; i++) {
	for (int i = 500 * 540 * 4; i < 510 * 540 * 4; i++) {
		bufred[i] = rand() % 256;
		if (i % 4 == 2) {
			bufred[i] = 128 + bufred[i] / 2;
		}
	}
#endif

	Texture->UpdateTextureRegions(0, 1, Region, 4 * Texture->GetSizeX(), 4, bufred, [](auto InTextureData, auto InRegions)
	{
		//delete InTextureData;
		delete InRegions;
	});
}

UTexture2D* UTextureUtil::CreateTextureByGL(int32 Width, int32 Height)
{
	auto Texture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
	Texture->UpdateResource();

	UMyBlueprintFunctionLibrary::GetDcOpenGL(Texture->GetSizeX(), Texture->GetSizeY(), bufGL);


	return Texture;
}

void UTextureUtil::UpdateTextureByGL(UTexture2D* Texture)
{
	UMyBlueprintFunctionLibrary::UpdatePicOpenGL(Texture->GetSizeX(), Texture->GetSizeY(), bufGL, count);
	auto Region = new FUpdateTextureRegion2D(0, 0, 0, 0, Texture->GetSizeX(), Texture->GetSizeY());

	Texture->UpdateTextureRegions(0, 1, Region, 4 * Texture->GetSizeX(), 4, bufGL, [](auto InTextureData, auto InRegions)
	{
		//delete InTextureData;
		delete InRegions;
	}
	);

}
