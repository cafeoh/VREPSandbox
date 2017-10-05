// Fill out your copyright notice in the Description page of Project Settings.

#include "VREPSandbox.h"
#include "LevelGenerator.h"

#define LOG(format, ...) UE_LOG(LogTemp, Log, format, __VA_ARGS__)
#define LOGW(format, ...) UE_LOG(LogTemp, Warning, format, __VA_ARGS__)
#define LOGE(format, ...) UE_LOG(LogTemp, Error, format, __VA_ARGS__)

void ALevelGenerator::BeginDestroy()
{
	Clean();

	TArray<AActor*> Attachees;
	this->GetAttachedActors(Attachees);

	//UE_LOG(LogTemp, Log, TEXT("DESTRUCTION BD, found %d attachees"), Attachees.Num());

	for (AActor *Attachee : Attachees) {
		Attachee->Destroy();
	}


	Super::BeginDestroy();
}

// Clean previously generated dungeon
void ALevelGenerator::Clean()
{
	TArray<AActor*> Attachees;
	this->GetAttachedActors(Attachees);
	for(AActor *Attachee : Attachees) {
		//LOG(TEXT("FOUND ACTOR"))
		if (IsValid(Attachee)) {
			Attachee->Destroy();
		}
	}

	Rooms.Empty();
}

FVector ALevelGenerator::GetTileWorldPosition(uint32 x, uint32 y) {
	return this->GetActorLocation() + (FVector(x,y,0) - FVector(MapSizeX - 1,MapSizeY - 1,0)/2)*TileWorldSize;
}

FVector ALevelGenerator::GetTileWorldPosition(uint32 index) {
	return GetTileWorldPosition(index%MapSizeX, index/MapSizeX);
}

void ALevelGenerator::DrawArrowOnTile(uint32 x, uint32 y, EDirection d, FColor color = FColor(0, 100, 255)){
	FVector Location = GetTileWorldPosition(x,y);

	FVector Start = FVector(0, 0, 400).RotateAngleAxis(uint32(d)*90,FVector::UpVector) + Location;
	FVector End = FVector(0, -TileWorldSize*.4, 400).RotateAngleAxis(uint32(d) * 90, FVector::UpVector) + Location;
	DrawDebugDirectionalArrow(GetWorld(), Start, End, 500, color, true, 0, 0 , 20);
}

void ALevelGenerator::DrawArrowOnTile(uint32 index, EDirection d, FColor color = FColor(0, 100, 255)) {
	DrawArrowOnTile(index%MapSizeX, index/MapSizeX, d, color);
}

float ALevelGenerator::GetAdjacentExitIndex(FExitStruct Exit)
{
	const int32 DestMap[] = {-int32(MapSizeX),1,MapSizeX,-1};
	return Exit.Index + DestMap[(uint32) Exit.ExitDirection];
}

void ALevelGenerator::BuildRoom(URoom &Room) {

	for (uint32 Index : Room.GetTiles()) {
		FTransform Transform = FTransform(GetTileWorldPosition(Index%MapSizeX,Index/MapSizeX)+FVector(0,0,100));

		if (GetWorld()) {
			AActor *NewActor;
			FExitStruct *Exit;

			if(FloorBP && FloorFreeformBP){
				if(Room.IsFreeform()){
					NewActor = GetWorld()->SpawnActor<AActor>(FloorFreeformBP->GeneratedClass, Transform);
				}else{
					NewActor = GetWorld()->SpawnActor<AActor>(FloorBP->GeneratedClass, Transform);
				}
				NewActor->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepWorldTransform);
			}

			if(WallBP && DoorWallBP){
				for (EDirection Direction : {EDirection::N, EDirection::E, EDirection::S, EDirection::W}){
					// Handle 4 cardinality
					uint32 AdjIndex;
					if		(Direction == EDirection::N) AdjIndex = Index - MapSizeX;
					else if (Direction == EDirection::E) AdjIndex = Index + 1;
					else if (Direction == EDirection::S) AdjIndex = Index + MapSizeX;
					else if (Direction == EDirection::W) AdjIndex = Index - 1;

					// Don't do anything if we're looking "towards" a tile inside this room
					if(Room.GetTiles().Contains(AdjIndex)) continue;

					Transform.SetRotation(FQuat(FRotator(0, uint32(Direction)*90, 0)));
					Exit = Room.FindExit(Index, Direction);


					if(Exit && Exit->Mode == 2){
						//DrawDebugBox(GetWorld(), GetTileWorldPosition(Index) + FVector(0, -200, 100).RotateAngleAxis(uint32(Direction) * 90, FVector::UpVector), FVector(0.1, .1, 0.1)*TileWorldSize / 2, FColor(0, 0, 255), true, 0, 0, 20);
						NewActor = GetWorld()->SpawnActor<AActor>(DoorWallBP->GeneratedClass, Transform);
						NewActor->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
					}else{
						//DrawDebugBox(GetWorld(), GetTileWorldPosition(Index) + FVector(0, -200, 100).RotateAngleAxis(uint32(Direction) * 90, FVector::UpVector), FVector(0.1, .1, 0.1)*TileWorldSize / 2, FColor(255, 0, 0), true, 0, 0, 20);
						NewActor = GetWorld()->SpawnActor<AActor>(WallBP->GeneratedClass, Transform);
						NewActor->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
					}
				}
			}
		}
	}

	Room.HasBeenBuilt = true;
}

// Main generation function. Spawn a full dungeon on this actor when called
void ALevelGenerator::Generate() 
{
	Clean();

	// Grid of pointer to the room occupied by it
	TArray<URoom *> RoomGrid;
	RoomGrid.Init(0, MapSizeX*MapSizeY);

	const uint32 TotalSpace = MapSizeX*MapSizeY;

	uint32 CurrentRoomSize = FMath::Min(FMath::Min(MapSizeX,MapSizeY),MaxRoomSize); // Cap maxroom size at level size!
	uint32 CurrentRoomSizeSpaceUsed = 0;

	// Setup starting room
	URoom *StartingRoom = NewObject<URoom>();
	StartingRoom->SetIsFreeform(false);
	StartingRoom->SetSize(1, 1);
	uint32 StartingRoomIndex = (MapSizeX/2)*(MapSizeY+1);
	StartingRoom->AddTile(StartingRoomIndex);
	RoomGrid[StartingRoomIndex] = StartingRoom;
	Rooms.Add(StartingRoom);

	FRandomStream RandomStream(Seed);

	FlushPersistentDebugLines(GetWorld());

	for (uint32 i = 0; i < MaxIteration; i++) {

		// Try and fit a random uniform room

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

		uint32 x = RandomStream.RandRange(0, MapSizeX - Width);
		uint32 y = RandomStream.RandRange(0, MapSizeY - Height);

		for (uint32 offY = 0; offY < Height && IsValidPlacement; offY++) {
			for (uint32 offX = 0; offX < Width && IsValidPlacement; offX++) {
				if (RoomGrid[(x + offX) + (y + offY)*MapSizeX]) {
					IsValidPlacement = false;
				}
			}
		}

		// Room doesn't fit, go to next iteration
		if (!IsValidPlacement) continue;

		/// We know we got a uniform room available! ///

		// Make a room out of it and write it's ID to the tile
		URoom *Room = NewObject<URoom>(); 
		Room->SetIsFreeform(false);
		Room->SetSize(Width,Height);

		for (uint32 offY = 0; offY < Height; offY++) {
			for (uint32 offX = 0; offX < Width; offX++) {
				uint32 Index = (x + offX) + (y + offY)*MapSizeX;
				RoomGrid[Index] = Room;
				Room->AddTile(Index);
			}
		}

		Rooms.Add(Room);

		// Go to the lower room size if we've already put enough of this size in the level
		CurrentRoomSizeSpaceUsed += Width*Height;
		if (CurrentRoomSizeSpaceUsed >= TotalSpace / MaxRoomSize) {
			CurrentRoomSizeSpaceUsed = 0;
			CurrentRoomSize--;
			if (CurrentRoomSize <= 1) {
				break;
			}
		}
	}
	

	// Build all Freeform rooms
	for (uint32 y = 0; y < MapSizeY; y++) {
		for (uint32 x = 0; x < MapSizeX; x++) {
			uint32 Index = x + y * MapSizeX;
			
			if(RoomGrid[Index]) continue; // Regular room tile

			URoom *Room = NewObject<URoom>();
			Room->SetIsFreeform(true);

			TArray<uint32> TileStack = {Index};
			uint32 Size=0;

			while (TileStack.Num() > 0){
				uint32 CurrentIndex = TileStack.Pop();

				Room->AddTile(CurrentIndex);
				RoomGrid[CurrentIndex] = Room;

				if ((CurrentIndex % MapSizeX) > 0 &&		    RoomGrid[CurrentIndex - 1]		  == 0){
					TileStack.AddUnique(CurrentIndex - 1);
				}
				if ((CurrentIndex / MapSizeX) > 0 &&			RoomGrid[CurrentIndex - MapSizeX] == 0){
					TileStack.AddUnique(CurrentIndex - MapSizeX);
				}
				if ((CurrentIndex % MapSizeX) < (MapSizeX-1) && RoomGrid[CurrentIndex + 1]		  == 0){
					TileStack.AddUnique(CurrentIndex + 1);
				}
				if ((CurrentIndex / MapSizeX) < (MapSizeY-1) && RoomGrid[CurrentIndex + MapSizeX] == 0){
					TileStack.AddUnique(CurrentIndex + MapSizeX);
				}

				Size++;
			}

			Room->SetSize(Size);
			Rooms.Add(Room);
		}
	}
			

	for (uint32 y = 0; y < MapSizeY; y++) {
		for (uint32 x = 0 ; x < MapSizeX ; x ++){
			uint32 Index = x + y * MapSizeX;
			URoom *Room = RoomGrid[Index];

			// TODO : Implement freeform rooms
			if (!Room){
				LOGE(TEXT("FOUND EMPTY ROOM TILE!!! SHOULDN'T HAPPEN!!!"));
				continue;
			}

			// Link exits with room to the east
			if (x < MapSizeX - 1){
				URoom *AdjRoom = RoomGrid[Index + 1];
				if(AdjRoom && Room != AdjRoom){
					//DrawDebugBox(GetWorld(), (GetTileWorldPosition(x,y)+GetTileWorldPosition(x+1, y))/2, FVector(.3, .3, 0.2)*TileWorldSize / 2, FColor(0, 255, 255), true, 0, 0, 20);
					Room->AddExit   ({ EDirection::E, Index,     1, AdjRoom });
					AdjRoom->AddExit({ EDirection::W, Index + 1, 1, Room    });
				}
			}
			// Link exits with room to the south
			if (y < MapSizeY - 1) {
				URoom *AdjRoom = RoomGrid[Index + MapSizeX];
				if (AdjRoom && Room != AdjRoom) {
					//DrawDebugBox(GetWorld(), (GetTileWorldPosition(x, y) + GetTileWorldPosition(x, y+1)) / 2, FVector(.3, .3, 0.2)*TileWorldSize / 2, FColor(0, 255, 255), true, 0, 0, 20);
					Room->AddExit   ({ EDirection::S, Index,            1, AdjRoom });
					AdjRoom->AddExit({ EDirection::N, Index + MapSizeX, 1,    Room });
				}
			}
		}
	}

	uint32 MainBranchSize = RandomStream.RandRange(7,15);
	URoom *CurrentRoom = StartingRoom;
	TArray<URoom*> RoomsToBuild = {CurrentRoom};
	bool EarlyStop = false;

	for(uint32 i=0 ; i<MainBranchSize - 1 && !EarlyStop ; i++){ // -1 Because the first room is a given
		TArray<FExitStruct> &Exits = CurrentRoom->GetExits();
		uint32 ExitOffset = RandomStream.RandRange(0,Exits.Num());

		for (uint32 t = 0; t<(uint32) Exits.Num(); t++) {
			FExitStruct &CurrentExit = Exits[(t+ExitOffset)%Exits.Num()];

			if (RoomsToBuild.Contains(CurrentExit.DestinationRoom)){
				if(t == Exits.Num()-1){
					LOG(TEXT("EARLY STOP : %d/%d rooms generated"),i+1,MainBranchSize)
					EarlyStop = true;
				}
				continue;
			}

			/// Valid exit ///

			DrawArrowOnTile(CurrentExit.Index, CurrentExit.ExitDirection);

			CurrentExit.Mode = 2;

			uint32 AdjIndex = GetAdjacentExitIndex(CurrentExit);

			CurrentRoom = CurrentExit.DestinationRoom;
			CurrentRoom->FindExit(AdjIndex, EDirection((uint32(CurrentExit.ExitDirection) + 2)%4))->Mode = 2;
			RoomsToBuild.Add(CurrentRoom);

			break;
		}
	}


 	for (URoom *Room : RoomsToBuild){
 		BuildRoom(*Room);
 	}

	LastGenerationTime = FDateTime::Now().GetTicks();
}

void ALevelGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (Regenerate) {
		Regenerate = false;
	}

	RootComponent->SetWorldScale3D(FVector(MapSizeX, MapSizeY, 1.));

	/*if (GenerateInEditor) {
		Generate();
	}*/

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
		if((FDateTime::Now().GetTicks() - LastGenerationTime > 200000)){
			//LOG(TEXT("%d"), FDateTime::Now().GetTicks());
			Generate();
		}
	}else{
		Clean();
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