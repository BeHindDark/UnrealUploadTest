// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WJTWeapon.h"
#include "WJTWeapon_Projectile.generated.h"

/**
 * 
 */
UCLASS()
class WJT_API AWJTWeapon_Projectile : public AWJTWeapon
{
	GENERATED_BODY()

	//원본은 ShooterWeapon하나만 참조를 하고 ShooterProjectile을 추가로 상속
	//즉 해당 클래스는 무기에 관한 클래스이고, 상속되는 ShooterProjectile은 탄환에 관한 클래스임.

public:


private:


protected:

	
};
