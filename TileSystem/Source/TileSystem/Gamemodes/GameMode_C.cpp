// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode_C.h"
#include "TileSystem/TileStuff/InstancedTileGrid.h"
#include "Kismet/GameplayStatics.h"
#include "TileSystem/Components/TileLocationComponent.h"

void AGameMode_C::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Grid;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInstancedTileGrid::StaticClass(), Grid);

	TileGrid = Cast<AInstancedTileGrid>(Grid[0]);

	SnapActorsToNearestTile();
}

AInstancedTileGrid* AGameMode_C::GetTileGridRef()
{
	return TileGrid;
}

void AGameMode_C::SnapActorsToNearestTile()
{
	TArray<AActor*> List;
	//get all actors with interface tilegridactor
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), List);

	for (AActor* A : List)
	{
		UTileLocationComponent* component = A->FindComponentByClass<UTileLocationComponent>();
		if (component)
		{
			int32 index = TileGrid->ConvertWorldSpaceToTileIndex(A->GetActorLocation());
			int32 row = index / TileGrid->GetGridColumns();
			int32 col = index % TileGrid->GetGridColumns();

			UE_LOG(LogTemp, Warning, TEXT("%i, %i"), row, col);

			component->UpdatePosition(row, col, TileGrid);
			
			FVector loc = TileGrid->ConvertTileIndexToWorldSpace(index);
			A->SetActorLocation(loc);
		}
	}
	
}
