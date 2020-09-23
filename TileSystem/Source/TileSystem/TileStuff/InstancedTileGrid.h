// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InstancedTileGrid.generated.h"

UENUM()
enum EObjectIdentity
{
	FreeTile UMETA(DisplayName = "FreeTile"),
	Obstacle UMETA(DisplayName = "Obstacle"),
	Actor UMETA(DisplayName = "Actor"),
	FAIL UMETA(DisplayName = "FAIL")
};

USTRUCT()
struct FArrayObject
{
	GENERATED_BODY()

public:
	EObjectIdentity Identity;
	AActor* ObjectContained;


};

USTRUCT()
struct FArrayColumns
{
	GENERATED_BODY()

public:
	TArray<FArrayObject> Cols;

	FArrayObject operator[] (int32 i) 
	{
		return Cols[i];
	}

	void Add(FArrayObject element) 
	{
		Cols.Add(element);
	}

};



UCLASS()
class TILESYSTEM_API AInstancedTileGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInstancedTileGrid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	int32 GetGridRows() const;

	UFUNCTION(BlueprintCallable)
	int32 GetGridColumns() const;	

	UFUNCTION(BlueprintCallable)
	int32 GetTileOffset() const;

	UFUNCTION(BlueprintCallable)
	int32 GetTileDimension() const;

	EObjectIdentity GetArrayValue(int32 r, int32 c);
	FArrayObject GetArrayObject(int32 index);


	//REPLACING WITH
	UPROPERTY()
	TArray<FArrayColumns> TileArray2;

	

	int32 Convert2DArrayPositionTo1D(int32 R, int32 C);
	int32 ConvertWorldSpaceToTileIndex(FVector Location);
	FVector ConvertTileIndexToWorldSpace(int32 Index);

	void RequestTileInfoUpdate(int32 onRow, int32 onCol, EObjectIdentity contained, AActor* object = nullptr);


private:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions", meta = (AllowPrivateAccess = "true"))
		int32 GridRows = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions", meta = (AllowPrivateAccess = "true"))
		int32 GridColumns = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions", meta = (AllowPrivateAccess = "true"))
		int32 TileOffset = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		UInstancedStaticMeshComponent* TileGrid = nullptr;

	//The Static Mesh size
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimensions", meta = (AllowPrivateAccess = "true"))
	int32 TileDimension;

protected:
	//creates the grid. Static mesh is set in BP.
	UFUNCTION(BlueprintCallable)
		void CreateGrid();

	UFUNCTION(BlueprintCallable)
	void CreateGridArray();
	
};
