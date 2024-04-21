#pragma once

#include "GameFramework/GameModeBase.h"
#include "IlluviumChallengeGameMode.generated.h"

class AIlluviumCreature;

UCLASS(minimalapi)
class AIlluviumChallengeGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly) int NumberOfCreaturesToSpawn = 10;
	UPROPERTY(EditDefaultsOnly) TSubclassOf<AIlluviumCreature> BPCreatureClass = nullptr;

protected:
	void BeginPlay() override;

};
