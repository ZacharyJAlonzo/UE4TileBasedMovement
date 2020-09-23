// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TileSystem/Interfaces/SelectableActorInterface.h"
#include "TileCharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnitTurnComplete);

class UTileLocationComponent;
class UTileMovementComponent;
class UTileAttackComponent;

UCLASS()
class TILESYSTEM_API ATileCharacterBase : public ACharacter//, public ISelectableActorInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATileCharacterBase();

	FUnitTurnComplete OnUnitTurnComplete;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool CanMove = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool CanAttack = true;

	void SetCanMove(bool move);
	void SetCanAttack(bool atk);

	//set by gamemode?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPlayerControlled = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UTileLocationComponent* LocationComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UTileMovementComponent* MovementComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UTileAttackComponent* AttackComponent = nullptr;


	UFUNCTION(BlueprintImplementableEvent, meta = (Blueprintable))
		void ActorSelected();

	UFUNCTION(BlueprintImplementableEvent, meta = (Blueprintable))
		void ActorDeselected();

	UFUNCTION(BlueprintImplementableEvent, meta = (Blueprintable))
		void AttackSelect();

	UFUNCTION(BlueprintImplementableEvent, meta = (Blueprintable))
		void Attack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
