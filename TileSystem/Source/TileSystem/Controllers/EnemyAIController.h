// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class TILESYSTEM_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	void AddToEnemyArray(AActor* Enemy);
	void TakeTurn();

protected:
	virtual void BeginPlay() override;

private:

	TArray<AActor*> EnemyArray;
	/*
	The enemy AI controller is the 'opponent' who manages enemy turns.
	it has an array of all enemy actors, and calls the SetBestAttackTileVars method for each to get them to move.

	when it has called all of the actors, the turn ends.
	*/
	
};
