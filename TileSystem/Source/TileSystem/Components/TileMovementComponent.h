// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TileMovementComponent.generated.h"

class AInstancedTileGrid;
class UTileLocationComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TILESYSTEM_API UTileMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTileMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	UInstancedStaticMeshComponent* GetMoveTileMesh();

	UFUNCTION(BlueprintCallable)
	UInstancedStaticMeshComponent* GetNoMoveTileMesh();


	UFUNCTION(BlueprintCallable)
	void MoveTo(FVector loc, int32 index);

private:

	UPROPERTY(EditAnywhere)
		int32 MoveDistance = 2;

	UPROPERTY()
	UInstancedStaticMeshComponent* MoveTileMesh;

	UPROPERTY()
	UInstancedStaticMeshComponent* NoMoveTileMesh;

	UPROPERTY()
	UTileLocationComponent* LocationComponent = nullptr;

	//set this on begin play by determining actor position
	UPROPERTY(VisibleAnywhere)
	int32 GridRowPosition = 0;

	UPROPERTY(VisibleAnywhere)
	int32 GridColPosition = 0;

	AInstancedTileGrid* TileGrid = nullptr;

	//IMPLEMENT THESE FUNCTIONS
	//WHEN C++ CHARACTER IS IMPLEMENTED, THEY WONT BE NEEDED
	UFUNCTION(BlueprintCallable)
		int32 GetGridRows();

	UFUNCTION(BlueprintCallable)
		int32 GetGridColumns();

	UFUNCTION(BlueprintCallable)
		int32 GetTileOffset();

	UFUNCTION(BlueprintCallable)
		int32 GetTileDimension();

	UFUNCTION(BlueprintCallable)
	FVector ConvertTileIndexToLocation(int32 Index);

	//returns tiles in a 1D system.
	UFUNCTION(BlueprintCallable)
	TArray<int32> GetTilesInMoveRange(TArray<int32>& OutNoMoveTiles);

	//take a 2D index and validate its array contents in the TileGrid
	bool ValidatePotentialMove(int32 r, int32 c);
	
};
