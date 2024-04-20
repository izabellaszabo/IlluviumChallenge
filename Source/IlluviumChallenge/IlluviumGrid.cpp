// Fill out your copyright notice in the Description page of Project Settings.
#include "IlluviumGrid.h"

AIlluviumGrid::AIlluviumGrid()
{
	PrimaryActorTick.bCanEverTick = false;

	GridMesh = CreateDefaultSubobject<UStaticMeshComponent>("GridMeshComponent");
	SetRootComponent(GridMesh);

	GridMesh->SetWorldScale3D(FVector(GridXScale, GridYScale, 1));
}

FVector AIlluviumGrid::GetWorldLocationForCoord(FVector2D coord)
{
	auto WorldLoc = GetActorLocation();
	WorldLoc.X += coord.X * GridSquareSize;
	WorldLoc.Y += coord.Y * GridSquareSize;
	WorldLoc.Z += GridHeight;
	return WorldLoc;
}
