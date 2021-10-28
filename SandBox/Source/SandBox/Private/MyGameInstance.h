// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SWebBrowser.h"
#include "MyObject.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "MyGameInstance")
	void InitSlate();
	UFUNCTION(BlueprintCallable, Category = "MyGameInstance")
	void InitSlateBrowser();

	UFUNCTION(BlueprintImplementableEvent, Category = "MyGameInstance")
	void OnClicked();

	void OnClicked2();

	TSharedPtr<SWebBrowser> m_browser;

	UPROPERTY(BlueprintReadWrite, Category = "MyGameInstance")
	UMyObject *MyObject;




};
