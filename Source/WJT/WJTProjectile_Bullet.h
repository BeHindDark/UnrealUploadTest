// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WJTWeapon_Projectile.h"
#include "WJTProjectile_Bullet.generated.h"

/**
 * 
 */
UCLASS()
class WJT_API AWJTProjectile_Bullet : public AWJTWeapon_Projectile
{
	GENERATED_BODY()

public:

	AWJTProjectile_Bullet();

private:
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BulletMesh;

	
};
