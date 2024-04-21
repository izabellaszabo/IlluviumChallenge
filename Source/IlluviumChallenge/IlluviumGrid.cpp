// Fill out your copyright notice in the Description page of Project Settings.
#include "IlluviumGrid.h"

AIlluviumGrid::AIlluviumGrid()
{
	PrimaryActorTick.bCanEverTick = false;

	GridMesh = CreateDefaultSubobject<UStaticMeshComponent>("GridMeshComponent");
	SetRootComponent(GridMesh);
}

FVector AIlluviumGrid::GetWorldLocationForCoord(FVector2D coord)
{
	auto GridCenterOffset = GridSquareSize / 2;
	auto WorldLoc = GetActorLocation();
	WorldLoc.X += (coord.X * GridSquareSize) + GridCenterOffset;
	WorldLoc.Y += (coord.Y * GridSquareSize) + GridCenterOffset;
	WorldLoc.Z += GridHeight;
	return WorldLoc;
}
