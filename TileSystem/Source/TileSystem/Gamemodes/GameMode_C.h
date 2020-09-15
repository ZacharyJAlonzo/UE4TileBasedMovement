// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameMode_C.generated.h"

/**
 * 
 */

class AInstancedTileGrid;

UCLASS()
class TILESYSTEM_API AGameMode_C : public AGameModeBase
{
	GENERATED_BODY()

private:
	AInstancedTileGrid* TileGrid = nullptr;

protected:
	virtual void BeginPlay() override;

	

public:
	AInstancedTileGrid* GetTileGridRef();

	UFUNCTION(BlueprintCallable)
	void SnapActorsToNearestTile();

};
