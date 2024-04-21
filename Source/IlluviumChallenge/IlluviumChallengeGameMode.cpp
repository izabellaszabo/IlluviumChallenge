#include "IlluviumChallengeGameMode.h"
#include "IlluviumChallengeGameState.h"

void AIlluviumChallengeGameMode::BeginPlay()
{
	if (!CreatureBlueprint)
	{
		UE_LOG(LogTemp, Error, TEXT("BPCreatureClass has not been set in the IlluviumChallangeGameMode blueprint!"))
		return;
	}

	if (NumberOfCreaturesToSpawn == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No creatures will be spawned by IlluviumChallangeGameMode"))
		return;
	}

	for (int i = 0; i < NumberOfCreaturesToSpawn; i++)
	{
		AIlluviumCreature* creature = GetWorld()->SpawnActor<AIlluviumCreature>(CreatureBlueprint, FVector::ZeroVector, FRotator::ZeroRotator);
		auto team = i % 2 == 0 ? ECreatureTeam::TeamBlue : ECreatureTeam::TeamRed;
		if (team == ECreatureTeam::TeamBlue)
			LivingBlues++;
		else
			LivingReds++;
		creature->InitCreature(team, i);
		creature->OnCreatureDied.AddDynamic(this, &AIlluviumChallengeGameMode::OnCreatureDied);
	}
}

void AIlluviumChallengeGameMode::OnCreatureDied(ECreatureTeam Team)
{
	if (Team == ECreatureTeam::TeamBlue)
		LivingBlues--;
	else
		LivingReds--;

	if (LivingBlues == 0 || LivingReds == 0)
	{
		FString winner = LivingBlues == 0 ? "Red" : "Blue";

		auto gs = GetWorld()->GetGameState<AIlluviumChallengeGameState>();
		if (gs)
			gs->Multicast_StopGame(winner);
	}
}
