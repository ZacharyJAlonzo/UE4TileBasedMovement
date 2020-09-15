// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SelectableActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USelectableActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TILESYSTEM_API ISelectableActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//blueprintimplementableevents cannot be virtual.
	//for now, I will work in blueprint. I will redo it in C++ after functionality is added

	UFUNCTION(BlueprintImplementableEvent, meta = (Blueprintable))
	void ActorSelected();

	UFUNCTION(BlueprintImplementableEvent, meta = (Blueprintable))
	void ActorDeselected();

	UFUNCTION(BlueprintImplementableEvent, meta = (Blueprintable))
	void AttackSelect();

	UFUNCTION(BlueprintImplementableEvent, meta = (Blueprintable))
	void Attack();


};
