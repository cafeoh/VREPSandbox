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
	/*TArray<AActor*> Attachees;
	this->GetAttachedActors(Attachees);
	for(AActor *Attachee : Attachees) {
		//LOG(TEXT("FOUND ACTOR"))
		if (IsValid(Attachee)) {
			Attachee->Destroy();
		}
	}*/

	Rooms.Empty();
}

FVector ALevelGenerator::GetTileWorldPosition(int32 x, int32 y) {
	return this->GetActorLocation() + (FVector(x,y,0) - FVector(MapSizeX - 1,MapSizeY - 1,0)/2)*TileWorldSize;
}

FVector ALevelGenerator::GetTileWorldPosition(int32 index) {
	return GetTileWorldPosition(index%MapSizeX, index/MapSizeX);
}

void ALevelGenerator::DrawArrowOnTile(int32 x, int32 y, EDirection d, FColor color = FColor(0, 100, 255)){
	FVector Location = GetTileWorldPosition(x,y);

	FVector Start = FVector(0, 0, 400).RotateAngleAxis(int32(d)*90,FVector::UpVector) + Location;
	FVector End = FVector(0, -TileWorldSize*.4, 400).RotateAngleAxis(int32(d) * 90, FVector::UpVector) + Location;
	DrawDebugDirectionalArrow(GetWorld(), Start, End, 500, color, true, 0, 0 , 20);
}

void ALevelGenerator::DrawArrowOnTile(int32 index, EDirection d, FColor color = FColor(0, 100, 255)) {
	DrawArrowOnTile(index%MapSizeX, index/MapSizeX, d, color);
}

float ALevelGenerator::GetAdjacentExitIndex(FExitStruct Exit)
{
	const int32 DestMap[] = {-int32(MapSizeX),1,MapSizeX,-1};
	return Exit.Index + DestMap[(int32) Exit.ExitDirection];
}

void ALevelGenerator::BuildRoom(URoom &Room) {

	if (!GetWorld()){
		return;
	}

	for (int32 Index : Room.GetTiles()) {

		FTileStruct &Tile = Tiles[Index];
		Tile.Walls.Init(0,4);
		Tile.Enabled = true;

		for (EDirection Direction : {EDirection::N, EDirection::E, EDirection::S, EDirection::W}){
			// Handle 4 cardinality
			int32 AdjIndex;
			if		(Direction == EDirection::N) AdjIndex = Index - MapSizeX;
			else if (Direction == EDirection::E) AdjIndex = Index + 1;
			else if (Direction == EDirection::S) AdjIndex = Index + MapSizeX;
			else if (Direction == EDirection::W) AdjIndex = Index - 1;

			// Don't do anything if we're looking "towards" a tile inside this room
			if(Room.GetTiles().Contains(AdjIndex)) continue;

			int32 ExitIndex = Room.FindExitIndex(Index, Direction);

			if(ExitIndex != -1){
				FExitStruct Exit = Room.GetExits()[ExitIndex];

				if (Exit.Mode == 2) {
					Tile.Walls[(uint8) Direction] = 2;
				}else{
					Tile.Walls[(uint8) Direction] = 1;
				}
			}else{
				Tile.Walls[(uint8) Direction] = 1;
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
	Tiles.Init({0}, MapSizeX*MapSizeY);

	const int32 TotalSpace = MapSizeX*MapSizeY;

	int32 CurrentRoomSize = FMath::Min(FMath::Min(MapSizeX,MapSizeY),MaxRoomSize); // Cap maxroom size at level size!
	int32 CurrentRoomSizeSpaceUsed = 0;

	// Setup starting room
	URoom *StartingRoom = NewObject<URoom>();
	StartingRoom->SetIsFreeform(false);
	StartingRoom->SetSize(1, 1);
	//int32 StartingRoomIndex = (MapSizeX/2)*(MapSizeY+1);
	int32 StartingRoomIndex = MapSizeX*MapSizeY/2 + MapSizeX/2;
	StartingRoom->AddTile(StartingRoomIndex);
	RoomGrid[StartingRoomIndex] = StartingRoom;
	Rooms.Add(StartingRoom);

	FRandomStream RandomStream(Seed);

	FlushPersistentDebugLines(GetWorld());

	for (int32 i = 0; i < MaxIteration; i++) {

		// Try and fit a random uniform room

		int32 Width, Height;
		bool IsValidPlacement = true;

		if (RandomStream.RandRange(0, 1)) {
			Width = RandomStream.RandRange(1, CurrentRoomSize);
			Height = CurrentRoomSize;
		}
		else {
			Width = CurrentRoomSize;
			Height = RandomStream.RandRange(1, CurrentRoomSize);
		}

		int32 x = RandomStream.RandRange(0, MapSizeX - Width);
		int32 y = RandomStream.RandRange(0, MapSizeY - Height);

		for (int32 offY = 0; offY < Height && IsValidPlacement; offY++) {
			for (int32 offX = 0; offX < Width && IsValidPlacement; offX++) {
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

		for (int32 offY = 0; offY < Height; offY++) {
			for (int32 offX = 0; offX < Width; offX++) {
				int32 Index = (x + offX) + (y + offY)*MapSizeX;
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
	for (int32 y = 0; y < MapSizeY; y++) {
		for (int32 x = 0; x < MapSizeX; x++) {
			int32 Index = x + y * MapSizeX;
			
			if(RoomGrid[Index]) continue; // Regular room tile

			URoom *Room = NewObject<URoom>();
			Room->SetIsFreeform(true);

			TArray<int32> TileStack = {Index};
			int32 Size=0;

			while (TileStack.Num() > 0){
				int32 CurrentIndex = TileStack.Pop();

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
			

	for (int32 y = 0; y < MapSizeY; y++) {
		for (int32 x = 0 ; x < MapSizeX ; x ++){
			int32 Index = x + y * MapSizeX;
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

	int32 MainBranchSize = RandomStream.RandRange(7,15); // Choose a random number of room
	URoom *CurrentRoom = StartingRoom;
	RoomsToBuild = {CurrentRoom};
	bool EarlyStop = false;

	for(int32 i=0 ; i<MainBranchSize - 1 && !EarlyStop ; i++){ // -1 Because the first room is already in
		TArray<FExitStruct> &Exits = CurrentRoom->GetExits();
		int32 ExitOffset = RandomStream.RandRange(0,Exits.Num());

		for (int32 t = 0; t<(int32) Exits.Num(); t++) {
			FExitStruct &CurrentExit = Exits[(t+ExitOffset)%Exits.Num()];

			if (RoomsToBuild.Contains(CurrentExit.DestinationRoom)){
				if(t == Exits.Num()-1){
					LOGE(TEXT("EARLY STOP : %d/%d rooms generated"),i+1,MainBranchSize)
					EarlyStop = true;
				}
				continue;
			}

			/// Valid exit ///

			DrawArrowOnTile(CurrentExit.Index, CurrentExit.ExitDirection);

			CurrentExit.Mode = 2;
			CurrentRoom = CurrentExit.DestinationRoom;

			int32 AdjIndex = GetAdjacentExitIndex(CurrentExit);
			int32 ExitIndex = CurrentRoom->FindExitIndex(AdjIndex, EDirection((int32(CurrentExit.ExitDirection) + 2) % 4));

			if(ExitIndex == -1){
				LOGE(TEXT("EARLY STOP : Can't find adjacent exit!"));
				EarlyStop = true;
				continue;
			}

			CurrentRoom->GetExits()[ExitIndex].Mode = 2;
			RoomsToBuild.Add(CurrentRoom);

			break;
		}
	}

 	for (URoom *Room : RoomsToBuild){
 		BuildRoom(*Room);
 	}

	BuildLevel();

	LastGenerationTime = FDateTime::Now().GetTicks();
}

void ALevelGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	RootComponent->SetWorldScale3D(FVector(MapSizeX, MapSizeY, 1.));

	BuildLevel();

	if (GenerateInEditor || Regenerate) {
		Clean();
		Generate();
		Regenerate = false;
	}
}

void ALevelGenerator::OnConstruction(const FTransform &Transform)
{
	Super::OnConstruction(Transform);

	// Prevent scaling and rotation
	// MapSizeX = FMath::RoundToInt(RootComponent->GetComponentScale().X);
	// MapSizeY = FMath::RoundToInt(RootComponent->GetComponentScale().Y);

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
	this->bRunConstructionScriptOnDrag = false;

	// Setup the visualization plane
	UStaticMeshComponent* PlaneRoot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneVisualAsset(TEXT("StaticMesh'/Game/Meshes/Plane.Plane'"));
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