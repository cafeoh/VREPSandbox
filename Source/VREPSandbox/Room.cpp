// Fill out your copyright notice in the Description page of Project Settings.

#include "VREPSandbox.h"
#include "Room.h"

uint32 URoom::AvailableID=0;

#define LOG(format, ...) UE_LOG(LogTemp, Log, format, __VA_ARGS__)

URoom::URoom(){
	ID = AvailableID;
	AvailableID++;
}

void URoom::SetSize(uint32 W, uint32 H)
{
	Width = W;
	Height = H;
}

void URoom::SetIsFreeform(bool bFreeform)
{
	Freeform = bFreeform;
}

void URoom::AddTile(uint32 Index)
{
	Tiles.Add(Index);
}

void URoom::AddExit(FExitStruct &Exit)
{
	Exits.Add(Exit);
}

uint32 URoom::GetWidth()
{
	return Width;
}

uint32 URoom::GetHeight()
{
	return Height;
}

uint32 URoom::GetID()
{
	return ID;
}

bool URoom::IsFreeform()
{
	return Freeform;
}

TArray<uint32> & URoom::GetTiles()
{
	return Tiles;
}

TArray<FExitStruct> & URoom::GetExits()
{
	return Exits;
}
