// Fill out your copyright notice in the Description page of Project Settings.

#include "VREPSandbox.h"
#include "LevelGenerator.h"

// Sets default values
ALevelGenerator::ALevelGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Setup the visualisation plane
	UStaticMeshComponent* PlaneRoot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneVisualAsset(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	if (PlaneVisualAsset.Succeeded())
	{
		PlaneRoot->SetStaticMesh(PlaneVisualAsset.Object);
		PlaneRoot->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	}
	PlaneRoot->SetMobility(EComponentMobility::Static);
	RootComponent = PlaneRoot;

/*
	// Setup floor 1 ISMC
	Floor1 = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISMComp"));
	Floor1->SetupAttachment(RootComponent);
	Floor1->SetMobility(EComponentMobility::Static);
	*/
}

void ALevelGenerator::OnConstruction(const FTransform &Transform)
{
	Super::OnConstruction(Transform);
	
	// Prevent scaling and rotation
	RootComponent->SetWorldScale3D(FVector(MapSizeX,MapSizeY,1.));
	RootComponent->SetWorldRotation(FQuat());
}

// Called when the game starts or when spawned
void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();

/*
	Floor1->SetStaticMesh(Floor1Mesh);
	Floor1->SetVisibility(true, true);
	Floor1->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Floor1->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	this->AddInstanceComponent(Floor1);

	FloorMap.Init(1, MapSizeX*MapSizeY);

	for (int32 y = 0; y < MapSizeY; y++) {
		for (int32 x = 0; x < MapSizeX; x++) {
			FTransform Transform = FTransform(FVector((x - (MapSizeX - 1) / 2.) * Spacing, (y - (MapSizeY - 1) / 2.) * Spacing, 0.));
			
			Floor1->AddInstance(Transform);
		}
	}*/


	
}

// Called every frame
void ALevelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

