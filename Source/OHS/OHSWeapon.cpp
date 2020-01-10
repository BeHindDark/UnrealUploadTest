// Fill out your copyright notice in the Description page of Project Settings.


#include "OHSWeapon.h"
#include "FireControlSystem.h"

// Sets default values
AOHSWeapon::AOHSWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

  //Set Components
  Sceneroot = CreateDefaultSubobject<USceneComponent>(TEXT("SCENEROOT"));
  WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPONMESH"));

  //계층구조
  RootComponent = Sceneroot;
  WeaponMesh->SetupAttachment(Sceneroot);

  //위치/회전 설정
 
  //Load SkeletalMEsh
  static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_DEFAULTWEAPON(TEXT("/Game/Mech_Constructor_Spiders/Meshes_Skeletal/Weapons/Weapon_Double_Gun_Lvl3.Weapon_Double_Gun_Lvl3"));
  if(SK_DEFAULTWEAPON.Succeeded())
  {
    WeaponMesh->SetSkeletalMesh(SK_DEFAULTWEAPON.Object);
  }
  WeaponMesh->SetRelativeLocationAndRotation(FVector::ZeroVector,FRotator(90.0f,0.0f,90.0f));
  //SkeletalMesh'/Game/Mech_Constructor_Spiders/Meshes_Skeletal/Weapons/Weapon_Double_Gun_Lvl3.Weapon_Double_Gun_Lvl3'

  PitchRotationSpeed = 15.0f;
  YawRotationSpeed = 30.0f;
  TargetLocation = FVector::ZeroVector;
}

// Called when the game starts or when spawned
void AOHSWeapon::BeginPlay()
{
	Super::BeginPlay();
  
}

// Called every frame
void AOHSWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

  //지금은 본체인 TPSCharacter의 포인터를 갖고 있는지를 따지지만
  //나중엔 FireControlSystem이라는 ActorComponent로부터 값을 받아올 예정
  if(FireControlSystem!=nullptr)
  {
    TargetLocation = FireControlSystem->TargetLocation;
    //포탑이 바라봐야 하는 방향 (rotation()을 쓰면 roll은 무조건 0이 나온다)
    FRotator TargetDirection = (TargetLocation - GetActorLocation()).Rotation();

    //현재 본체의 방향
    FRotator BodyRotation = FireControlSystem->BodyRotation;

    //목표지점을 바라보기 위한 포탑의 상대회전 값
    FRotator TargetRelativeRotation = FRotator(FMath::ClampAngle(TargetDirection.Pitch - BodyRotation.Pitch,-20.0f,60.0f),
                                                TargetDirection.Yaw - BodyRotation.Yaw,
                                                0.0f);

    //이미 해당 방향 인근을 바라보고 있다면 굳이 열심히 계산하지 말고 걍 그쪽으로 정렬시키기
    if(TargetDirection.Equals(GetActorRotation(),0.0001f))
    {
      SetActorRelativeRotation(TargetRelativeRotation);
      bLocked = true;
    }
    else
    {
      //현재 본체에 대한 상대회전
      FRotator CurRelativeRotation = GetActorRotation()- BodyRotation;

      //타겟을 바라보게 하기 위해 회전시켜야 하는 총각도
      FRotator RotationDiff = TargetRelativeRotation - CurRelativeRotation;

      //실제로 돌릴각도 (빈 공간)
      FRotator DeltaRotation = FRotator::ZeroRotator;

      //돌릴 Pitch값을 정한다.
      if(FMath::Abs(RotationDiff.Pitch)<=PitchRotationSpeed*DeltaTime)
      {
        //만약 회전시켜야 하는 총각도가 회전속도*DeltaTime 보다 작으면, 그대로 돌린다.
        DeltaRotation.Pitch = RotationDiff.Pitch;
      }
      else
      {
        //회전시켜야 하는 총각도가 회전속도*DeltaTime보다 크면, 회전속도에 맞게끔 방향만 맞춰서 돌린다.
        DeltaRotation.Pitch = FMath::Sign(RotationDiff.Pitch)*PitchRotationSpeed*DeltaTime;
      }

      //Yaw도 마찬가지로 돌릴각도를 정한다.
      if(FMath::Abs(RotationDiff.Yaw)<=PitchRotationSpeed*DeltaTime)
      {
        DeltaRotation.Yaw = RotationDiff.Yaw;
      }
      else
      {
        DeltaRotation.Yaw = FMath::Sign(RotationDiff.Yaw)*YawRotationSpeed*DeltaTime;
      }

      //현재상대회전 + 돌릴각도 = 새 상대회전
      SetActorRelativeRotation(CurRelativeRotation + DeltaRotation);
      bLocked = false;
    }
    //AddActorWorldRotation(DeltaRotation);
  }

}

void AOHSWeapon::SetOwner(AActor * NewOwner)
{
  Super::SetOwner(NewOwner);
}

void AOHSWeapon::ConnectFireControlSystem(UFireControlSystem * NewFireControlSystem)
{
  if(NewFireControlSystem!=nullptr)
  {
    FireControlSystem = NewFireControlSystem;
    NewFireControlSystem->OnFireOrder.AddUObject(this, &AOHSWeapon::OnFireOrder);
  }
}

void AOHSWeapon::OnFireOrder()
{
  UE_LOG(OHS, Warning, TEXT("Fire!!!!"));
}
