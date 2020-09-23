// Fill out your copyright notice in the Description page of Project Settings.


#include "TileCharacterControllerBase.h"
#include "TileSystem/Components/TileAttackComponent.h"
#include "TileSystem/Components/TileMovementComponent.h"
#include "TileSystem/Components/TileLocationComponent.h"
#include "TileSystem/TileStuff/InstancedTileGrid.h"
#include "TileSystem/Gamemodes/GameMode_C.h"
#include "TileSystem/Interfaces/SelectableActorInterface.h"
#include "TileSystem/Actors/TileCharacterBase.h"


void ATileCharacterControllerBase::BeginPlay()
{
	Super::BeginPlay();

	AGameMode_C* mode;
	mode = Cast<AGameMode_C>(GetWorld()->GetAuthGameMode());
	TileGrid = mode->GetTileGridRef();
	//get a pointer to the tilegrid

	AttackComponent = GetPawn()->FindComponentByClass<UTileAttackComponent>();
	MovementComponent = GetPawn()->FindComponentByClass<UTileMovementComponent>();
	LocationComponent = GetPawn()->FindComponentByClass<UTileLocationComponent>();

	Char = Cast<ATileCharacterBase>(GetPawn());
}


void ATileCharacterControllerBase::SetBestAttackTileVars()
{
	//unused in this case
	TArray<int32> NoMoveTiles; 

	//the valid tiles that we are in range to move to
	TArray<int32> ValidMoves = MovementComponent->GetTilesInMoveRange(NoMoveTiles);

	//the attacks, and the range that they have
	TArray<FMoveInformation> AttackList = AttackComponent->MoveList;

		//for each attack
	for (int32 k = 0; k < AttackList.Num(); k++)
	{

		int32 LargestNumberAttacked = 0;
		TArray<int32> BestMoveOptions;
		TArray<int32> BestEpicenterOptions;

		//for every valid move tile
		for (int32 i = 0; i < ValidMoves.Num(); i++)
		{
			TArray<int32> EpicenterRange;
			//get the range of our attack at the specified tile index, ValidMove[i].
			TArray<int32> GeneralRange = AttackComponent->GetGeneralRange(ValidMoves[i], AttackList[k], EpicenterRange);
			int32 NumberAttacked = 0;

			//the functionality of bestmoveoptions is a bit different for ranged attacks.
			//we also need to keep track of the best epicenter location
			if (AttackList[k].RangeInfo.bIsRangedAttack)
			{
				//iterate through each general range tile getting the epicenter range there.
				//iterate through the epicenter range, adding up the total hit.

				for (int32 j = 0; j < GeneralRange.Num(); j++)
				{
					//set the epicenter to our desired index
					AttackComponent->DesiredAttackLocation(GeneralRange[j]);
					//update the epicenter range
					AttackComponent->GetGeneralRange(ValidMoves[i], AttackList[k], EpicenterRange);

					NumberAttacked = 0;
					//iterate over epicenter range
					for (int32 m = 0; m < EpicenterRange.Num(); m++)
					{

						FArrayObject obj;
						if (TileGrid)
						{
							obj = TileGrid->GetArrayObject(EpicenterRange[m]);
							if (obj.Identity == EObjectIdentity::Actor && obj.ObjectContained != GetPawn()) //&& actor is not friendly
							{
								NumberAttacked++;
							}
							else continue;
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("AIController had no TileGrid pointer."))
						}
					}

					//after this loop we know how many actors can be attacked at the specific index.

					//if the epicenter we checked was best
					if (NumberAttacked > LargestNumberAttacked)
					{
						BestEpicenterOptions.Empty();
						BestEpicenterOptions.Add(GeneralRange[j]);

						BestMoveOptions.Empty();
						BestMoveOptions.Add(ValidMoves[i]);

						LargestNumberAttacked = NumberAttacked;
					}
					//else if (NumberAttacked == LargestNumberAttacked)
					//{
						//BestEpicenterOptions.Add(GeneralRange[j]);
						//BestMoveOptions.Add(ValidMoves[i]);
					//}


				}


			}
			//else do the other code

			else
			{
				//THE CODE FOR NORMAL ATTACKS. BISPHYS DOES NOT MATTER SINCE IF IT IS TICKED, GET DIAGONAL / GET HORIZONTALS WILL HANDLE IT.
				//for each tile in the general range if we were to be at position ValidMoves[i]

				for (int32 j = 0; j < GeneralRange.Num(); j++)
				{

					FArrayObject obj;
					if (TileGrid)
					{
						obj = TileGrid->GetArrayObject(GeneralRange[j]);
						if (obj.Identity == EObjectIdentity::Actor && obj.ObjectContained != GetPawn()) //&& actor is not friendly
						{
							NumberAttacked++;
						}
						else continue;
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("AIController had no TileGrid pointer."))
					}
				}

				//after this loop, the number of actors attacked if we were to be on tile ValidMoves[i] is known.
				if (NumberAttacked > LargestNumberAttacked)
				{
					LargestNumberAttacked = NumberAttacked;
					//wipe the array of best options, since this is now the best option.
					BestMoveOptions.Empty();
					BestMoveOptions.Add(ValidMoves[i]);
				}
				//if it is equal, then it is just as good. make decision based on other factor in future.
				else if (NumberAttacked == LargestNumberAttacked)
				{
					//BestMoveOptions.Add(ValidMoves[i]);
				}
				
			}

			if (BestMoveOptions.IsValidIndex(0))
			{
				//BestEpicenterOptions.Add(BestMoveOptions[0]);
			}

		}



		//after this loop completes, one attack has been checked.
		//for now, I'm only going to have one attack be checked.

		//set the vars for the AI
		if (BestMoveOptions.IsValidIndex(0))
		{
			BestMoveTile = BestMoveOptions[0];
			UE_LOG(LogTemp, Warning, TEXT("The best move tile was: %i, the most attacked was: %i"), BestMoveOptions[0], LargestNumberAttacked);

		
			if (BestEpicenterOptions.IsValidIndex(0))
			{
				BestEpicenterTile = BestEpicenterOptions[0];
				AttackComponent->DesiredAttackLocation(BestEpicenterOptions[0]);
				UE_LOG(LogTemp, Warning, TEXT("The best epicenter was %i"), BestEpicenterOptions[0]);

				TArray<int32> OutTiles;
				if (AttackComponent->GetGeneralRange(AttackComponent->MoveList[0], OutTiles).Contains(BestEpicenterOptions[0]))
				{
					//show possible attacks:: delay, then attack.
					//ISelectableActorInterface::Execute_AttackSelect(GetPawn());
					Char->AttackSelect();

					FTimerHandle handle;
					GetWorld()->GetTimerManager().SetTimer(handle, this, &ATileCharacterControllerBase::AIAttackThenMove, 1.0, false);

				}
				//best attack is not in current range, move first
				else
				{
					//show posible moves:: delay, then move.
					//ISelectableActorInterface::Execute_ActorSelected(GetPawn());
					Char->ActorSelected();

					FTimerHandle handle;
					GetWorld()->GetTimerManager().SetTimer(handle, this, &ATileCharacterControllerBase::AIMoveThenAttack, 1.0, false);

				}
			}
			else
			{
				Char->ActorSelected();
		

				FTimerHandle handle;
				GetWorld()->GetTimerManager().SetTimer(handle, this, &ATileCharacterControllerBase::AIMoveThenAttack, 1.0, false);

			}

		}
		else //no move -> attack was found. move towards the closest enemy
		{
			UE_LOG(LogTemp, Warning, TEXT("No Enemy in potential move->attack range."));
			//find the closest enemy
			//select actor to show move tiles
			//delay, then move
			BestMoveTile = GetTileClosestToNearestEnemy();
			UE_LOG(LogTemp, Warning, TEXT("Best Move: %i"), BestMoveTile);

			//show posible moves:: delay, then move.
			//ISelectableActorInterface::Execute_ActorSelected(GetPawn());
			Char->ActorSelected();

			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, this, &ATileCharacterControllerBase::AIMove, 1.0, false);

		}
			

	}
	
		//pseudocode
		//
		//tile epicenter in AttackComponent starts at 0 initially.
		//
		//for each valid move tile:
		//	
		//
		//for each attack:
		//		if(is ranged attack)
		//			TileEpicenter = index;
		//			Range = GetGeneralRange(Attack #, EpicenterRange)
		//
		//			for each tile in Epicenter range:
		//				sum total enemies hit
		//				if > (greatest number of enemies hit)
		//					set greatest number hit
		//					clear best options array
		//					add new option to array
		//				else if == (greatest number hit)
		//					add to best option array
		//
		//			The best tile epicenter is found for that attack after this loop completes.
		//			
		//
		//		else
		//			Range = GetGeneralRange(Attack #, EpicenterRange)
		//			compare this to each other valid move to find the best index to stand.	
		//		

}

void ATileCharacterControllerBase::AIAttackThenMove()
{
	//ISelectableActorInterface::Execute_Attack(GetPawn());
	Char->Attack();
	Char->SetCanAttack(false);

	AIDeselect();

	//show available moves
	//ISelectableActorInterface::Execute_ActorSelected(GetPawn());
	FTimerHandle handle2;
	GetWorld()->GetTimerManager().SetTimer(handle2, this, &ATileCharacterControllerBase::AISelect, 0.75, false);

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &ATileCharacterControllerBase::AIMove, 1.5, false);

}

void ATileCharacterControllerBase::AIMoveThenAttack()
{
	AIMove();
	FTimerHandle handle2;
	GetWorld()->GetTimerManager().SetTimer(handle2, this, &ATileCharacterControllerBase::AIAttackSelect, 0.75, false);

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &ATileCharacterControllerBase::AIAttack, 1.5, false);
}

void ATileCharacterControllerBase::AIMove()
{
	UE_LOG(LogTemp, Error, TEXT("Move attempted"));
	MovementComponent->MoveTo(TileGrid->ConvertTileIndexToWorldSpace(BestMoveTile), BestMoveTile);
	//Char->SetCanMove(false);
}

void ATileCharacterControllerBase::AIAttack()
{
	//ISelectableActorInterface::Execute_Attack(GetPawn());
	UE_LOG(LogTemp, Error, TEXT("Attack attempted"));
	Char->Attack();
	Char->SetCanAttack(false);

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &ATileCharacterControllerBase::AIDeselect, 0.5, false);
}

void ATileCharacterControllerBase::AIDeselect()
{
	//ISelectableActorInterface::Execute_ActorDeselected(GetPawn());
	Char->ActorDeselected();
}

void ATileCharacterControllerBase::AISelect()
{
	//ISelectableActorInterface::Execute_ActorSelected(GetPawn());
	Char->ActorSelected();
}

void ATileCharacterControllerBase::AIAttackSelect()
{
	//ISelectableActorInterface::Execute_AttackSelect(GetPawn());
	Char->AttackSelect();
}

int32 ATileCharacterControllerBase::GetTileClosestToNearestEnemy()
{
	//find closest enemy (the first actor that is non friendly to us)
	

	//a copy of the tile grid
	int32 sizeR = TileGrid->GetGridRows();
	int32 sizeC = TileGrid->GetGridColumns();

	int32** arr = new int32 * [sizeR];
	for (int r = 0; r < sizeR; r++)
	{
		arr[r] = new int32[sizeC];
	}

	//set default values
	for (int r = 0; r < sizeR; r++)
	{
		for (int c = 0; c < sizeC; c++)
		{
			arr[r][c] = -1;
		}

	}

	arr[LocationComponent->GetGridRowPosition()][LocationComponent->GetGridColPosition()] = 0;
	bool ChangeMade = true;

	bool ClosestEnemySet = false;
	int32 ClosestEnemyTile = 0;

	int32 nextVal = 0;

	while (ChangeMade)
	{
		ChangeMade = false;

		for (int32 r = 0; r < TileGrid->GetGridRows(); r++)
		{
			for (int32 c = 0; c < TileGrid->GetGridColumns(); c++)
			{
				if (arr[r][c] == nextVal)
				{
					if (r + 1 < sizeR && arr[r + 1][c] == -1) 
                    {                         
                        //check if there is a blocking item stored in the main array.
                        if (ValidateNotObstacle(r + 1, c))
                        {                          
                            arr[r + 1][c] = nextVal + 1;
                            ChangeMade = true;
                        }
						else
						{
							arr[r + 1][c] = -(TileGrid->GetGridColumns() * TileGrid->GetGridRows());
						}

						int32 Index1D = TileGrid->Convert2DArrayPositionTo1D(r + 1, c);
						if (!ClosestEnemySet && TileGrid->GetArrayObject(Index1D).Identity == EObjectIdentity::Actor && TileGrid->GetArrayObject(Index1D).ObjectContained != GetPawn())
						{
							ClosestEnemySet = true;
							ClosestEnemyTile = Index1D;
						}
                                         
                    }
                   
                    if (r - 1 >= 0 && arr[r - 1][c] == -1)
                    {   
                    
                        if (ValidateNotObstacle(r - 1, c))
                        {
                            arr[r - 1][c] = nextVal + 1;
                            ChangeMade = true;
                        }
						else
						{
							arr[r - 1][c] = -(TileGrid->GetGridColumns() * TileGrid->GetGridRows());
						}

						int32 Index1D = TileGrid->Convert2DArrayPositionTo1D(r - 1, c);
						if (!ClosestEnemySet && TileGrid->GetArrayObject(Index1D).Identity == EObjectIdentity::Actor && TileGrid->GetArrayObject(Index1D).ObjectContained != GetPawn())
						{
							ClosestEnemySet = true;
							ClosestEnemyTile = Index1D;
						}
                                          
                    }
                    
                    if (c + 1 < sizeC && arr[r][c + 1] == -1)
                    { 
                        
                        if (ValidateNotObstacle(r, c + 1))
                        {                          
                            arr[r][c + 1] = nextVal + 1;
                            ChangeMade = true;
                        }
						else
						{
							arr[r][c + 1] = -(TileGrid->GetGridColumns() * TileGrid->GetGridRows());
						}

						int32 Index1D = TileGrid->Convert2DArrayPositionTo1D(r, c + 1);
						if (!ClosestEnemySet && TileGrid->GetArrayObject(Index1D).Identity == EObjectIdentity::Actor && TileGrid->GetArrayObject(Index1D).ObjectContained != GetPawn())
						{
							ClosestEnemySet = true;
							ClosestEnemyTile = Index1D;
						}
                   
                                              
                    }
                                  
                    if (c - 1 >= 0 && arr[r][c - 1] == -1)
                    { 
                       
                        if (ValidateNotObstacle(r, c - 1))
                        {                         
                            arr[r][c - 1] = nextVal + 1;
                            ChangeMade = true;
                        }
						else
						{
							arr[r][c - 1] = -(TileGrid->GetGridColumns() * TileGrid->GetGridRows());
						}

						int32 Index1D = TileGrid->Convert2DArrayPositionTo1D(r, c - 1);
						if (!ClosestEnemySet && TileGrid->GetArrayObject(Index1D).Identity == EObjectIdentity::Actor && TileGrid->GetArrayObject(Index1D).ObjectContained != GetPawn())
						{
							ClosestEnemySet = true;
							ClosestEnemyTile = Index1D;
						}
                 

                    }
				}
			}
		}

		nextVal++;
	}
	//after this is over, we know where to start the next pathfinding from


	//we need another copy of the array 
	sizeR = TileGrid->GetGridRows();
	sizeC = TileGrid->GetGridColumns();

	int32** arr2 = new int32 * [sizeR];
	for (int r = 0; r < sizeR; r++)
	{
		arr2[r] = new int32[sizeC];
	}

	//set default values
	for (int r = 0; r < sizeR; r++)
	{
		for (int c = 0; c < sizeC; c++)
		{
			arr2[r][c] = -1;
		}

	}

	//starting point is the enemy location
	UE_LOG(LogTemp, Error, TEXT("Closest Enemy: %i, %i"), ClosestEnemyTile / TileGrid->GetGridColumns(), ClosestEnemyTile% TileGrid->GetGridColumns());
	arr2[ClosestEnemyTile / TileGrid->GetGridColumns()][ClosestEnemyTile % TileGrid->GetGridColumns()] = 0;
	ChangeMade = true;
	nextVal = 0;

	while (ChangeMade)
	{
		ChangeMade = false;

		for (int32 r = 0; r < TileGrid->GetGridRows(); r++)
		{
			for (int32 c = 0; c < TileGrid->GetGridColumns(); c++)
			{

				if (arr2[r][c] == nextVal)
				{
					if (r + 1 < sizeR && arr2[r + 1][c] == -1)
					{
						//check if there is a blocking item stored in the main array.
						if (ValidateNotObstacle(r + 1, c))
						{
							arr2[r + 1][c] = nextVal + 1;
							ChangeMade = true;
						}
						else
						{
							arr2[r + 1][c] = -(TileGrid->GetGridColumns() * TileGrid->GetGridRows());
						}

					}

					if (r - 1 >= 0 && arr2[r - 1][c] == -1)
					{

						if (ValidateNotObstacle(r - 1, c))
						{
							arr2[r - 1][c] = nextVal + 1;
							ChangeMade = true;
						}
						else
						{
							arr2[r - 1][c] = -(TileGrid->GetGridColumns() * TileGrid->GetGridRows());
						}

					}

					if (c + 1 < sizeC && arr2[r][c + 1] == -1)
					{

						if (ValidateNotObstacle(r, c + 1))
						{
							arr2[r][c + 1] = nextVal + 1;
							ChangeMade = true;
						}
						else
						{
							arr2[r][c + 1] = -(TileGrid->GetGridColumns() * TileGrid->GetGridRows());
						}

					}
					
					if (c - 1 >= 0 && arr2[r][c - 1] == -1)
					{

						if (ValidateNotObstacle(r, c - 1))
						{
							arr2[r][c - 1] = nextVal + 1;
							ChangeMade = true;
						}
						else
						{
							arr2[r][c - 1] = -( TileGrid->GetGridColumns() * TileGrid->GetGridRows() );
						}
					}
				}


			}
		}

		nextVal++;

	}
	//reverse pathfind to get the best tile

			/*@ is the starting value of 0
			 *
			 *3 2 1 @ 1 2 3
			 *4 3 2 1 2 3 4
			 *5 4 3 2 3 4 5
			 *6 5 4 E 4 5 6
			 *
			 *E is at a distance of 3
			 *the first iteration is complete
			 *
			 *now, we start from E
			 *
			 *6 5 4 @ 4 5 6
			 *5 4 3 2 3 4 5
			 *4 3 2 1 2 3 4
			 *3 2 1 E 1 2 3
			 *
			 *now, we subtract from both of these arrays.
			 *clearly, the shortest path is
			 *   x 0 x
			 *   x 1 x
			 *   x 2 x
			 *   x 3 x
			 *
			 *and, for the reverse
			 *   x 3 x
			 *   x 2 x
			 *   x 1 x
			 *   x 0 x
			 *
			 *now, using the calculated distance, 3 in this example,
			 *we can subtract to ensure we are on the proper path.
			 *
			 *if (initial lengthMap)[r][c] == distance - (backwards lengthMap)[r][c]
			 *then we know we are on the right path.
			 *	0 == (3) - 3 (true)
			 *	1 == (3) - 2 (true)
			 *	2 == (3) - 1 (true)
			 *	3 == (3) - 0 (true)
			 *
			 *if we exclude the tiles where lengthMap is 0 (the starting tiles)
			 *we return the second case, the tile of distance 1.
			 *
			 *this is the closest tile to the enemy.
			 */

	//the distance to the closest enemy
	int32 distance = arr[ClosestEnemyTile / TileGrid->GetGridColumns()][ClosestEnemyTile % TileGrid->GetGridColumns()];

	UE_LOG(LogTemp, Warning, TEXT("Distance: %i"), distance);

	for (int32 i = 0; i < TileGrid->GetGridRows(); i++)
	{
		for (int32 k = 0; k < TileGrid->GetGridColumns(); k++)
		{
			//the backwards comparison from Roguelike code.
			//the second comparison makes sure that we are moving as far as possible towards the enemy
			if (arr[i][k] == distance - arr2[i][k] && ( distance - arr2[i][k] == MovementComponent->GetMoveRange() ) && arr[i][k] > 0 && arr2[i][k] > 0)
			{
				return TileGrid->Convert2DArrayPositionTo1D(i, k);
			}
		}
	}

	//return tile
	return -1;
}


bool ATileCharacterControllerBase::ValidateNotObstacle(int32 row, int32 col)
{
	if (TileGrid->GetArrayObject(TileGrid->Convert2DArrayPositionTo1D(row, col)).Identity != EObjectIdentity::Obstacle)
	{
		//UE_LOG(LogTemp, Error, TEXT("%i, %i was a valid move"),r ,c);
		return true;
	}

	return false;
}