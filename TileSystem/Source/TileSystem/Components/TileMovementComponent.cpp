// Fill out your copyright notice in the Description page of Project Settings.


#include "TileMovementComponent.h"
#include "TileSystem/TileStuff/InstancedTileGrid.h"
#include "Engine/World.h"
#include "TileSystem/Gamemodes/GameMode_C.h"
#include "AIController.h"
#include "TileSystem/Components/TileLocationComponent.h"

// Sets default values for this component's properties
UTileMovementComponent::UTileMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

    MoveTileMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Move Tile Mesh"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MoveMesh(TEXT("StaticMesh'/Game/MyBlueprints/Material/MoveTile.MoveTile'"));
    MoveTileMesh->SetStaticMesh(MoveMesh.Object);

    NoMoveTileMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("No Move Tile Mesh"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> NoMoveMesh(TEXT("StaticMesh'/Game/MyBlueprints/Material/NoMoveTile.NoMoveTile'"));
    NoMoveTileMesh->SetStaticMesh(NoMoveMesh.Object);

}


// Called when the game starts
void UTileMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	//find a pointer to the tile grid. Maybe from Gamemode BP?
	AGameMode_C* mode;
	mode = Cast<AGameMode_C>(GetWorld()->GetAuthGameMode());	
	TileGrid = mode->GetTileGridRef();

    LocationComponent = GetOwner()->FindComponentByClass<UTileLocationComponent>(); 
    LocationComponent->ObjectType = EObjectIdentity::Actor;

    //update enum state
    LocationComponent->UpdatePosition(LocationComponent->GetGridRowPosition(), LocationComponent->GetGridColPosition(), TileGrid);
}

FVector UTileMovementComponent::ConvertTileIndexToLocation(int32 Index)
{
    //UE_LOG(LogTemp, Error, TEXT("CAll"))
    return TileGrid->ConvertTileIndexToWorldSpace(Index);
}


// Called every frame
void UTileMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UInstancedStaticMeshComponent* UTileMovementComponent::GetMoveTileMesh()
{
    return MoveTileMesh;
}

UInstancedStaticMeshComponent* UTileMovementComponent::GetNoMoveTileMesh()
{
    return NoMoveTileMesh;
}

int32 UTileMovementComponent::GetGridRows()
{
    return TileGrid->GetGridRows();
}

int32 UTileMovementComponent::GetGridColumns()
{
    return TileGrid->GetGridColumns();
}

int32 UTileMovementComponent::GetTileOffset()
{
    return TileGrid->GetTileOffset();
}

int32 UTileMovementComponent::GetTileDimension()
{
    return TileGrid->GetTileDimension();
}

//optional out parameter
TArray<int32> UTileMovementComponent::GetTilesInMoveRange(TArray<int32>& OutNoMoveTiles)
{
	TArray<int32> Tiles;

  
   //a diagonal move should count as 2 moves: forward->right etc.
    //algorithm to find distance to every tile may be better here.

    // 3 2 1 2 3
    // 2 1 0 1 2  etc.
    // 3 2 1 2 3

    //the calculated bounds can be used to not check the entire array.

    //potential moves can then be validated to determine if they contain an obstacle/other character
    //as determined by what is stored in the corresponding index of TileArray.

    //initialize array
    int32 sizeR = TileGrid->GetGridRows();
    int32 sizeC = TileGrid->GetGridColumns();

    int32** arr = new int32*[sizeR];
    for (int r = 0; r < sizeR; r++)
    {
        arr[r] = new int32[sizeC];
    }

    //set default values
    for (int r = 0; r < sizeR; r++)
    {
        for (int c = 0; c < sizeC; c++)
        {
            arr[r][c] = -1;
        }

    }

    //This really shouldnt be very performance heavy, even if it looks bad. 
 
    //pathfind
    TArray<int32> validMoves;

    arr[LocationComponent->GetGridRowPosition()][LocationComponent->GetGridColPosition()] = 0;
    int32 nextVal = 0;
    bool ChangeMade = true;
    while (ChangeMade && !(nextVal + 1 > MoveDistance))
    {
        ChangeMade = false;

        for (int r = 0; r < sizeR; r++)
        {
            for (int c = 0; c < sizeC; c++)
            {
                if (arr[r][c] == nextVal)
                {
                    if (r + 1 < sizeR && arr[r + 1][c] == -1) 
                    {                         
                        //check if there is a blocking item stored in the main array.
                        if (ValidatePotentialMove(r + 1, c))
                        {
                            validMoves.Add(TileGrid->Convert2DArrayPositionTo1D(r + 1, c));
                            arr[r + 1][c] = nextVal + 1;
                            ChangeMade = true;
                        }
                        else
                        {
                            //tiles beyond this wont be checked.
                            OutNoMoveTiles.Add(TileGrid->Convert2DArrayPositionTo1D(r + 1, c));
                            arr[r + 1][c] = -2;
                        }
                       
                    }
                   
                    if (r - 1 >= 0 && arr[r - 1][c] == -1)
                    {   
                    
                        if (ValidatePotentialMove(r - 1, c))
                        {
                            validMoves.Add(TileGrid->Convert2DArrayPositionTo1D(r - 1, c));
                            arr[r - 1][c] = nextVal + 1;
                            ChangeMade = true;
                        }
                        else
                        {
                            //tiles beyond this wont be checked.
                            OutNoMoveTiles.Add(TileGrid->Convert2DArrayPositionTo1D(r - 1, c));
                            arr[r - 1][c] = -2;
                        }
                        
                    }
                    
                    if (c + 1 < sizeC && arr[r][c + 1] == -1)
                    { 
                        
                        if (ValidatePotentialMove(r, c + 1))
                        {
                            validMoves.Add(TileGrid->Convert2DArrayPositionTo1D(r, c + 1));
                            arr[r][c + 1] = nextVal + 1;
                            ChangeMade = true;
                        }
                        else
                        {
                            //tiles beyond this wont be checked.
                            OutNoMoveTiles.Add(TileGrid->Convert2DArrayPositionTo1D(r, c + 1));
                            arr[r][c + 1] = -2;
                        }
                                              
                    }
                                  
                    if (c - 1 >= 0 && arr[r][c - 1] == -1)
                    { 
                       
                        if (ValidatePotentialMove(r, c - 1))
                        {
                            validMoves.Add(TileGrid->Convert2DArrayPositionTo1D(r, c - 1));
                            arr[r][c - 1] = nextVal + 1;
                            ChangeMade = true;
                        }
                        else
                        {
                            //tiles beyond this wont be checked.
                            OutNoMoveTiles.Add(TileGrid->Convert2DArrayPositionTo1D(r, c - 1));
                            arr[r][c - 1] = -2;                          
                        }

                    }
                }
            }
        }

        nextVal++;
    }

    //clearn up arr pointers

    for (int32 i = 0; i < sizeR; i++)
        delete[] arr[i];
    delete[] arr;

    arr = nullptr;

   
    return validMoves;
	
}

bool UTileMovementComponent::ValidatePotentialMove(int32 r, int32 c)
{
    if (TileGrid->GetArrayValue(r, c) == EObjectIdentity::FreeTile)
    {
        //UE_LOG(LogTemp, Error, TEXT("%i, %i was a valid move"),r ,c);
        return true;
    }

    //TODO add another instanced static mesh, using red tiles to show places that cannot be moved to.
    //UE_LOG(LogTemp, Warning, TEXT("%i, %i was NOT valid move"), r,c)
    return false;
}

void UTileMovementComponent::MoveTo(FVector loc, int32 index)
{
    AAIController* Controller = Cast<AAIController>(Cast<APawn>(GetOwner())->GetController());

    TArray<int32> NoMoveArray;
    if (GetTilesInMoveRange(NoMoveArray).Contains(index))
    {

        if (Controller)
        {
            //for when I implement the char as a c++ class
            MoveTileMesh->ClearInstances();
            NoMoveTileMesh->ClearInstances();

            //update position call
            LocationComponent->UpdatePosition(index / TileGrid->GetGridColumns(), index % TileGrid->GetGridColumns(), TileGrid);
        
            //THIS SHOULD BE ITERATIVE. MOVE FROM TILE TO TILE, NOT PATHFINDING DIAGONALS.
            Controller->MoveToLocation(loc);         
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failure"))
                return;
        }
    }
}

