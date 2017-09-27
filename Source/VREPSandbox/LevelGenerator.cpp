// Fill out your copyright notice in the Description page of Project Settings.

#include "VREPSandbox.h"
#include "LevelGenerator.h"

// Create the actor for a single room from a given room structure
void ALevelGenerator::CreateRoom(RoomStruct &Room) 
{
	FTransform Transform = FTransform(FQuat::Identity, Room.Location - FVector(MapSizeX,MapSizeY,0.)/2 + (this->GetActorLocation() + FVector(0,0,100)), Room.Scale / 100);
	//Room.RoomActor = GetWorld()->SpawnActorDeferred<AActor>(FloorClass, Room.Location - FVector(MapSizeX, MapSizeY, 0.) / 2 + this->GetActorLocation(),FRotator::ZeroRotator);

	Room.RoomActor = GetWorld()->SpawnActorDeferred<AActor>(FloorClass, Transform);

	if (Room.RoomActor) {

		
		Room.RoomActor->FinishSpawning(Transform);
		Room.RoomActor->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	}
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

		CreateRoom(Room);

		Rooms.Add(Room);
	}
}

void ALevelGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) 
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	Clean();
	Generate();
}

void ALevelGenerator::OnConstruction(const FTransform &Transform)
{
	Super::OnConstruction(Transform);
	
	// Prevent scaling and rotation
	RootComponent->SetWorldScale3D(FVector(MapSizeX/100,MapSizeY/100,1.));
	RootComponent->SetWorldRotation(FQuat::Identity);

	// UE_LOG(LogTemp, Log, TEXT("CREATE"));

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

	Generate();
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

