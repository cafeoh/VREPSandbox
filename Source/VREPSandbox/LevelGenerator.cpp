// Fill out your copyright notice in the Description page of Project Settings.

#include "VREPSandbox.h"
#include "LevelGenerator.h"

#define LOG(format, ...) UE_LOG(LogTemp, Log, format, __VA_ARGS__)

// Create the actor for a single room from a given room structure
void ALevelGenerator::CreateRoom(FRoomStruct &Room) 
{
	FTransform Transform = FTransform(FQuat::Identity, Room.Location - FVector(MapSizeX,MapSizeY,0.)/2 + (this->GetActorLocation() + FVector(0,0,100)), Room.Scale / 100);

	Room.RoomActor = GetWorld()->SpawnActorDeferred<AActor>(FloorClass, Transform);

	if (Room.RoomActor) {

		Room.RoomActor->FinishSpawning(Transform, true);
		Room.RoomActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		Room.RoomActor->SetActorTransform(Transform);
		Room.RoomActor->RerunConstructionScripts();
	}
}

void ALevelGenerator::BeginDestroy()
{

	UE_LOG(LogTemp, Log, TEXT("%d"), Rooms.Num());

	Clean();

	TArray<AActor*> Attachees;
	this->GetAttachedActors(Attachees);

	UE_LOG(LogTemp, Log, TEXT("DESTRUCTION BD, found %d attachees"), Attachees.Num());

	for (AActor *Attachee : Attachees) {
		Attachee->Destroy();
	}

	Super::BeginDestroy();
}

// Clean previously generated dungeon
void ALevelGenerator::Clean()
{
	for (FRoomStruct &Room : Rooms) {
		if (Room.RoomActor && Room.RoomActor->IsValidLowLevel()) {
			Room.RoomActor->Destroy();
		}
	}

	TArray<AActor*> Attachees;
	this->GetAttachedActors(Attachees);
	for(AActor *Attachee : Attachees) {
		if (IsValid(Attachee)) {
			Attachee->Destroy();
		}
	}
	Rooms.Empty(); 
}

FVector ALevelGenerator::GetRoomWorldPosition(uint32 x, uint32 y) {
	return this->GetActorLocation() + (FVector(x,y,0) - FVector(MapSizeX - 1,MapSizeY - 1,0)/2)*TileWorldSize;
}

// Main generation function. Spawn a full dungeon on this actor when called
void ALevelGenerator::Generate() 
{
	Clean();

	TArray<uint32> RoomGrid;
	RoomGrid.Init(0, MapSizeX*MapSizeY);

	const uint32 TotalSpace = MapSizeX*MapSizeY;

	uint32 RoomID = 1;
	uint32 CurrentRoomSize = FMath::Min(FMath::Min(MapSizeX,MapSizeY),MaxRoomSize); // Cap maxroom size at level size!
	uint32 CurrentRoomSizeSpaceUsed = 0;

	FRandomStream RandomStream(Seed);

	for (uint32 i = 0; i < MaxIteration; i++) {

		uint32 Width, Height;
		bool IsValidPlacement = true;

		if (RandomStream.RandRange(0, 1)) {
			Width = RandomStream.RandRange(1, CurrentRoomSize);
			Height = CurrentRoomSize;
		}
		else {
			Width = CurrentRoomSize;
			Height = RandomStream.RandRange(1, CurrentRoomSize);
		}

		uint32 x = RandomStream.RandRange(0, MapSizeX - Width -1);
		uint32 y = RandomStream.RandRange(0, MapSizeY - Height -1);

		for (uint32 offX = 0; offX < Width && IsValidPlacement; offX++) {
			for (uint32 offY = 0; offY < Height && IsValidPlacement; offY++) {
				if (RoomGrid[(x + offX) + (y + offY)*MapSizeX] != 0) {
					IsValidPlacement = false;
				}
			}
		}

		if (!IsValidPlacement) continue;

		for (uint32 offX = 0; offX < Width; offX++) {
			for (uint32 offY = 0; offY < Height; offY++) {
				RoomGrid[(x + offX) + (y + offY)*MapSizeX] = RoomID;
			}
		}

		RoomID++;
		CurrentRoomSizeSpaceUsed += Width*Height;

		if (CurrentRoomSizeSpaceUsed >= TotalSpace / MaxRoomSize) {
			CurrentRoomSizeSpaceUsed = 0;
			CurrentRoomSize--;
			if (CurrentRoomSize <= 1) {
				break;
			}
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("%d rooms have been created!"),RoomID - 1)
	LOG(TEXT("%d rooms have been created!"), RoomID - 1)

	TMap<uint32,FColor> ColorMap;

	FlushPersistentDebugLines(GetWorld());
	for (uint32 x = 0; x < MapSizeX; x++) {
		for (uint32 y = 0; y < MapSizeY; y++) {
			uint32 CurrentRoomID = RoomGrid[x + y*MapSizeX];
			FColor Color;
			if (CurrentRoomID != 0) {
				FColor *ColorPtr = ColorMap.Find(CurrentRoomID);
				if (!ColorPtr) {
					FVector ColorVector = RandomStream.VRand() * 255;
					Color = FColor(ColorVector.X, ColorVector.Y, ColorVector.Z);
					ColorMap.Add(CurrentRoomID, Color);
				}
				else {
					Color = *ColorPtr;
				}
				DrawDebugBox(GetWorld(), GetRoomWorldPosition(x, y), FVector(TileWorldSize*.8) / 2, Color, true, 0, 0, 20);
			}
// 			DrawDebugBox(GetWorld(), GetRoomWorldPosition(x,y), FVector(TileWorldSize*.8)/2, Color, true, 0, 0,  20);
		}
	}

	

}

void ALevelGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (Regenerate) {
		Regenerate = false;
	}

	RootComponent->SetWorldScale3D(FVector(MapSizeX, MapSizeY, 1.));

	if (GenerateInEditor) {
		Generate();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void ALevelGenerator::OnConstruction(const FTransform &Transform)
{
	Super::OnConstruction(Transform);

	// Prevent scaling and rotation
	MapSizeX = FMath::RoundToInt(RootComponent->GetComponentScale().X);
	MapSizeY = FMath::RoundToInt(RootComponent->GetComponentScale().Y);

	RootComponent->SetWorldScale3D(FVector(MapSizeX,MapSizeY,1.));
	RootComponent->SetWorldRotation(FQuat::Identity);

	if (GenerateInEditor) {
		Generate();
		if (RegenerateChildrenOnMove) {
		}
	}
}

// Grey out unused options
bool ALevelGenerator::CanEditChange(const UProperty* InProperty) const
{
	if (!Super::CanEditChange(InProperty)) {
		return false;
	}

	FString PropertyName = InProperty->GetName();

	if (PropertyName == FString("Regenerate") || PropertyName == FString("RegenerateChildrenOnMove")) {
		return GenerateInEditor;
	}
	
	return true;
}

// Sets default values
ALevelGenerator::ALevelGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Setup the visualization plane
	UStaticMeshComponent* PlaneRoot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneVisualAsset(TEXT("StaticMesh'/Game/Geometry/Meshes/Plane.Plane'"));
	if (PlaneVisualAsset.Succeeded())
	{
		PlaneRoot->SetStaticMesh(PlaneVisualAsset.Object);
		PlaneRoot->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		FVector BoundMin;
		FVector BoundMax;
		PlaneRoot->GetLocalBounds(BoundMin, BoundMax);
		TileWorldSize = (BoundMax - BoundMin).GetMax();
		UE_LOG(LogTemp, Log, TEXT("Tile bounds are : %f"), TileWorldSize);
	}

	PlaneRoot->SetMobility(EComponentMobility::Static);
	RootComponent = PlaneRoot;
}

// Called when the game starts or when spawned
void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void ALevelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

