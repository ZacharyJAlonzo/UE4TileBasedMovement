// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

void AEnemyAIController::TakeTurn()
{
	//call setbestmove on all actors
}

void AEnemyAIController::BeginPlay()
{
}

void AEnemyAIController::AddToEnemyArray(AActor* Enemy)
{
	EnemyArray.Add(Enemy);
}
