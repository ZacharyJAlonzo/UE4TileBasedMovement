// Fill out your copyright notice in the Description page of Project Settings.


#include "TileLocationComponent.h"
#include "TileSystem/Gamemodes/GameMode_C.h"
#include "TileSystem/TileStuff/InstancedTileGrid.h"



// Sets default values for this component's properties
UTileLocationComponent::UTileLocationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...

	ObjectType = EObjectIdentity::Obstacle;
}


// Called when the game starts
void UTileLocationComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UTileLocationComponent::UpdatePosition(int32 r, int32 c, AInstancedTileGrid* grid)
{
	
	grid->RequestTileInfoUpdate(GridRowPosition, GridColPosition, EObjectIdentity::FreeTile, GetOwner());

	GridRowPosition = r;
	GridColPosition = c;

	//use owner->getEObjectIdentity, which will be inherited from an actor
	grid->RequestTileInfoUpdate(GridRowPosition, GridColPosition, ObjectType, GetOwner());
}



int32 UTileLocationComponent::GetGridRowPosition()
{
	return GridRowPosition;
}

int32 UTileLocationComponent::GetGridColPosition()
{
	return GridColPosition;
}
