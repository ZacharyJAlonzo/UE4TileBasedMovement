// Fill out your copyright notice in the Description page of Project Settings.


#include "TileGrid.h"


// Sets default values
ATileGrid::ATileGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(TEXT("StaticMesh'/Game/MyBlueprints/Material/TileMesh.TileMesh'"));

	TileMesh = Mesh.Object;

	CreateTileGrid();
}



// Called when the game starts or when spawned
void ATileGrid::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATileGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATileGrid::CreateTileGrid()
{
	if (!TileMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("FAIL"))
		return;
	}

	//get dimensions of the static mesh. Returns distance from center to edge. Box size 200 gives -100 and 100.
	FBox box = TileMesh->GetBoundingBox();

	//all tiles are square so is valid
	TileDimension = box.GetExtent().X * 2;

	//setup 2D array
	TileArray = new int32 * [GridRows]; //Each element is a pointer to an undefined array

	for (int32 i = 0; i < GridRows; i++)
	{
		TileArray[i] = new int32[GridColumns]; //defined the array for each row
	}

	for (int32 r = 0; r < GridRows; r++)
	{
		for (int32 c = 0; c < GridColumns; c++)
		{
			TileArray[r][c] = 0;
		}
	}

	///TODO add player start location to array

	//fill static mesh array
	for (int32 r = 0; r < GridRows * GridColumns; r++)
	{		
			FString name = "Tile";
			name.AppendInt(r);

			//1D -> 2D works as : 1D Col/2Dcol = 2D row, 1D col % 2D col = 2D col.

			UStaticMeshComponent* Tile = NewObject<UStaticMeshComponent>(this, FName(name));
			Tile->SetStaticMesh(TileMesh);

			FVector Location = FVector(GetActorLocation().X + ((r / GridColumns) * (TileDimension + TileOffset)),
										GetActorLocation().Y + ((r % GridColumns) * (TileDimension + TileOffset)), 0);
			Tile->SetWorldLocation(Location);
					
			Tile->RegisterComponent();

			MeshArray.Add(Tile);
	}



}






