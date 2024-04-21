// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "GameFramework/GameStateBase.h"
#include "IlluviumChallengeGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNextTimeStep);

/**
 * The gamestate's main function is the TimeStep timer that runs at the required interval. 
 * It broadcasts the next TimeStep to any actors who are subscribed to the delegate.
 */
UCLASS()
class AIlluviumChallengeGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	// How often should a TimeStep happend
	UPROPERTY(EditDefaultsOnly, Category = "Setup") int TimeStepInMS = 100;
	// How long to let the game startup before starting gameplay
	UPROPERTY(EditDefaultsOnly, Category = "Setup") float SecondsBeforeStartingGame = 3;

	// Delegate
	FNextTimeStep OnNextTimeStep;

	UFUNCTION(NetMulticast, Reliable) void Multicast_StopGame(const FString& Team);

protected:
	void BeginPlay() override;

private:
	void StartGame();

	FTimerHandle TimeStepHandle;
	UFUNCTION() void DoTimeStep();
};
