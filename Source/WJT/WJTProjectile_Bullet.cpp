// Fill out your copyright notice in the Description page of Project Settings.


#include "WJTProjectile_Bullet.h"

//SuggetsProjectileVelocity�� PredictProjectilePath�Ἥ �ڵ�¥��

AWJTProjectile_Bullet::AWJTProjectile_Bullet() {
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PROJECTILEMESH(TEXT("/Game/Mech_Constructor_Spiders/Meshes/Projectile_Bullet.Projectile_Bullet"));
	if (PROJECTILEMESH.Succeeded()) {
		WeaponMesh->SetStaticMesh(PROJECTILEMESH.Object);
	}

	//auto Velocity = UGameplayStatics::SuggestProjectileVelocity();
	//auto Path = UGameplayStatics::PredictProjectilePath();
}


