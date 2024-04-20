// Fill out your copyright notice in the Description page of Project Settings.
#include "IlluviumChallengeGameState.h"
#include "Kismet/KismetSystemLibrary.h"

void AIlluviumChallengeGameState::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle th;
	GetWorld()->GetTimerManager().SetTimer(th, this, &AIlluviumChallengeGameState::StartGame, SecondsBeforeStartingGame, false);
}

void AIlluviumChallengeGameState::StartGame()
{
	float IntervalInSeconds = (float)TimeStepInMS / 1000.0f;
	GetWorld()->GetTimerManager().SetTimer(TimeStepHandle, this, &AIlluviumChallengeGameState::DoTimeStep, IntervalInSeconds, true);
}

void AIlluviumChallengeGameState::DoTimeStep()
{
	CurrentTimeStep++;
	OnNextTimeStep.Broadcast();

	//UE_LOG(LogTemp, Warning, TEXT("CurrentTimeStep = %d, WorldTime = %f"), CurrentTimeStep, UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld()))
}
