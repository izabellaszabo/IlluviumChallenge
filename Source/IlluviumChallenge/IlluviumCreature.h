// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "GameFramework/Actor.h"
#include "IlluviumCreature.generated.h"

class AIlluviumGrid;

UENUM(BlueprintType)
enum class ECreatureTeam :uint8
{
	TeamRed,
	TeamBlue,
	None
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCreatureDied, ECreatureTeam, TeamParam);

/**
* An Illuvium creature finds an enemy creature, moves towards it on the IlluviumGrid, and attacks its target until it or itself dies.
*/
UCLASS()
class AIlluviumCreature : public AActor
{
	GENERATED_BODY()
	
public:	
	AIlluviumCreature();
	void InitCreature(ECreatureTeam NewTeam, int RandomSeed);
	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UStaticMeshComponent* CreatureMesh = nullptr;
	UPROPERTY(EditDefaultsOnly) UMaterialInstance* RedMaterial = nullptr;
	UPROPERTY(EditDefaultsOnly) UMaterialInstance* BlueMaterial = nullptr;

	// Measured in Grid Squares
	UPROPERTY(EditDefaultsOnly) int AttackDistance = 2;
	UPROPERTY(EditDefaultsOnly) int TimeStepsPerAttack = 3;
	UPROPERTY(EditDefaultsOnly) int MaxHealth = 10;
	UPROPERTY(EditDefaultsOnly) float MoveGridSquaresPerTimeStep = 1;

	// Delegate
	FCreatureDied OnCreatureDied;
	ECreatureTeam GetTeam();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_Team) ECreatureTeam Team = ECreatureTeam::None;
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_RandomSeed) int RandomSeed = -1;
	UFUNCTION() void OnTimeStep();

	// Initial setup functions
	UFUNCTION() void OnRep_Team();
	UFUNCTION() void OnRep_RandomSeed();
	void SetInitialLocation();
	void SetGridRef();
	void SetAttackStrength();

	// Target finding
	AIlluviumCreature* EnemyTarget = nullptr;
	AIlluviumGrid* Grid = nullptr;
	FVector2D CurrentCoord = FVector2D::ZeroVector;
	bool IsTargetInRange = false;
	void FindTarget();
	void MoveTowardsTarget();

	// Attack
	int AttackStrength = -1;
	int AttackCharge = -1;
	void Attack();

	// Health and death
	int CurrentHealth = -1;
	bool IsDead = false;
	void Die();
};
