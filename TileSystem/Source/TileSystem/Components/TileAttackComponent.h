// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TileAttackComponent.generated.h"


USTRUCT(BlueprintType)
struct FRangeData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 HorizontalRange = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 VerticalRange = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 DiagonalRange = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 EpicenterHorizontal = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 EpicenterVertical = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 EpicenterDiagonal = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 GeneralRange = 0;


	bool bDidAttack = false;

	//should a general pathfinding range be applied, instead of a combination of the three?
	//If ticked, only general range field will be used.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bUseGeneralRange = false;

	//Should blocking obstacles affect the range of this move?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsPhysicalAttack = false;

	//should this attack create an epicenter at the desired attack location?
	//if ticked, the attack will have a range of HDV and use epicenter values.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsRangedAttack = false;

};


USTRUCT(BlueprintType)
struct FMoveInformation
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRangeData RangeInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 RawDamage = 0;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UParticleSystem* AttackParticle;

};


class AInstancedTileGrid;
class UTileLocationComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TILESYSTEM_API UTileAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTileAttackComponent();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FMoveInformation> MoveList;

	//pathfind similar to the movement component to return only valid attack squares.
	UFUNCTION(BlueprintCallable)
	TArray<int32> GetValidAttackTiles(int32 index, TArray<int32>& OutEpicenterTiles);

	UFUNCTION(BlueprintCallable)
		void DesiredAttackLocation(int32 index);

	UPROPERTY()
		UInstancedStaticMeshComponent* AttackTileMesh = nullptr;

	UPROPERTY()
		UInstancedStaticMeshComponent* EpicenterMesh = nullptr;

	UFUNCTION(BlueprintCallable)
	UInstancedStaticMeshComponent* GetAttackTileMesh();


	UFUNCTION(BlueprintCallable)
		UInstancedStaticMeshComponent* GetEpicenterTileMesh();

	TArray<int32> GetGeneralRange(FMoveInformation Move, TArray<int32>& OutEpicenterTiles);
	TArray<int32> GetGeneralRange(int32 index, FMoveInformation Move, TArray<int32>& OutEpicenterTiles);

private:	
	UTileLocationComponent* LocationComponent = nullptr;
	AInstancedTileGrid* TileGrid = nullptr;

	int32 TileEpicenter = 0;

	//helpers for GetValidAttackTiles
	TArray<int32> GetDiagonals(FMoveInformation Move, bool bUseEpicenter);
	TArray<int32> GetHorizontals(FMoveInformation Move, bool bUseEpicenter);

	TArray<int32> GetDiagonals(int32 index, FMoveInformation Move, bool bUseEpicenter);
	TArray<int32> GetHorizontals(int32 index, FMoveInformation Move, bool bUseEpicenter);

	//placeholder with similar functionality.
	bool ValidatePotentialMove(int32 r, int32 c);

		
};
