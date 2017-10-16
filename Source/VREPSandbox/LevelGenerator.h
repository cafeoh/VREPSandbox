// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Room.h"
#include "LevelGenerator.generated.h"

// Contains all necessary information for the blueprint to spawn tiles
USTRUCT(BlueprintType)
struct FTileStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool Enabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<uint8> Walls;
};

UCLASS()
class VREPSANDBOX_API ALevelGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevelGenerator();

	// Property
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
		int32 MapSizeX = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
		int32 MapSizeY = 5;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
		int32 Seed = 12345;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
		int32 MaxIteration = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
		int32 MaxRoomSize = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
		bool GenerateInEditor = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
		bool RegenerateChildrenOnMove = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
		bool Regenerate = false;



	UPROPERTY(EditAnywhere, Category = "Building Blocks")
		UBlueprint *FloorBP;
	UPROPERTY(EditAnywhere, Category = "Building Blocks")
		UBlueprint *FloorFreeformBP;
	UPROPERTY(EditAnywhere, Category = "Building Blocks")
		UBlueprint *WallBP;
	UPROPERTY(EditAnywhere, Category = "Building Blocks")
		UBlueprint *DoorWallBP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Blocks")
		UBlueprint *PieceBP;

protected:

	UPROPERTY()
	float TileWorldSize;

	UPROPERTY()
	TArray<AActor*> Pieces;

	UPROPERTY(BlueprintReadOnly)
	TArray<URoom*> Rooms;

	UPROPERTY(BlueprintReadOnly)
	TArray<URoom*> RoomsToBuild;

	UPROPERTY(BlueprintReadOnly)
	TArray<FTileStruct> Tiles;

	int64 LastGenerationTime;

	void BuildRoom(URoom &Room);


	UFUNCTION(BlueprintImplementableEvent, Category = "Level Generator")
		void BuildLevel();
	UFUNCTION(BlueprintCallable, Category = "Level Generator")
		void Generate();
	UFUNCTION(BlueprintCallable, Category = "Level Generator")
		void Clean();

	FVector GetTileWorldPosition(int32 x, int32 y);

	UFUNCTION(BlueprintPure, Category = "Level Generator")
		FVector GetTileWorldPosition(int32 Index);

	void DrawArrowOnTile(int32 x, int32 y, EDirection d, FColor color);
	void DrawArrowOnTile(int32 index, EDirection d, FColor color);

	float GetAdjacentExitIndex(FExitStruct Exit);

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
