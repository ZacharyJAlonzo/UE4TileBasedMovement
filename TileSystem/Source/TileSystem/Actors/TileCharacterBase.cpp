// Fill out your copyright notice in the Description page of Project Settings.


#include "TileCharacterBase.h"

#include "TileSystem/Components/TileLocationComponent.h"
#include "TileSystem/Components/TileMovementComponent.h"
#include "TileSystem/Components/TileAttackComponent.h"

// Sets default values
ATileCharacterBase::ATileCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LocationComponent = CreateDefaultSubobject<UTileLocationComponent>(FName("Location Component"));
	MovementComponent = CreateDefaultSubobject<UTileMovementComponent>(FName("Movement Component"));
	AttackComponent = CreateDefaultSubobject<UTileAttackComponent>(FName("Attack Component"));

}

void ATileCharacterBase::SetCanMove(bool move)
{
	CanMove = move;

	if (!CanMove && !CanAttack)
	{
		OnUnitTurnComplete.Broadcast();
	}
}

void ATileCharacterBase::SetCanAttack(bool atk)
{
	CanAttack = atk;

	if (!CanMove && !CanAttack)
	{
		OnUnitTurnComplete.Broadcast();
	}
}


// Called when the game starts or when spawned
void ATileCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATileCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATileCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

