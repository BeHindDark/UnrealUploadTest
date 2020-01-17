// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OHS/OHS.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/Character.h"
#include "TPSCharacter.generated.h"

UCLASS()
class OHS_API ATPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PostInitializeComponents() override;

	virtual void PossessedBy(AController* NewController) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Ä¿½ºÅÒ º¯¼ö
public:
	// public variables

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* TPCamera;

	UPROPERTY(VisibleAnywhere, Category = CollisionBox)
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere, Category = Controller)
	APlayerController* PlayerController;

	UPROPERTY(VisibleAnywhere, Category = FireControlSystem)
	class UFireControlSystem* FireControlSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	float MoveInput = 0.0f;

protected:
	UPROPERTY(EditAnywhere, Category = Camera, Meta = (AllowPrivateAccess = true))
	float CameraYawSpeed;

	UPROPERTY(EditAnywhere, Category = Camera, Meta = (AllowPrivateAccess = true))
	float CameraPitchSpeed;

	UPROPERTY(EditAnywhere,Category = Movement,Meta = (AllowPrivateAccess = true))
	float BodyYawSpeed = 0.3;

	UPROPERTY(EditAnywhere, Category = FireControlSystem, Meta = (AllowPrivateAccess = true))
	float AimingRange;

	UPROPERTY(VisibleAnywhere, Category = FireControlSystem, Meta = (AllowPrivateAccess = true))
	FVector AimingLocation;

	TArray<const USkeletalMeshSocket*> SocketArray;

private:
	float CameraYawMovement;
	float CameraPitchMovement;
	bool bIsPlayerControlling;


	//custom functions

protected:
	FVector GetCameraAimLocation(UCameraComponent* CurrentCamera);

private:
	//private custom functions

	//player Axis input functions
	
	//mouse Y : TPCamera Peach Controll
	void LookUp(float NewAxisValue);

	//mouse X : TPCamera Yaw Controll
	void TurnCamera(float NewAxisValue);
	
	//WS : MoveForward or Backward
	void MoveFrontBack(float NewAxisValue);

	//AD : Move Right or Left
	void MoveRightLeft(float NewAxisValue);

	//QE : Character Yaw Controll
	void TurnCharacter(float NewAxisValue);

	//player Action input functions

	//LMB down : Fire
	void StartFire();

	//LMB release : stopFire
	void StopFire();
};