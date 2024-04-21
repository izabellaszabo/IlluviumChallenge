// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "IlluviumGrid.generated.h"

/* 
* This is the grid the Creatures move on. 
*/
UCLASS()
class AIlluviumGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	AIlluviumGrid();

	UPROPERTY(EditDefaultsOnly) int GridXSize = 100;
	UPROPERTY(EditDefaultsOnly) int GridYSize = 100;
	// The size of a single grid square in unreal units - I have manually set this to match up with the size of the grid mesh on the material with scale (10, 10, 1)
	UPROPERTY(EditDefaultsOnly) int GridSquareSize = 10;
	UPROPERTY(EditDefaultsOnly) int GridHeight = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UStaticMeshComponent* GridMesh = nullptr;

	FVector GetWorldLocationForCoord(FVector2D coord);
	
};
