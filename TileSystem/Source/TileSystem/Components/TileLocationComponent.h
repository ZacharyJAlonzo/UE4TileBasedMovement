// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TileLocationComponent.generated.h"


class AInstancedTileGrid;
enum EObjectIdentity : int32;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TILESYSTEM_API UTileLocationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTileLocationComponent();

	void UpdatePosition(int32 r, int32 c, AInstancedTileGrid* grid);

	int32 GetGridRowPosition();
	int32 GetGridColPosition();

	EObjectIdentity ObjectType;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


private:

	UPROPERTY(VisibleAnywhere)
		int32 GridRowPosition = 0;

	UPROPERTY(VisibleAnywhere)
		int32 GridColPosition = 0;
};
