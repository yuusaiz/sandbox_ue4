// Fill out your copyright notice in the Description page of Project Settings.


#include "TextureUtil.h"

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

#define BUFSIZE 800*450*4
	uint8 buf[BUFSIZE];

	for (int i = 0; i < BUFSIZE; i++) {
		buf[i] = rand() % 256;
	}

	Texture->UpdateTextureRegions(0, 1, Region, 4 * Texture->GetSizeX(), 4, buf);
}
