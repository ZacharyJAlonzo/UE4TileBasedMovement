// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllerBase.h"
#include "DrawDebugHelpers.h"
#include "TileSystem/Interfaces/SelectableActorInterface.h"
#include "TileSystem/TileStuff/InstancedTileGrid.h"
#include "Kismet/GameplayStatics.h"
#include "TileSystem/Components/TileMovementComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "TileSystem/Gamemodes/GameMode_C.h"
#include "GameFramework/Character.h"
#include "AIController.h"
#include "TileSystem/Components/TileAttackComponent.h"


void APlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();

	//probably not this function. Call a buffer function that will call TraceScreen with an out parameter FHitresult
	//function call also has to call the 'SelectableActor' interface method 'ActorSelected'
	InputComponent->BindAction("SelectActor", IE_Pressed, this, &APlayerControllerBase::SelectActor);
	
	InputComponent->BindAction("DeselectActor", IE_Pressed, this, &APlayerControllerBase::DeselectActor);
	//Bind action for deselect as well. Similar requirements.

	//change input bind to selecting attack vs specific attack
	InputComponent->BindAction("Attack1", IE_Pressed, this, &APlayerControllerBase::IsSelectingAttack);
}

void APlayerControllerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsSelectingAttack)
	{
		//raycast, pass tile to attack component, update displayed tiles in char BP.
		FHitResult Hit = TraceScreenToWorld();

		//if we hit something.
		if (Hit.GetActor())
		{
			int32 index = TileGrid->ConvertWorldSpaceToTileIndex(Hit.ImpactPoint);

			//our selected actor is not nullptr
			if (ActorSelected)
			{
				UTileAttackComponent* Component = ActorSelected->FindComponentByClass<UTileAttackComponent>();

				//the Actor has an attack component
				if (Component)
				{
					UE_LOG(LogTemp, Warning, TEXT("EPICENTER: %i"), index);
					//used in ranged attacks. needs to be set even if unused.
					Component->DesiredAttackLocation(index);
					ISelectableActorInterface::Execute_AttackSelect(ActorSelected);

					
				}
			}
		}
	

	}
}

void APlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	//find a pointer to the tile grid. Maybe from Gamemode BP?
	AGameMode_C* mode;
	mode = Cast<AGameMode_C>(GetWorld()->GetAuthGameMode());

	TileGrid = mode->GetTileGridRef();
}

void APlayerControllerBase::IsSelectingAttack()
{
	if (bIsSelectingAttack)
	{

		if (ActorSelected)
		{
			ISelectableActorInterface::Execute_ActorDeselected(ActorSelected);
			ISelectableActorInterface::Execute_ActorSelected(ActorSelected);
		}
		
	}

	bIsSelectingAttack = !bIsSelectingAttack;
}

void APlayerControllerBase::DeselectActor()
{
	//also stop selecting attack tile

	if (ActorSelected)
	{
		ISelectableActorInterface::Execute_ActorDeselected(ActorSelected);
	}
	
	ActorSelected = nullptr;	
}

void APlayerControllerBase::SelectActor()
{
	FHitResult Hit = TraceScreenToWorld();
	//if(! selecting an attack tile)
	//var set by a button press/UI clicked?
	//
	if (bIsSelectingAttack)
	{
		if (ActorSelected)
		{
			ISelectableActorInterface::Execute_Attack(ActorSelected);
		
			IsSelectingAttack();
			DeselectActor();
			return;
		}

	}

	//if an actor is already selected, see if we are pressing a tile in move range
	if (ActorSelected && !bIsSelectingAttack)
	{
		int32 index = TileGrid->ConvertWorldSpaceToTileIndex(Hit.ImpactPoint);
	
		//loc is the position to navigate to.
		FVector loc = TileGrid->ConvertTileIndexToWorldSpace(index);

		UTileMovementComponent* Component = ActorSelected->FindComponentByClass<UTileMovementComponent>();

		if (Component)
		{
			ISelectableActorInterface::Execute_ActorDeselected(ActorSelected);
			Component->MoveTo(loc, index);
			ISelectableActorInterface::Execute_ActorSelected(ActorSelected);
		}
	}

	if (!Hit.GetActor())
	{
		return;
	}

	if (Hit.GetActor()->GetClass()->ImplementsInterface(USelectableActorInterface::StaticClass()))
	{
		if (ActorSelected)
		{
			UE_LOG(LogTemp, Warning, TEXT("Actor was selected, but new actor is chosen"))
			DeselectActor();
		}

		//cast to 'SelectableActor' interface 
		ActorSelected = Hit.GetActor();
		ISelectableActorInterface::Execute_ActorSelected(Hit.GetActor());

		UE_LOG(LogTemp, Warning, TEXT("ActorSelected Call"))
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast Fail"))
	}

}

//REFACTOR THIS INTO SMALLER PIECES
FHitResult APlayerControllerBase::TraceScreenToWorld()
{

	//for now, hardcode the center of the viewport
	int32 ViewportX, ViewportY;
	GetViewportSize(ViewportX, ViewportY);

	//multiply by 0.5 to find halfway point
	float ScreenX = 0.5 * ViewportX;
	float ScreenY = 0.5 * ViewportY;

	FVector WorldPosition;
	FVector WorldDirection;
	if (DeprojectScreenPositionToWorld(ScreenX, ScreenY, WorldPosition, WorldDirection))
	{

		//do actual line trace and note hit result
		//tiles will be hit by this trace, however only 'SelectableActors' will be selected.
		//an 'ActorSelected' var will be updated. (in future used to show UI info?)
		FVector Start = PlayerCameraManager->GetCameraLocation();
		FVector End = WorldPosition + (WorldDirection * TraceLength);

		FHitResult Hit;
		FCollisionQueryParams QueryParams = FCollisionQueryParams(FName(""), false, GetPawn());
		GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams);


		return Hit;
		//SelectActor(Hit);	
	}

	return FHitResult();
}