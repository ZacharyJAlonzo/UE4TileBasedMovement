// Fill out your copyright notice in the Description page of Project Settings.


#include "TileAttackComponent.h"
#include "TileSystem/Gamemodes/GameMode_C.h"
#include "TileSystem/TileStuff/InstancedTileGrid.h"
#include "TileSystem/Components/TileLocationComponent.h"
#include "Particles/ParticleSystem.h"


// Sets default values for this component's properties
UTileAttackComponent::UTileAttackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

    AttackTileMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Attack Tile Mesh"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> AttackMesh(TEXT("StaticMesh'/Game/MyBlueprints/Material/AttackRangeTile.AttackRangeTile'"));
    AttackTileMesh->SetStaticMesh(AttackMesh.Object);

    EpicenterMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Epicenter Mesh"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Epicenter(TEXT("StaticMesh'/Game/MyBlueprints/Material/NoMoveTile.NoMoveTile'"));
    EpicenterMesh->SetStaticMesh(Epicenter.Object);

	// ...
}


// Called when the game starts
void UTileAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	AGameMode_C* mode;
	mode = Cast<AGameMode_C>(GetWorld()->GetAuthGameMode());
	TileGrid = mode->GetTileGridRef();
	//get a pointer to the tilegrid

    LocationComponent = GetOwner()->FindComponentByClass<UTileLocationComponent>();

}


// Called every frame
void UTileAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


TArray<int32> UTileAttackComponent::GetValidAttackTiles(int32 index, TArray<int32>& OutEpicenterTiles)
{
    FMoveInformation Move;
    if (MoveList.IsValidIndex(index))
    {
        Move = MoveList[index];
    }
    else return TArray<int32>();

	TArray<int32> tiles;
	if (TileGrid)
	{

		//the same as movement pathfinding, but include diagonals on first pass. Blocking tiles limit range.
		//makes a square area
		//if physical is false, return will block actors, but include all tiles in range.
		tiles = GetGeneralRange(Move, OutEpicenterTiles);	
		
		
		return tiles;
	}

	return tiles;
}


void UTileAttackComponent::DesiredAttackLocation(int32 index)
{
    
    TileEpicenter = index;
}

UInstancedStaticMeshComponent* UTileAttackComponent::GetAttackTileMesh()
{
    return AttackTileMesh;
}

UInstancedStaticMeshComponent* UTileAttackComponent::GetEpicenterTileMesh()
{
    return EpicenterMesh;
}




TArray<int32> UTileAttackComponent::GetDiagonals(FMoveInformation Move, bool bUseEpicenter)
{
    //TODO Setup blocking hits for actors when IsPhysical is ticked

    TArray<int32> validMoves;
    //gets all valid tiles in horizontal range
    
    int32 rowLoc = LocationComponent->GetGridRowPosition();
    int32 colLoc = LocationComponent->GetGridColPosition();
    int32 range = Move.RangeInfo.DiagonalRange;

    //this will only be true if the epicenter is within range. No need to protect.
    if (bUseEpicenter)
    {
        rowLoc = TileEpicenter / TileGrid->GetGridColumns();
        colLoc = TileEpicenter % TileGrid->GetGridColumns();
        range = Move.RangeInfo.EpicenterDiagonal;
    }

    for (int32 i = 0; i < range; i++)
    {
     
        if (ValidatePotentialMove(rowLoc + i, colLoc + i))
        {
            validMoves.Add(TileGrid->Convert2DArrayPositionTo1D(rowLoc + i, colLoc + i));
        }
        if (ValidatePotentialMove(rowLoc + i, colLoc - i))
        {
            validMoves.Add(TileGrid->Convert2DArrayPositionTo1D(rowLoc + i, colLoc - i));
        }

        if (ValidatePotentialMove(rowLoc - i, colLoc + i))
        {
            validMoves.Add(TileGrid->Convert2DArrayPositionTo1D(rowLoc - i, colLoc + i));
        }
        if (ValidatePotentialMove(rowLoc - i, colLoc - i))
        {
            validMoves.Add(TileGrid->Convert2DArrayPositionTo1D(rowLoc - i, colLoc - i));
        }

    }

    return validMoves;
}

//should have an out param for epicenter values
TArray<int32> UTileAttackComponent::GetHorizontals(FMoveInformation Move, bool bUseEpicenter)
{
    //TODO Setup blocking hits when isPhysical is ticked

    //gets all valid tiles in horizontal range

    int32 rowLoc = LocationComponent->GetGridRowPosition();
    int32 colLoc = LocationComponent->GetGridColPosition();
    int32 range = Move.RangeInfo.HorizontalRange;

    //this will only be true if the epicenter is within range. No need to protect.
    if (bUseEpicenter)
    {
        rowLoc = TileEpicenter / TileGrid->GetGridColumns();
        colLoc = TileEpicenter % TileGrid->GetGridColumns();
        range = Move.RangeInfo.EpicenterHorizontal;
    }

     TArray<int32> validMoves;  
     //gets all valid tiles in horizontal range
     for (int32 i = 0; i < range; i++)
     {
         //rows
         if (ValidatePotentialMove(rowLoc + i, colLoc))
         {
             validMoves.Add(TileGrid->Convert2DArrayPositionTo1D(rowLoc + i, colLoc));
         }
         if (ValidatePotentialMove(rowLoc - i, colLoc))
         {
             validMoves.Add(TileGrid->Convert2DArrayPositionTo1D(rowLoc - i, colLoc));
         }

         //cols
         if (ValidatePotentialMove(rowLoc, colLoc + i))
         {
             validMoves.Add(TileGrid->Convert2DArrayPositionTo1D(rowLoc, colLoc + i));
         }
         if (ValidatePotentialMove(rowLoc, colLoc - i))
         {
             validMoves.Add(TileGrid->Convert2DArrayPositionTo1D(rowLoc, colLoc - i));
         }

     }

    return validMoves;
}



TArray<int32> UTileAttackComponent::GetGeneralRange(FMoveInformation Move, TArray<int32>& OutEpicenterTiles)
{
    TArray<int32> H = GetHorizontals(Move, false);
    TArray<int32> D = GetDiagonals(Move, false);

    //use epicenter values
    if (Move.RangeInfo.bIsRangedAttack)
    {
        //combine the arrays
        H.Append(D);

        TArray<int32> EpicenterRange;
        if (H.Contains(TileEpicenter))
        {
            TArray<int32> HE = GetHorizontals(Move, true);
            TArray<int32> DE = GetDiagonals(Move, true);

            HE.Append(DE);

            OutEpicenterTiles = HE;         
        }

    }
  
    H.Append(D);

    return H;
   
}


//if a free tile, or an actor is contained. placeholder for now.
bool UTileAttackComponent::ValidatePotentialMove(int32 r, int32 c)
{
    //UE_LOG(LogTemp, Error, TEXT("%i, %i was %i"), r, c, TileGrid->GetArrayValue(r, c));
    if (TileGrid->GetArrayValue(r, c) == EObjectIdentity::FreeTile || TileGrid->GetArrayValue(r, c) == EObjectIdentity::Actor)
    {
        return true;
    }

    return false;
}




