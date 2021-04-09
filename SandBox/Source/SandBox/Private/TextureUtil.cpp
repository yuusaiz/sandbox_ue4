// Fill out your copyright notice in the Description page of Project Settings.


#include "TextureUtil.h"

uint8 UTextureUtil::buf[BUFSIZE];
uint8 UTextureUtil::bufred[BUFSIZE];

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

	//for (int i = 0; i < BUFSIZE; i++) {
	for (int i = 500 * 540 * 4; i < 510 * 540 * 4; i++) {
		buf[i] = rand() % 256;
	}

	Texture->UpdateTextureRegions(0, 1, Region, 4 * Texture->GetSizeX(), 4, buf);
}

void UTextureUtil::UpdateTextureWhiteNoiseRed(UTexture2D* Texture)
{
	auto Region = new FUpdateTextureRegion2D(0, 0, 0, 0, Texture->GetSizeX(), Texture->GetSizeY());

//#define BUFSIZE 960*540*4
//	uint8 buf[BUFSIZE];
	memset(bufred, 200, sizeof buf);

	//for (int i = 0; i < BUFSIZE; i++) {
	for (int i = 500 * 540 * 4; i < 510 * 540 * 4; i++) {
		bufred[i] = rand() % 256;
		if (i % 4 == 2) {
			bufred[i] = 128 + bufred[i] / 2;
		}
		
	}

	Texture->UpdateTextureRegions(0, 1, Region, 4 * Texture->GetSizeX(), 4, bufred);
}
