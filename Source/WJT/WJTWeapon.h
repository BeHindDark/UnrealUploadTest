// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WJT.h"
#include "GameFramework/Actor.h"
#include "WJTWeapon.generated.h"

UCLASS()
class WJT_API AWJTWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWJTWeapon();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(VisibleAnywhere)
	int32 Damage;

	UPROPERTY(VisibleAnywhere)
	float CurrentDamage;

	UPROPERTY(VisibleAnywhere)
	int32 MaxMagazine;

	UPROPERTY(VisibleAnywhere)
	int32 CurrentMagazine;

public:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* WeaponMesh;

	void SetDamage(int32 D);

	void ShootBullet();
};
