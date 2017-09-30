// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Room.generated.h"

UENUM(BlueprintType)
enum class EDirection : uint8
{
	N=0	 UMETA(DisplayName = "North"),
	E=1	 UMETA(DisplayName = "East"),
	S=2	 UMETA(DisplayName = "South"),
	W=3	 UMETA(DisplayName = "West")
};

/**
* Exit from a room to another (multiple exits with the same direction can point to the same room)
* Another mirrored exit should exist in the DestinationRoom
**/

class URoom;

USTRUCT()
struct FExitStruct
{
	GENERATED_BODY()

	UPROPERTY()
	EDirection ExitDirection;

	// Tile inside of the owning room
	UPROPERTY()
	uint32 Index;

	// If this exit is to be used in generation
	UPROPERTY()
	bool Enabled;

	// Room this exit leads to
	UPROPERTY()
	URoom *DestinationRoom;

};

UCLASS()
class VREPSANDBOX_API URoom : public UObject
{
	GENERATED_BODY()

public:
	URoom();


	void SetSize(uint32 W, uint32 H);
	void SetIsFreeform(bool bFreeform);
	void AddTile(uint32 Index);
	void AddExit(FExitStruct &Exit);


	uint32 GetWidth();
	uint32 GetHeight();
	uint32 GetID();

	TArray<FExitStruct> FindExits(uint32 Index);
	FExitStruct *FindExit(uint32 Index, EDirection Direction);

	bool IsFreeform();

	TArray<uint32> &GetTiles();
	TArray<FExitStruct> &GetExits();

	// Boolean used for construction
	UPROPERTY()
	bool HasBeenBuilt = false;

protected:
	// If the room is not a rectangle
	UPROPERTY()
	bool Freeform = false;

	// Unique room ID
	UPROPERTY()
	uint32 ID;

	// Bounds if Freeform, size of the rectangle otherwise
	UPROPERTY()
	uint32 Width = 0;
	UPROPERTY()
	uint32 Height = 0;

	// Array of tiles. If not a Freeform room, this is sorted (first tile is top-leftmost, last tile is bottom-rightmost)
	UPROPERTY()
	TArray<uint32> Tiles;

	// List of exits to other rooms
	UPROPERTY()
	TArray<FExitStruct> Exits;

private:
	
	static uint32 AvailableID;

};