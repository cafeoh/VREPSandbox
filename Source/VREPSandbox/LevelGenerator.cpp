// Fill out your copyright notice in the Description page of Project Settings.

#include "VREPSandbox.h"
#include "LevelGenerator.h"

#define LOG(format, ...) UE_LOG(LogTemp, Log, format, ##__VA_ARGS__)

// Create the actor for a single room from a given room structure
void ALevelGenerator::CreateRoom(RoomStruct &Room) 
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
	for (RoomStruct &Room : Rooms) {
		if (Room.RoomActor) {
			Room.RoomActor->Destroy();
		}
	}
	Rooms.Empty(); 
}

// Main generation function. Spawn a full dungeon on this actor when called
void ALevelGenerator::Generate() 
{
	Clean();

	FRandomStream RandomStream(Seed);

	for (uint32 i = 0 ; i < NumberOfRooms ; i++) {
		RoomStruct Room;

		Room.Scale = FVector(RandomStream.FRandRange(500, 1000), RandomStream.FRandRange(500, 1000), 300);
		Room.Location = FVector(RandomStream.FRandRange(Room.Scale.X, MapSizeX), RandomStream.FRandRange(Room.Scale.Y, MapSizeY), 0) - FVector(Room.Scale.X, Room.Scale.Y, 0.)/2;

		Rooms.Add(Room);
	}

	for (uint32 i = 0; i < 1000; i++) {

	}

	for (RoomStruct Room : Rooms) {
		CreateRoom(Room);
	}

}

void ALevelGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (Regenerate) {
		Regenerate = false;
	}

	Clean();

	if (GenerateInEditor) {
		Generate();
	}
}

void ALevelGenerator::OnConstruction(const FTransform &Transform)
{
	Super::OnConstruction(Transform);
	
	// Prevent scaling and rotation
	RootComponent->SetWorldScale3D(FVector(MapSizeX/100,MapSizeY/100,1.));
	RootComponent->SetWorldRotation(FQuat::Identity);

	if (GenerateInEditor) {
		if (Rooms.Num() == 0) {
			Generate();
		}

		if (RegenerateChildrenOnMove) {
			for (RoomStruct &Room : Rooms) {
				if (Room.RoomActor && Room.RoomActor->IsValidLowLevel()) {
					Room.RoomActor->RerunConstructionScripts();
				}
			}
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
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneVisualAsset(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	if (PlaneVisualAsset.Succeeded())
	{
		PlaneRoot->SetStaticMesh(PlaneVisualAsset.Object);
		PlaneRoot->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
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

