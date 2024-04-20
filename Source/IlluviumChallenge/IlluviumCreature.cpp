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

	CurrentHealth = MaxHealth;
	SetGridRef();

	// TODO
	CurrentCoord = FVector2D(FMath::RandRange(0, 100), FMath::RandRange(0, 100));
	SetActorLocation(Grid->GetWorldLocationForCoord(CurrentCoord));
	UE_LOG(LogTemp, Warning, TEXT("Initial Coord = %f %f"), CurrentCoord.X, CurrentCoord.Y)
}

void AIlluviumCreature::SetGridRef()
{ 
	auto a = UGameplayStatics::GetActorOfClass(GetWorld(), AIlluviumGrid::StaticClass());
	Grid = Cast<AIlluviumGrid>(a);
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

	auto msg = EnemyTarget ? "Found" : "Not Found";
	UE_LOG(LogTemp, Warning, TEXT("Enemy Target %s"), msg)
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

	if (FMath::Abs(EnemyCoords.X - CurrentCoord.X) > AttackDistance)
	{
		auto DirX = FMath::Sign(EnemyCoords.X - CurrentCoord.X);
		CurrentCoord.X += MoveGridSquaresPerTimeStep * DirX;

		CloseEnough = false;
	}

	if (FMath::Abs(EnemyCoords.Y - CurrentCoord.Y) > AttackDistance)
	{
		auto dir = FMath::Sign(EnemyCoords.Y - CurrentCoord.Y);
		CurrentCoord.Y += MoveGridSquaresPerTimeStep * dir;

		CloseEnough = false;
	}

	IsTargetInRange = CloseEnough;

	UE_LOG(LogTemp, Warning, TEXT("New Coord = %f %f"), CurrentCoord.X, CurrentCoord.Y)

	SetActorLocation(Grid->GetWorldLocationForCoord(CurrentCoord));
}

void AIlluviumCreature::Attack()
{
	if (!IsTargetInRange)
		return;

	UGameplayStatics::ApplyDamage(EnemyTarget, AttackStrength, nullptr, this, UDamageType::StaticClass());

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
