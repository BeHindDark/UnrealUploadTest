// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OHS/OHS.h"
#include "GameFramework/Actor.h"
#include "OHSWeapon.generated.h"

UCLASS()
class OHS_API AOHSWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOHSWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	USceneComponent* Sceneroot;

	UPROPERTY(VisibleAnywhere, Category = FireControl)
	FVector TargetLocation;

	UPROPERTY(VisibleAnywhere, Category = FireControl)
	bool bLocked;

	UPROPERTY(VisibleAnywhere, Category = FireControl)
	bool bEquality;

	int WeaponIndex = -1;

protected:
	UPROPERTY(EditAnywhere, Category = Weapon)
	float YawRotationSpeed = 60.0f;

	UPROPERTY(EditAnywhere, Category = Weapon)
	float PitchRotationSpeed = 30.0f;

	UPROPERTY(EditAnywhere, Category = Weapon)
	float YawRotationLimit[2];

	UPROPERTY(EditAnywhere, Category = Weapon)
	float PitchRotationLimit[2];

	class UFireControlSystem* FireControlSystem;
	

public:
	void ConnectFireControlSystem(class UFireControlSystem* NewFireControlSystem);

protected:
	void TurnTowardDirectAim(float DeltaTime);

private:
	void OnFireOrder();
};