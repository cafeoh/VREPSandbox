// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Room.h"
#include "LevelGenerator.generated.h"


UCLASS()
class VREPSANDBOX_API ALevelGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevelGenerator();

	// Property
	UPROPERTY(EditAnywhere, Category = "Generation")
		uint32 MapSizeX = 5;

	UPROPERTY(EditAnywhere, Category = "Generation")
		uint32 MapSizeY = 5;


	UPROPERTY(EditAnywhere, Category = "Generation")
		uint32 Seed = 12345;

	UPROPERTY(EditAnywhere, Category = "Generation")
		uint32 MaxIteration = 100;

	UPROPERTY(EditAnywhere, Category = "Generation")
		uint32 MaxRoomSize = 3;

	UPROPERTY(EditAnywhere, Category = "Generation")
		bool GenerateInEditor = true;

	UPROPERTY(EditAnywhere, Category = "Generation")
		bool RegenerateChildrenOnMove = true;

	UPROPERTY(EditAnywhere, Category = "Generation")
		bool Regenerate = false;



	UPROPERTY(EditAnywhere, Category = "Building Blocks")
		UStaticMesh *FloorClass;

	UPROPERTY()
		float TileWorldSize;

protected:

	UInstancedStaticMeshComponent *Floor1;

	UPROPERTY()
	TArray<URoom*> Rooms;

	void BuildRoom(URoom &Room);
	void Generate();
	void Clean();

	UInstancedStaticMeshComponent *FloorISM;

	FVector GetTileWorldPosition(uint32 x, uint32 y);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform &Transform) override;
	virtual bool CanEditChange(const UProperty* InProperty) const;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void BeginDestroy() override;
	//virtual bool Destroy(bool bNetForce, bool bShouldModifyLevel) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
