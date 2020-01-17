// Fill out your copyright notice in the Description page of Project Settings.


#include "WJTProjectile_Bullet.h"

AWJTProjectile_Bullet::AWJTProjectile_Bullet() {
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MESH(TEXT("/Game/Mech_Constructor_Spiders/Meshes/Projectile_Bullet.Projectile_Bullet"));
	if (MESH.Succeeded()) {
		WeaponMesh->SetStaticMesh(MESH.Object);
	}
}