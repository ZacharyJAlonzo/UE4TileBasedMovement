// Fill out your copyright notice in the Description page of Project Settings.


#include "InstancedTileGrid.h"
#include "Kismet/GameplayStatics.h"
#include "TileSystem/Components/TileLocationComponent.h"

// Sets default values
AInstancedTileGrid::AInstancedTileGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TileGrid = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Instanced Mesh"));
	//allows for movement of (0,0) point
	SetRootComponent(TileGrid);	

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(TEXT("StaticMesh'/Game/MyBlueprints/Material/TileMesh.TileMesh'"));

	TileGrid->SetStaticMesh(Mesh.Object);
}



int32 AInstancedTileGrid::ConvertWorldSpaceToTileIndex(FVector Location)
{
	float col = 0.0f;
	float row = 0.0f;

	//RECALL THAT IN GENERAL: X = Columns, Y = Rows
	Location.X > GetActorLocation().X ? 
		col = FMath::Abs(GetActorLocation().X - Location.X) / (TileDimension + TileOffset) : 
		col = FMath::Abs(Location.X - GetActorLocation().X) / (TileDimension + TileOffset);

	Location.Y > GetActorLocation().Y ?
		row = FMath::Abs(GetActorLocation().Y - Location.Y) / (TileDimension + TileOffset) :
		row = FMath::Abs(Location.Y - GetActorLocation().Y) / (TileDimension + TileOffset);

	return Convert2DArrayPositionTo1D(FMath::RoundToInt(row), FMath::RoundToInt(col));
}

FVector AInstancedTileGrid::ConvertTileIndexToWorldSpace(int32 Index)
{
	FTransform Transform;
	if (TileGrid->GetInstanceTransform(Index, Transform, true))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Instance: %i, Loc: %s"), Index, *Transform.GetLocation().ToString())
		return Transform.GetLocation();
		
	}

	else return FVector::ZeroVector;
}

//This method is called by actors in their begin play. This actor initializes everything to free space on construction.
//
void AInstancedTileGrid::RequestTileInfoUpdate(int32 onRow, int32 onCol, EObjectIdentity contained, AActor* object)
{
	//UE_LOG(LogTemp, Error, TEXT("Update requested on: %i, %i"), onRow, onCol);
	//place new element at index

	if (TileArray2.IsValidIndex(onRow) && TileArray2[onRow].Cols.IsValidIndex(onCol))
	{
		FArrayObject obj;
		obj.Identity = contained;
		obj.ObjectContained = object;

		TileArray2[onRow].Cols.EmplaceAt(onCol, obj);
	}
		
	if (TileArray2.IsValidIndex(onRow) && TileArray2[onRow].Cols.IsValidIndex(onCol + 1))
	{
		TileArray2[onRow].Cols.RemoveAt(onCol + 1);
	}
		
	
}

int32 AInstancedTileGrid::Convert2DArrayPositionTo1D(int32 R, int32 C)
{
	//As a side note for later
	//1D -> 2D works as : 1D Col/2Dcol = 2D row, 1D col % 2D col = 2D col.
	//FVector Location = FVector(GetActorLocation().X + ((r / GridColumns) * (TileDimension + TileOffset)),
	//GetActorLocation().Y + ((r % GridColumns) * (TileDimension + TileOffset)), 0);
	
	return (R * GridColumns) + C;
}


int32 AInstancedTileGrid::GetGridRows() const
{
	return GridRows;
}

int32 AInstancedTileGrid::GetGridColumns() const
{
	return GridColumns;
}

int32 AInstancedTileGrid::GetTileOffset() const
{
	return TileOffset;
}

int32 AInstancedTileGrid::GetTileDimension() const
{
	return TileDimension;
}

EObjectIdentity AInstancedTileGrid::GetArrayValue(int32 r, int32 c)
{
	if (TileArray2.IsValidIndex(r))
	{
		if (TileArray2[r].Cols.IsValidIndex(c))
		{
			//UE_LOG(LogTemp, Warning, TEXT("%i, %i was value: %i"), r, c, TileArray2[r][c]);
			return TileArray2[r][c].Identity;
		}
	}

	return EObjectIdentity::FAIL;
}

FArrayObject AInstancedTileGrid::GetArrayObject(int32 index)
{
	if (TileArray2.IsValidIndex(index / GridColumns))
	{
		if (TileArray2[index / GridColumns].Cols.IsValidIndex(index % GridColumns))
		{
			//UE_LOG(LogTemp, Warning, TEXT("%i, %i was value: %i"), r, c, TileArray2[r][c]);
			return TileArray2[index / GridColumns][index % GridColumns];
		}
	}

	FArrayObject obj;
	obj.Identity = EObjectIdentity::FAIL;

	return obj;
}

// Called when the game starts or when spawned
void AInstancedTileGrid::BeginPlay()
{
	Super::BeginPlay();

	if (!TileGrid->GetStaticMesh())
	{
		return;
	}

	CreateGridArray();

	//get dimensions of the static mesh. Returns distance from center to edge. Box size 200 gives -100 and 100.
	FBox box = TileGrid->GetStaticMesh()->GetBoundingBox();

	//all tiles are square so is valid
	TileDimension = box.GetExtent().X * 2;
}

void AInstancedTileGrid::PostEditChangeProperty(FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);
	
	//CreateGrid();
	CreateGridArray();
}

// ? Do I need to tick?
void AInstancedTileGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInstancedTileGrid::CreateGridArray()
{
	//setup 2D array
	if (GridRows <= 0)
		GridRows = 1;
	if (GridColumns <= 0)
		GridColumns = 1;

	for (int i = 0; i < GridRows; i++)
	{
		TileArray2.Add(FArrayColumns());
	}

	for (int32 r = 0; r < GridRows; r++)
	{
		for (int32 c = 0; c < GridColumns; c++)
		{
			FArrayObject obj;
			obj.Identity = EObjectIdentity::FreeTile;
			obj.ObjectContained = nullptr;

			TileArray2[r].Cols.Add(obj);
		}
	}

}

void AInstancedTileGrid::CreateGrid()
{
	if (TileGrid)
	{
		TileGrid->ClearInstances();
	}

	if (!TileGrid->GetStaticMesh())
	{
		return;
	}

	//get dimensions of the static mesh. Returns distance from center to edge. Box size 200 gives -100 and 100.
	FBox box = TileGrid->GetStaticMesh()->GetBoundingBox();

	//all tiles are square so is valid
	TileDimension = box.GetExtent().X * 2;

	//create instances of static mesh
	for (int32 r = 0; r < GridRows * GridColumns; r++)
	{		
		//1D -> 2D works as : 1D Col/2Dcol = 2D row, 1D col % 2D col = 2D col.

		//The reason Y is first is because in general: Y is row, and X is column. This keeps the R,C notation consistent,
		//and allows the Instance numbering to match the 1D<->2D array conversions.

		FVector Location = FVector(GetActorLocation().Y + ((r % GridColumns) * (TileDimension + TileOffset)), 
									GetActorLocation().X + ((r / GridColumns) * (TileDimension + TileOffset)), 0);
	
		FTransform Transform = FTransform(Location);

		TileGrid->AddInstance(Transform);
	}

}

