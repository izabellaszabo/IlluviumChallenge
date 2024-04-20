// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "GameFramework/Actor.h"
#include "IlluviumCreature.generated.h"

class AIlluviumGrid;

UENUM(BlueprintType)
enum class ECreatureTeam :uint8
{
	TeamRed,
	TeamBlue
};

UCLASS()
class AIlluviumCreature : public AActor
{
	GENERATED_BODY()
	
public:	
	AIlluviumCreature();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UStaticMeshComponent* CreatureMesh = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere) ECreatureTeam Team = ECreatureTeam::TeamBlue;
	UPROPERTY(EditDefaultsOnly) int MaxHealth = 10;
	// Measured in Grid Squares
	UPROPERTY(EditDefaultsOnly) int AttackDistance = 2;
	// How many grid squares we can move in a time step
	UPROPERTY(EditDefaultsOnly) float MoveGridSquaresPerTimeStep = 1;

	// Unreal Engine function from AActor
	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	void SetGridRef();

	// Getter functions
	FVector2D GetCoordinates();

protected:
	virtual void BeginPlay() override;
	
	// Upropertys have to be at least protected

private:
	UFUNCTION() void OnTimeStep();

	// Target finding
	AIlluviumCreature* EnemyTarget = nullptr;
	AIlluviumGrid* Grid = nullptr;
	bool IsTargetInRange = false;
	FVector2D CurrentCoord = FVector2D::ZeroVector;
	void FindTarget();
	void MoveTowardsTarget();

	// Attack
	float AttackStrength = -1;	
	void Attack();

	// Health and Death
	int CurrentHealth = -1;
	bool IsDead = false;
	void Die();

};
