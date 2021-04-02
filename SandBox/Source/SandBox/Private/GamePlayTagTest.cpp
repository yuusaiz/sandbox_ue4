// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlayTagTest.h"

// Sets default values
AGamePlayTagTest::AGamePlayTagTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGamePlayTagTest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGamePlayTagTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGamePlayTagTest::TestA()
{
	GameplayTags.AddTag(FGameplayTag::RequestGameplayTag(FName("MyColor.Red")));
}
