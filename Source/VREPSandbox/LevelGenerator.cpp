// Fill out your copyright notice in the Description page of Project Settings.

#include "VREPSandbox.h"
#include "LevelGenerator.h"

#define LOG(format, ...) UE_LOG(LogTemp, Log, format, ##__VA_ARGS__)

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

// Main generation function. Spawn a full dungeon on this actor when called
void ALevelGenerator::Generate() 
{
	Clean();

	FRandomStream RandomStream(Seed);

	/*for (uint32 i = 0 ; i < NumberOfRooms ; i++) {
		FRoomStruct Room;

		Room.Scale = FVector(RandomStream.FRandRange(500, 1000), RandomStream.FRandRange(500, 1000), 300);
		Room.Location = FVector(RandomStream.FRandRange(Room.Scale.X, MapSizeX), RandomStream.FRandRange(Room.Scale.Y, MapSizeY), 0) - FVector(Room.Scale.X, Room.Scale.Y, 0.)/2;

		Rooms.Add(Room);
	}

	for (uint32 pass = 0; pass < PushIteration; pass++) {
		bool Overlap = false;

		TArray<FVector> DisplacementToApply;
		DisplacementToApply.Init(FVector(0.),Rooms.Num());

		for (int i = 0; i < Rooms.Num(); i++) {
			for (int j = 0; j < Rooms.Num(); j++) {
				if (i == j) continue;
				
				FRoomStruct Room1 = Rooms[i];
				FRoomStruct Room2 = Rooms[j];

				FVector DeltaLocation = Room2.Location - Room1.Location;
				float OverlapStrength = (DeltaLocation.GetAbs() - (Room1.Scale + Room2.Scale) / 2).GetMax();

				if (OverlapStrength < 0) {
					DisplacementToApply[j] -= DeltaLocation.GetUnsafeNormal().RotateAngleAxis(5,FVector(0,0,1))*OverlapStrength*PushForce;
					Overlap = true;
				}
			}
		}

		if (!Overlap){
			UE_LOG(LogTemp, Log, TEXT("Finished early after %d iterations"), pass + 1);
			break;
		} else {
			for (int n = 0; n < Rooms.Num(); n++) {
				Rooms[n].Location += DisplacementToApply[n];
				UE_LOG(LogTemp, Log, TEXT("Before : %s"), *(Rooms[n].Location.ToString()));
				Rooms[n].Location = Rooms[n].Location.ComponentMax(FVector(0)+Rooms[n].Scale/2);
				Rooms[n].Location = Rooms[n].Location.ComponentMin(FVector( MapSizeX,  MapSizeY, 0.)-Rooms[n].Scale/2);
				Rooms[n].Location.Z = 0;
				UE_LOG(LogTemp, Log, TEXT("After : %s"), *(Rooms[n].Location.ToString()));
			}
		}

	}

	for (FRoomStruct &Room : Rooms) {
		CreateRoom(Room);
	}
	*/

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
			for (FRoomStruct &Room : Rooms) {
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

