// Fill out your copyright notice in the Description page of Project Settings.


#include "FireControlSystem.h"
#include "OHSWeapon.h"

// Sets default values for this component's properties
UFireControlSystem::UFireControlSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
  TargetLocation = FVector::ZeroVector;
  BodyRotation = FRotator::ZeroRotator;
	// ...
}


// Called when the game starts
void UFireControlSystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UFireControlSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

