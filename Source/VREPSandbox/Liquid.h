// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Liquid.generated.h"

// This class does not need to be modified.
UCLASS(Blueprintable)
class VREPSANDBOX_API ULiquid : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Quantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector MRT; // Metallic Roughness Transparency


	UFUNCTION(BlueprintCallable)
		void AddQuantity(float AdditionalQuantity);
};