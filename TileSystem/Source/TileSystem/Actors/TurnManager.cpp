// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnManager.h"
#include "TileSystem/Controllers/EnemyAIController.h"
#include "TileSystem/Controllers/PlayerControllerBase.h"
#include "TileSystem/Actors/TileCharacterBase.h"
#include "TileSystem/Controllers/TileCharacterControllerBase.h"

// Sets default values
ATurnManager::ATurnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATurnManager::SetControlledUnits(TArray<ATileCharacterBase*> units)
{
	ControlledUnits = units;

	UnitCount = units.Num();


	UE_LOG(LogTemp, Error, TEXT("Units Sett %i"), UnitCount);

	for (ATileCharacterBase* A : units)
	{
		A->OnUnitTurnComplete.AddDynamic(this, &ATurnManager::IncrementMovedUnitCount);
	}


}

void ATurnManager::IncrementMovedUnitCount()
{
	MovedUnitCount++;

	if (MovedUnitCount == UnitCount)
	{

	}
}

// Called when the game starts or when spawned
void ATurnManager::BeginPlay()
{
	Super::BeginPlay();
	//get the player controller and the enemy ai controller

	PlayerController = Cast<APlayerControllerBase>(GetWorld()->GetFirstPlayerController());

	PlayerController->OnPlayerTurnEnd.AddDynamic(this, &ATurnManager::PlayerTurnEnd);

}

// Called every frame
void ATurnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsPlayerTurn)
	{
		//run the player's turn. 
		//turn is over when they move+attack for every character

		//while they have not moved all of their units
		//continue player turn
	}

}

void ATurnManager::PlayerTurnEnd()
{
	UE_LOG(LogTemp, Error, TEXT("Delegate called"));

	//run AI turn
	for (ATileCharacterBase* A : ControlledUnits)
	{
		ATileCharacterControllerBase* Controller = Cast<ATileCharacterControllerBase>(A->GetController());

		if (Controller)
		{
			Controller->SetBestAttackTileVars();
		}

		A->SetCanMove(true);
		A->SetCanAttack(true);
	}

	OnPlayerTurnStart.Broadcast();

}


