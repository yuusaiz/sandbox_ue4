// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"

void UMyBlueprintFunctionLibrary::PrintAdress(UObject *texture)
{
	UE_LOG(LogTemp, Log, TEXT("Adress=%llx"), (uint64)texture);
}
