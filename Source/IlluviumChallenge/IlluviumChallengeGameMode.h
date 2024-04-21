#pragma once

#include "GameFramework/GameModeBase.h"
#include "IlluviumCreature.h"
#include "IlluviumChallengeGameMode.generated.h"

/**
* The gamemode's main function is to spawn the required number of IlluviumCreatures. 
*/
UCLASS(minimalapi)
class AIlluviumChallengeGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Setup") int NumberOfCreaturesToSpawn = 10;
	UPROPERTY(EditDefaultsOnly, Category = "Setup") TSubclassOf<AIlluviumCreature> CreatureBlueprint = nullptr;

protected:
	void BeginPlay() override;

private:
	// Number of creatures on each team that are still alive
	int LivingReds = 0;
	int LivingBlues = 0;

	UFUNCTION() void OnCreatureDied(ECreatureTeam Team);

};
