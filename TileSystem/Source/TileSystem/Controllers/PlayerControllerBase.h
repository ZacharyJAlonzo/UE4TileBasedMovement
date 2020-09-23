// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerBase.generated.h"

/**
 * 
 * 
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDelegate);

class ISelectableActorInterface;
class AInstancedTileGrid;
class ATurnManager;
class ATileCharacterBase;


UCLASS()
class TILESYSTEM_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()


public:
	//called by gamemode on begin play?
	UFUNCTION(BlueprintCallable)
	void SetControlledUnits(TArray<ATileCharacterBase*> units);

	FPlayerDelegate OnPlayerTurnEnd;

private:
	
	UPROPERTY(EditAnywhere)
		float TraceLength = 1000.f;

	ATurnManager* TurnManager = nullptr;

	FHitResult TraceScreenToWorld();

	void DeselectActor();
	void SelectActor();
	
	//set in linetrace. Interface is implemented by actors
	///TODO make Interface variable once c++ is implemented for character
	/// 
	/// 
	//AActor* ActorSelected = nullptr;

	ATileCharacterBase* ActorSelected = nullptr;

	UPrimitiveComponent* ComponentSelected = nullptr;

	AInstancedTileGrid* TileGrid = nullptr;

	//called by attack component?
	void IsSelectingAttack();
	bool bIsSelectingAttack = false;




protected:
	
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	UFUNCTION()
	void IncrementUnitsCompletedTurn();

	UFUNCTION()
	void NewTurn();

	TArray<ATileCharacterBase*> ControlledUnits;
	int32 UnitCount = 0;
	int32 MovedUnitCount = 0;
};
