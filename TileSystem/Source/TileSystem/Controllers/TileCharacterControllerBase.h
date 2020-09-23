// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TileCharacterControllerBase.generated.h"

/**
 * 
 */

class AInstancedTileGrid;
class UTileAttackComponent;
class UTileMovementComponent;
class UTileLocationComponent;
class ATileCharacterBase;

UCLASS()
class TILESYSTEM_API ATileCharacterControllerBase : public AAIController
{
	GENERATED_BODY()

		//called by player controller on tile select
public:


	/*
	
	What the AI needs to do on a turn:

	Find a tile that will allow for the most targets hit given his attack set

	if this tile is not in range, move closer to this tile.
	
	
	*/
	AInstancedTileGrid* TileGrid;
	UTileAttackComponent* AttackComponent;
	UTileMovementComponent* MovementComponent;
	UTileLocationComponent* LocationComponent;

	int32 BestEpicenterTile = 0;
	int32 BestMoveTile = 0;


	virtual void BeginPlay() override;

	//sets BestEpicenterTile and BestMoveTile
	void SetBestAttackTileVars();

	void AIAttackThenMove();
	void AIMoveThenAttack();
	void AIMove();
	void AIAttack();
	void AIDeselect();
	void AISelect();
	void AIAttackSelect();

	int32 GetTileClosestToNearestEnemy();
	bool ValidateNotObstacle(int32 row, int32 col);

	ATileCharacterBase* Char = nullptr;
};
