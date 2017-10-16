// Fill out your copyright notice in the Description page of Project Settings.

#include "VREPSandbox.h"
#include "Room.h"

int32 URoom::AvailableID=0;

#define LOG(format, ...) UE_LOG(LogTemp, Log, format, __VA_ARGS__)

URoom::URoom(){
	ID = AvailableID;
	AvailableID++;
}

void URoom::SetSize(int32 W, int32 H)
{
	Width = W;
	Height = H;
	Size = W*H;
}

void URoom::SetSize(int32 S)
{
	Size = S;
}

void URoom::SetIsFreeform(bool bFreeform)
{
	Freeform = bFreeform;
}

void URoom::AddTile(int32 Index)
{
	Tiles.Add(Index);
}

void URoom::AddExit(FExitStruct Exit)
{
	Exits.Add(Exit);
}

int32 URoom::GetWidth()
{
	return Width;
}

int32 URoom::GetHeight()
{
	return Height;
}

int32 URoom::GetSize()
{
	if (Freeform){
		return Size;
	}else{
		return Width*Height;
	}
}

int32 URoom::GetID()
{
	return ID;
}

// Returns the exits at the given index
TArray<FExitStruct> URoom::FindExits(int32 Index)
{
	TArray<FExitStruct> Result;
	for(FExitStruct Exit : Exits){
		if(Exit.Index == Index){
			Result.Add(Exit);
		}
	}

	return Result;
}

// Returns the index of the exit in the array, -1 if there is none
int32 URoom::FindExitIndex(int32 Index, EDirection Direction)
{
	for (int32 i=0 ; i<Exits.Num() ; i++){
		if (Exits[i].Index == Index && Exits[i].ExitDirection == Direction) {
			return i;
		}
	}

	return -1;
}

bool URoom::IsFreeform()
{
	return Freeform;
}

TArray<int32> & URoom::GetTiles()
{
	return Tiles;
}

TArray<FExitStruct> & URoom::GetExits()
{
	return Exits;
}
