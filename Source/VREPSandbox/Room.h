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

USTRUCT(BlueprintType)
struct FExitStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exit")
	EDirection ExitDirection;

	// Tile inside of the owning room
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exit")
	int32 Index;

	// 0 = No passage (wall or window)
	// 1 = Potential exit (actual exit to a room will be chosen amongst those)
	// 2 = Exit (this will be a passage)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exit")
	uint8 Mode;

	// Room this exit leads to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exit")
	URoom *DestinationRoom;

};

UCLASS()
class VREPSANDBOX_API URoom : public UObject
{
	GENERATED_BODY()

public:
	URoom();


	void SetSize(int32 W, int32 H);
	void SetSize(int32 S);
	void SetIsFreeform(bool bFreeform);
	void AddTile(int32 Index);
	void AddExit(FExitStruct Exit);

	UFUNCTION(BlueprintPure)
	int32 GetWidth();
	UFUNCTION(BlueprintPure)
	int32 GetHeight();
	UFUNCTION(BlueprintPure)
	int32 GetSize();
	UFUNCTION(BlueprintPure)
	int32 GetID();

	UFUNCTION(BlueprintPure)
	TArray<FExitStruct> FindExits(int32 Index);
	UFUNCTION(BlueprintPure)
	int32 FindExitIndex(int32 Index, EDirection Direction);

	UFUNCTION(BlueprintPure)
	bool IsFreeform();

	UFUNCTION(BlueprintPure)
	TArray<int32> &GetTiles();
	UFUNCTION(BlueprintPure)
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
	int32 ID;

	// Size of the rectangle if room isn't freeform
	UPROPERTY()
	int32 Width = 0;
	UPROPERTY()
	int32 Height = 0;
	UPROPERTY()
	int32 Size = 0;

	// Array of tiles. If not a Freeform room, this is sorted (first tile is top-leftmost, last tile is bottom-rightmost)
	UPROPERTY()
	TArray<int32> Tiles;

	// List of exits to other rooms
	UPROPERTY()
	TArray<FExitStruct> Exits;

private:
	
	static int32 AvailableID;

};
