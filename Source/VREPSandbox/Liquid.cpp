// Fill out your copyright notice in the Description page of Project Settings.

#include "VREPSandbox.h"
#include "Liquid.h"


// Add default functionality here for any ILiquid functions that are not pure virtual.

void ULiquid::AddQuantity(float AdditionalQuantity)
{
	Quantity = FMath::Max<float>(0., Quantity + AdditionalQuantity);
}
