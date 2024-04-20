// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "GameFramework/GameStateBase.h"
#include "IlluviumChallengeGameState.generated.h"

/**
 * 
 */
UCLASS()
class AIlluviumChallengeGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	// How often should a TimeStep happend
	UPROPERTY(EditDefaultsOnly) int TimeStepInMS = 100;
	// How long to let the game startup before starting gameplay
	UPROPERTY(EditDefaultsOnly) float SecondsBeforeStartingGame = 3;

protected:
	void BeginPlay() override;

private:
	// Starts the TimeSteps
	UFUNCTION() void StartGame();

	FTimerHandle TimeStepHandle;
	UFUNCTION() void DoTimeStep();

	// Might need to use a bigger type, like long if game can run for longer periods of time
	int CurrentTimeStep = 0;
};
