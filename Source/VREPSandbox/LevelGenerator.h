// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGenerator.generated.h"

UCLASS()
class VREPSANDBOX_API ALevelGenerator : public AActor
{
	GENERATED_BODY()

	struct RoomStruct {
		FVector Location;
		FVector Scale;

		AActor *RoomActor;
	};
	
public:	
	// Sets default values for this actor's properties
	ALevelGenerator();

	// Property
	UPROPERTY(EditAnywhere, Category = "Generation")
	int32 MapSizeX = 100;

	UPROPERTY(EditAnywhere, Category = "Generation")
	int32 MapSizeY = 100;

	UPROPERTY(EditAnywhere, Category = "Generation")
	int32 Seed = 12345;

	UPROPERTY(EditAnywhere, Category = "Generation")
	uint32 NumberOfRooms = 50;

	UPROPERTY(EditAnywhere, Category = "Building Blocks")
	UClass *FloorClass;

private:

	UInstancedStaticMeshComponent *Floor1;

	TArray<RoomStruct> Rooms;

	void CreateRoom(RoomStruct &Room);
	void Generate();
	void Clean();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform &Transform) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
