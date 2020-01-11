// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "WJTBullet.generated.h"

UCLASS()
class WJT_API AWJTBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWJTBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float RPM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float FinalDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Test;


};
