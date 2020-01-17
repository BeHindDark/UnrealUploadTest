// Fill out your copyright notice in the Description page of Project Settings.


#include "WJTWeapon.h"

// Sets default values
AWJTWeapon::AWJTWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WEAPON_MESH"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MESH(TEXT("/Game/Mech_Constructor_Spiders/Meshes/Weapon_Shock_Rifle_Lvl5.Weapon_Shock_Rifle_Lvl5"));
	if (MESH.Succeeded()) {
		WeaponMesh->SetStaticMesh(MESH.Object);
	}
}

// Called when the game starts or when spawned
void AWJTWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWJTWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWJTWeapon::SetDamage(int32 D) {
	Damage = D;
}

