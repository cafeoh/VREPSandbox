// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Room.generated.h"

UENUM(BlueprintType)
enum class EExitDirection : uint8
{
	N	 UMETA(DisplayName = "North"),
	E	 UMETA(DisplayName = "East"),
	S	 UMETA(DisplayName = "South"),
	W	 UMETA(DisplayName = "West")
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
	EExitDirection ExitDirection;

	// Tile inside of the owning room
	UPROPERTY()
	uint32 X;
	UPROPERTY()
	uint32 Y;

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

	bool IsFreeform();

	TArray<uint32> &GetTiles();
	TArray<FExitStruct> &GetExits();

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
