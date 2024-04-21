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

	UE_LOG(LogTemp, Warning, TEXT("Starting Game"))
}

void AIlluviumChallengeGameState::DoTimeStep()
{
	OnNextTimeStep.Broadcast();
}

void AIlluviumChallengeGameState::Multicast_StopGame_Implementation(const FString& Team)
{
	TimeStepHandle.Invalidate();
	UE_LOG(LogTemp, Warning, TEXT(" %s team WINS!"), *Team)
}
