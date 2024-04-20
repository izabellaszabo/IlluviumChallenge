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

	// A note on the GridScales: Tha material on the mesh has 10 smaller grid squares per big square.
	// Setting a scale of 10 for example will result in 10*10 small squares as in 100 small squares in a direction. 
	// The creature has also been adjusted to this scale. I quite liked the scale and feel of these settings so I am keeping them for now.
	UPROPERTY(EditDefaultsOnly) int GridXScale = 10;
	UPROPERTY(EditDefaultsOnly) int GridYScale = 10;
	// The size of a single grid square in unreal units - I have manually set this to match up with the size of the grid mesh on the material with scale (10, 10, 1)
	UPROPERTY(EditDefaultsOnly) int GridSquareSize = 10;
	UPROPERTY(EditDefaultsOnly) int GridHeight = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UStaticMeshComponent* GridMesh = nullptr;

	FVector GetWorldLocationForCoord(FVector2D coord);
	
};
