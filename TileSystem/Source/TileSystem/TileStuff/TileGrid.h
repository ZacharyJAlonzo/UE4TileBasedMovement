// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileGrid.generated.h"

UCLASS()
class TILESYSTEM_API ATileGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileGrid();

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	//called by TileMovementComponent. will be called after a Move?
	void RequestTileUpdate(int32 fromRow, int32 fromCol, int32 newRow, int32 newCol);

private:

	UPROPERTY(EditAnywhere, Category = "Grid Dimension")
	int32 GridRows = 5;

	UPROPERTY(EditAnywhere, Category = "Grid Dimension")
	int32 GridColumns = 5;

	//distance between tiles
	UPROPERTY(EditAnywhere, Category = "Grid Dimension")
	int32 TileOffset = 10;

	//the mesh for each tile. must be a square.
	//UPROPERTY(EditAnywhere, Category = "Grid Mesh")
	UPROPERTY()
	UStaticMesh* TileMesh;

	//this 2D array represents the physical grid
	//0 is a Tile that is open. 1 Represents an obstacle. 2 represents an AI actor.
	//Pathfinding and Move distance will be calculated here, and Mesh array materials will be altered accordingly.
	int32** TileArray;

	//The meshes that store vector location and allow material manipulation
	//1D array of length r*c in end
	UPROPERTY()
	TArray<UStaticMeshComponent*> MeshArray;


	float TileDimension = 0.f;

	//create the grid of static mesh components
	UFUNCTION(BlueprintCallable)
	void CreateTileGrid();

};
