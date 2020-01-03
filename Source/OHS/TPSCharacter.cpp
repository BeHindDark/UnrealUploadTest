// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSCharacter.h"

// Sets default values
ATPSCharacter::ATPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
  
  //컴포넌트 세팅
  SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
  TPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
  BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BOXCOLLISION"));

  //계층구조
  SpringArm->SetupAttachment(GetCapsuleComponent());
  TPCamera->SetupAttachment(SpringArm);
  BoxCollision->SetupAttachment(GetCapsuleComponent());

  //위치 및 크기 조정
  GetMesh()->SetRelativeLocationAndRotation(FVector(50.0f, 0.0f, -120.0f), FRotator(0.0f, -90.0f, 0.0f));
  SpringArm->TargetArmLength = 800.0f;
  SpringArm->SocketOffset = FVector(0.0f, 0.0f, 300.0f);
  BoxCollision->SetRelativeLocation(FVector(-10.0f, 0.0f, 30.0f));
  BoxCollision->SetBoxExtent(FVector(150.0f, 150.0f, 50.0f));
  CameraYawSpeed = 3.0f;
  CameraPitchSpeed = 3.0f;

  //Load SkeletalMesh
  static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SpiderMid(TEXT("/Game/Mech_Constructor_Spiders/Meshes_Skeletal/Legs_Spiders/Legs_Spider_Med.Legs_Spider_Med"));
  if(SK_SpiderMid.Succeeded())
  {
    GetMesh()->SetSkeletalMesh(SK_SpiderMid.Object);
  }

  //Load AnimationBlueprint

}

// Called when the game starts or when spawned
void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

  //control
  if(CameraPitchMovement!=0.0f)
  {
    TPCamera->AddRelativeRotation(FRotator(CameraPitchMovement*DeltaTime, 0.0f, 0.0f));
  }

  if(CameraYawMovement!=0.0f)
  {
    SpringArm->AddRelativeRotation(FRotator(0.0f, CameraYawMovement*DeltaTime, 0.0f));
  }
  
}

// Called to bind functionality to input
void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
  
  //Axis Binding
  PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATPSCharacter::TurnCamera);
  PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATPSCharacter::LookUp);
  PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATPSCharacter::MoveFrontBack);
  PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ATPSCharacter::MoveRightLeft);
  PlayerInputComponent->BindAxis(TEXT("TurnBody"), this, &ATPSCharacter::TurnCharacter);
}

//mouse Y : TPCamera Peach Controll
void ATPSCharacter::LookUp(float NewAxisValue)
{
  if(FMath::IsNearlyEqual(NewAxisValue,0.0f))
  {
    CameraPitchMovement = 0.0f;
  }
  else
  {
    CameraPitchMovement = CameraPitchSpeed*NewAxisValue;
  }
}

//mouse X : TPCamera Yaw Controll
void ATPSCharacter::TurnCamera(float NewAxisValue)
{
  if(FMath::IsNearlyEqual(NewAxisValue,0.0f))
  {
    CameraYawMovement = 0.0f;
  }
  else
  {
    CameraYawMovement = CameraYawSpeed*NewAxisValue;
  }
}

//WS : MoveForward or Backward
void ATPSCharacter::MoveFrontBack(float NewAxisValue)
{
  AddMovementInput(GetActorForwardVector(),NewAxisValue);
}

//AD : Move Right or Left
void ATPSCharacter::MoveRightLeft(float NewAxisValue)
{
  AddMovementInput(GetActorRightVector(),NewAxisValue);
}

//QE : Character Yaw Controll
void ATPSCharacter::TurnCharacter(float NewAxisValue)
{
  AddControllerYawInput(NewAxisValue);
}