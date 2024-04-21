// Fill out your copyright notice in the Description page of Project Settings.
#include "IlluviumCreature.h"
#include "IlluviumChallengeGameState.h"
#include "Kismet/GameplayStatics.h"
#include "IlluviumGrid.h"

AIlluviumCreature::AIlluviumCreature()
{
	PrimaryActorTick.bCanEverTick = false;

	CreatureMesh = CreateDefaultSubobject<UStaticMeshComponent>("CreatureMeshComponent");
	SetRootComponent(CreatureMesh);
}

void AIlluviumCreature::BeginPlay()
{
	Super::BeginPlay();
	
	auto gs = GetWorld()->GetGameState<AIlluviumChallengeGameState>();
	if (gs)
		gs->OnNextTimeStep.AddDynamic(this, &AIlluviumCreature::OnTimeStep);	
	else
		UE_LOG(LogTemp, Error, TEXT("IlluviumCreature: No AIlluviumChallengeGameState found"))
}

void AIlluviumCreature::InitCreature(ECreatureTeam NewTeam, int RandomSeed)
{
	CurrentHealth = MaxHealth;
	SetGridRef();
	SetTeam(NewTeam);
	SetRandomSeed(RandomSeed);
	SetInitialLocation();
	SetAttackStrength();
}

void AIlluviumCreature::SetGridRef()
{ 
	auto a = UGameplayStatics::GetActorOfClass(GetWorld(), AIlluviumGrid::StaticClass());
	Grid = Cast<AIlluviumGrid>(a);
}

void AIlluviumCreature::SetTeam(ECreatureTeam NewTeam)
{
	Team = NewTeam;
	auto mat = Team == ECreatureTeam::TeamBlue ? BlueMaterial : RedMaterial;
	CreatureMesh->SetMaterial(0, mat);
}

void AIlluviumCreature::SetRandomSeed(int Seed)
{
	FMath::SRandInit(Seed);
}

void AIlluviumCreature::SetInitialLocation()
{
	auto XCoord = FMath::RoundToInt(FMath::SRand() * Grid->GridXSize);
	auto YCoord = FMath::RoundToInt(FMath::SRand() * Grid->GridXSize);
	CurrentCoord = FVector2D(XCoord, YCoord);
	SetActorLocation(Grid->GetWorldLocationForCoord(CurrentCoord));
	UE_LOG(LogTemp, Warning, TEXT("Initial Coord = %f %f"), CurrentCoord.X, CurrentCoord.Y)
}

void AIlluviumCreature::SetAttackStrength()
{
	auto initialAttack = FMath::SRand();
	FVector2D in (0.0f, 1.0f);
	FVector2D out(2.0f, 5.0f);
	auto mappedAttack = FMath::GetMappedRangeValueClamped(in, out, initialAttack);
	AttackStrength = FMath::RoundToInt(mappedAttack);
	UE_LOG(LogTemp, Warning, TEXT("Attack Set: %d"), AttackStrength)
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
	if(EnemyTarget)
		UE_LOG(LogTemp, Warning, TEXT("Enemy Target Found"))
	else
		UE_LOG(LogTemp, Warning, TEXT("Enemy Target NOT Found"))
}

FVector2D AIlluviumCreature::GetCoordinates()
{
	return CurrentCoord;
}

void AIlluviumCreature::MoveTowardsTarget()
{
	if (!EnemyTarget || IsTargetInRange)
		return;

	auto EnemyCoords = EnemyTarget->GetCoordinates();
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

	UE_LOG(LogTemp, Warning, TEXT("New Coord = %f %f"), CurrentCoord.X, CurrentCoord.Y)

	SetActorLocation(Grid->GetWorldLocationForCoord(CurrentCoord));
}

void AIlluviumCreature::Attack()
{
	if (!IsTargetInRange || !EnemyTarget)
		return;

	UGameplayStatics::ApplyDamage(EnemyTarget, AttackStrength, nullptr, this, UDamageType::StaticClass());

	if (EnemyTarget->IsDead)
		EnemyTarget = nullptr;

	UE_LOG(LogTemp, Warning, TEXT("Attacking"))
}

float AIlluviumCreature::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	CurrentHealth -= DamageAmount;
	UE_LOG(LogTemp, Warning, TEXT("Took damage: %f"), DamageAmount)
	
	if (CurrentHealth <= 0)
		Die();

	return 0;
}

void AIlluviumCreature::Die()
{
	IsDead = true;
	SetCanBeDamaged(false);

	// Could Unsubscribe from GameState's OnNextTimeStep delegate if creature can never respawn
	// gs->OnNextTimeStep.RemoveDynamic(this, &AIlluviumCreature::OnTimeStep);
	// Or could also Destroy Creature after a delay for example

	UE_LOG(LogTemp, Warning, TEXT("Died"))
}
