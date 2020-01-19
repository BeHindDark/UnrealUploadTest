// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WJTWeapon.h"
#include "WJTWeapon_Instance.generated.h"

/**
 * 
 */
UCLASS()
class WJT_API AWJTWeapon_Instance : public AWJTWeapon
{
	GENERATED_BODY()

	//원본은 ShooterWeapon하나만 참조
	
protected:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* InstanceBullet;
private:


public:

	AWJTWeapon_Instance();


};
