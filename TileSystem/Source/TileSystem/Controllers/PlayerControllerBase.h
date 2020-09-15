// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerBase.generated.h"

/**
 * 
 */
class ISelectableActorInterface;
class AInstancedTileGrid;

UCLASS()
class TILESYSTEM_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

private:
	
	UPROPERTY(EditAnywhere)
		float TraceLength = 1000.f;

	FHitResult TraceScreenToWorld();

	void DeselectActor();
	void SelectActor();
	
	//set in linetrace. Interface is implemented by actors
	///TODO make Interface variable once c++ is implemented for character
	AActor* ActorSelected = nullptr;

	UPrimitiveComponent* ComponentSelected = nullptr;

	AInstancedTileGrid* TileGrid = nullptr;

	//called by attack component?
	void IsSelectingAttack();



	bool bIsSelectingAttack = false;

protected:
	
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
};
