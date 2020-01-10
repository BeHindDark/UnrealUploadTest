// Fill out your copyright notice in the Description page of Project Settings.


#include "WJTBullet.h"

// Sets default values
AWJTBullet::AWJTBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));

	RootComponent = Mesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BULLET_MESH(TEXT("/Game/Mech_Constructor_Spiders/Meshes/Projectile_Bullet.Projectile_Bullet"));
	if (BULLET_MESH.Succeeded()) {
		Mesh->SetStaticMesh(BULLET_MESH.Object);
	}

	Mesh->AddRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
}

// Called when the game starts or when spawned
void AWJTBullet::BeginPlay()
{
	Super::BeginPlay();
		
	
}

// Called every frame
void AWJTBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

