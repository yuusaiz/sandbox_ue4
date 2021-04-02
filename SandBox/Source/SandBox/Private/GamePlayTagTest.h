// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "GameFramework/Actor.h"
#include "GamePlayTagTest.generated.h"

UCLASS()
class AGamePlayTagTest : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGamePlayTagTest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// �Q�[���v���C�^�O�R���e�i
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override
	{
		TagContainer = GameplayTags;
		return;
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags")
		FGameplayTagContainer GameplayTags;

	UFUNCTION(BlueprintCallable, Category = "TEST")
	void TestA();
};
