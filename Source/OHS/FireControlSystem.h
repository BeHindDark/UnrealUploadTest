// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OHS/OHS.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/ActorComponent.h"
#include "FireControlSystem.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnFireOrderDelegate);

/*
 *
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OHS_API UFireControlSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFireControlSystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame 
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
public:
	UPROPERTY(VisibleAnywhere, Category = FireControlSystem)
	FVector TargetLocation;

	FOnFireOrderDelegate OnFireOrder;

	TArray<class AOHSWeapon*> WeaponArray;
	
	UPROPERTY(VisibleAnywhere, Category = FireControlSystem)
	TArray<FTransform> SocketTransforms; 

	TArray<TSubclassOf<class AOHSWeapon>*> WeaponClassArray;

	//TArray<FOnFireOrderDelegate> FireOrders;
	
};