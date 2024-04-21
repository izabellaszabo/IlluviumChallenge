#include "IlluviumChallengeGameMode.h"
#include "IlluviumCreature.h"

void AIlluviumChallengeGameMode::BeginPlay()
{
	if (!BPCreatureClass)
	{
		UE_LOG(LogTemp, Error, TEXT("BPCreatureClass has not been set in the IlluviumChallangeGameMode blueprint!"))
			return;
	}

	for (int i = 0; i < NumberOfCreaturesToSpawn; i++)
	{
		AIlluviumCreature* creature = GetWorld()->SpawnActor<AIlluviumCreature>(BPCreatureClass, FVector::ZeroVector, FRotator::ZeroRotator);
		auto team = i % 2 == 0 ? ECreatureTeam::TeamBlue : ECreatureTeam::TeamRed;
		creature->InitCreature(team, i);
	}
}