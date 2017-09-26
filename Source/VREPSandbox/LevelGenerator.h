// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGenerator.generated.h"

UCLASS()
class VREPSANDBOX_API ALevelGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelGenerator();

//  UPROPERTY(EditAnywhere, Category = "Meshes")
//  UStaticMesh *Floor1Mesh;

// 	UPROPERTY(EditAnywhere, Category = "Generation")
// 	float Spacing = 1000;


	UPROPERTY(EditAnywhere, Category = "Generation")
	int32 MapSizeX = 100;

	UPROPERTY(EditAnywhere, Category = "Generation")
	int32 MapSizeY = 100;


	UPROPERTY(EditAnywhere, Category = "Building Blocks")
	UBlueprint *FloorBP;
	
	

private:

	UInstancedStaticMeshComponent *Floor1;
	TArray<int32> FloorMap;	
	TArray<int32> PillarMap;
	TArray<int32> WallMap;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform &Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
