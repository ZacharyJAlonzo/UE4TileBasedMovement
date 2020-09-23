// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurnManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurnDelegate);

class APlayerControllerBase;
class AEnemyAIController;
class ATileCharacterBase;

UCLASS()
class TILESYSTEM_API ATurnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurnManager();

	UFUNCTION(BlueprintCallable)
	void SetControlledUnits(TArray<ATileCharacterBase*> units);

	UFUNCTION()
		void IncrementMovedUnitCount();

	FTurnDelegate OnPlayerTurnStart;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void PlayerTurnEnd();


private:
	APlayerControllerBase* PlayerController = nullptr;
	//AEnemyAIController* EnemyController = nullptr;

	TArray<ATileCharacterBase*> ControlledUnits;

	bool bIsPlayerTurn = true;

	int32 UnitCount = 0;
	int32 MovedUnitCount = 0;

};
