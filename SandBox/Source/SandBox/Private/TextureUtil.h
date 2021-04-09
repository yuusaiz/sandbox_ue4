// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TextureUtil.generated.h"

#define BUFSIZE 960*540*4

/**
 * 
 */
UCLASS()
class UTextureUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, Category = "ObjectDeliverer Test")
	static UTexture2D* CreateTexture(int32 Width, int32 Height);

	UFUNCTION(BlueprintCallable, Category = "ObjectDeliverer Test")
	static void UpdateTexture(UTexture2D* Texture, const TArray<uint8>& PixelsBuffer);

	UFUNCTION(BlueprintCallable, Category = "ObjectDeliverer Test")
	static void UpdateTextureWhiteNoise(UTexture2D* Texture);

	UFUNCTION(BlueprintCallable, Category = "ObjectDeliverer Test")
	static void UpdateTextureWhiteNoiseRed(UTexture2D* Texture);

	static uint8 buf[BUFSIZE];
	static uint8 bufred[BUFSIZE];

};
