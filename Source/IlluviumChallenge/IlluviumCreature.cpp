// Fill out your copyright notice in the Description page of Project Settings.
#include "IlluviumCreature.h"
#include "IlluviumChallengeGameState.h"
#include "Kismet/GameplayStatics.h"
#include "IlluviumGrid.h"
#include "Net/UnrealNetwork.h"

AIlluviumCreature::AIlluviumCreature()
{
	PrimaryActorTick.bCanEverTick = false;
	// Needed so actor spawns on Clients too
	bReplicates = true;

	CreatureMesh = CreateDefaultSubobject<UStaticMeshComponent>("CreatureMeshComponent");
	SetRootComponent(CreatureMesh);
}

void AIlluviumCreature::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	
	auto gs = GetWorld()->GetGameState<AIlluviumChallengeGameState>();
	if (gs)
		gs->OnNextTimeStep.AddDynamic(this, &AIlluviumCreature::OnTimeStep);	
	else
		UE_LOG(LogTemp, Error, TEXT("IlluviumCreature: No AIlluviumChallengeGameState found"))
}

ECreatureTeam AIlluviumCreature::GetTeam()
{
	return Team;
}

void AIlluviumCreature::InitCreature(ECreatureTeam NewTeam, int RandomSeedLoc)
{
	Team = NewTeam;
	OnRep_Team(); // Calling because if its standalone, the onrep wouldnt get called
	RandomSeed = RandomSeedLoc;
	OnRep_RandomSeed();
}

void AIlluviumCreature::OnRep_Team()
{
	auto mat = Team == ECreatureTeam::TeamBlue ? BlueMaterial : RedMaterial;
	CreatureMesh->SetMaterial(0, mat);
}

void AIlluviumCreature::OnRep_RandomSeed()
{
	FMath::SRandInit(RandomSeed);
	SetGridRef();
	SetInitialLocation();
	SetAttackStrength();
}

void AIlluviumCreature::SetGridRef()
{ 
	auto a = UGameplayStatics::GetActorOfClass(GetWorld(), AIlluviumGrid::StaticClass());
	Grid = Cast<AIlluviumGrid>(a);
}

void AIlluviumCreature::SetInitialLocation()
{
	auto XCoord = FMath::RoundToInt(FMath::SRand() * Grid->GridXSize);
	auto YCoord = FMath::RoundToInt(FMath::SRand() * Grid->GridXSize);
	CurrentCoord = FVector2D(XCoord, YCoord);
	SetActorLocation(Grid->GetWorldLocationForCoord(CurrentCoord));
	//UE_LOG(LogTemp, Warning, TEXT("Initial Coord = %f %f"), CurrentCoord.X, CurrentCoord.Y)
}

void AIlluviumCreature::SetAttackStrength()
{
	auto initialAttack = FMath::SRand();
	FVector2D in (0.0f, 1.0f);
	FVector2D out(2.0f, 5.0f);
	auto mappedAttack = FMath::GetMappedRangeValueClamped(in, out, initialAttack);
	AttackStrength = FMath::RoundToInt(mappedAttack);
	//UE_LOG(LogTemp, Warning, TEXT("Attack Set: %d"), AttackStrength)
}

void AIlluviumCreature::OnTimeStep()
{
	if (IsDead)
		return;

	FindTarget();
	MoveTowardsTarget();
	Attack();
}

void AIlluviumCreature::FindTarget()
{
	if (EnemyTarget && !EnemyTarget->IsDead)
		return;

	EnemyTarget = nullptr;
	IsTargetInRange = false;

	TArray<AActor*> OtherCreatures;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AIlluviumCreature::GetClass(), OtherCreatures);

	AIlluviumCreature* ClosestCreature = nullptr;
	float MinDistance = MAX_FLT;

	// Find closest valid creature
	for (AActor* a : OtherCreatures)
	{
		auto creature = Cast<AIlluviumCreature>(a);		
		if (creature && creature != this && !creature->IsDead && creature->Team != Team)
		{
			auto dist = creature->GetActorLocation() - GetActorLocation();
			float distSquared = dist.SizeSquared();

			if (distSquared < MinDistance)
			{
				ClosestCreature = creature;
				MinDistance = distSquared;
			}
		}
	}

	EnemyTarget = ClosestCreature;
	//if(EnemyTarget)
	//	UE_LOG(LogTemp, Warning, TEXT("Enemy Target Found %s"), *EnemyTarget->GetName())
	//else
	//	UE_LOG(LogTemp, Warning, TEXT("Enemy Target NOT Found"))
}

void AIlluviumCreature::MoveTowardsTarget()
{
	if (!EnemyTarget || IsTargetInRange)
		return;

	auto EnemyCoords = EnemyTarget->CurrentCoord;
	auto CloseEnough = true;

	auto xDist = FMath::Abs(EnemyCoords.X - CurrentCoord.X);
	if (xDist > AttackDistance)
	{
		auto dir = FMath::Sign(EnemyCoords.X - CurrentCoord.X);
		auto dist = FMath::Min(xDist, MoveGridSquaresPerTimeStep);
		CurrentCoord.X += dist * dir;

		CloseEnough = false;
	}

	auto yDist = FMath::Abs(EnemyCoords.Y - CurrentCoord.Y);
	if (yDist > AttackDistance)
	{
		auto dir = FMath::Sign(EnemyCoords.Y - CurrentCoord.Y);
		auto dist = FMath::Min(yDist, MoveGridSquaresPerTimeStep);
		CurrentCoord.Y += dist * dir;

		CloseEnough = false;
	}

	IsTargetInRange = CloseEnough;

	//UE_LOG(LogTemp, Warning, TEXT("New Coord = %f %f"), CurrentCoord.X, CurrentCoord.Y)

	SetActorLocation(Grid->GetWorldLocationForCoord(CurrentCoord));
}

void AIlluviumCreature::Attack()
{
	if (!IsTargetInRange || !EnemyTarget)
		return;

	auto scale = GetActorScale();
	if (AttackCharge == -1)
		SetActorScale3D(FVector(scale.X, scale.Y, scale.Z * 3));

	AttackCharge++;

	if (AttackCharge == TimeStepsPerAttack)
	{
		UGameplayStatics::ApplyDamage(EnemyTarget, AttackStrength, nullptr, this, UDamageType::StaticClass());

		if (EnemyTarget->IsDead)
			EnemyTarget = nullptr;

		AttackCharge = -1;
		SetActorScale3D(FVector(scale.X, scale.Y, scale.Z / 3));
	}

	//UE_LOG(LogTemp, Warning, TEXT("Attacking"))
}

float AIlluviumCreature::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	CurrentHealth -= DamageAmount;
	//UE_LOG(LogTemp, Warning, TEXT("Took damage: %f"), DamageAmount)

	if (CurrentHealth <= 0)
		Die();

	return 0;
}

void AIlluviumCreature::Die()
{
	IsDead = true;
	OnCreatureDied.Broadcast(Team);
	auto scale = GetActorScale();
	SetActorScale3D(FVector(scale.X, scale.Y, 0.01));

	// Could Unsubscribe from GameState's OnNextTimeStep delegate if creature can never respawn
	// gs->OnNextTimeStep.RemoveDynamic(this, &AIlluviumCreature::OnTimeStep);
	// Or could also Destroy Creature after a delay for example

	//UE_LOG(LogTemp, Warning, TEXT("Died"))
}

void AIlluviumCreature::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AIlluviumCreature, Team);
	DOREPLIFETIME(AIlluviumCreature, RandomSeed);
}
