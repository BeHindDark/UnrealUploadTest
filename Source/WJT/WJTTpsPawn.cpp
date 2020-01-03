// Fill out your copyright notice in the Description page of Project Settings.


#include "WJTTpsPawn.h"

// Sets default values
AWJTTpsPawn::AWJTTpsPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWJTTpsPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWJTTpsPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AWJTTpsPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

