// Fill out your copyright notice in the Description page of Project Settings.


#include "OHSWeapon.h"
#include "TPSCharacter.h"

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
  if(TPSCharacter!=nullptr)
  {
    auto BodyRotation = TPSCharacter->GetActorRotation();
    auto CurRelativeRotation = GetActorRotation()- BodyRotation;
    auto TargetDirection = (TargetLocation - GetActorLocation()).Rotation();
    auto TargetRelativeRotation = TargetDirection - BodyRotation;
    auto RestrictedRelativeRotation = FRotator(TargetRelativeRotation.Pitch, TargetRelativeRotation.Yaw, 0.0f);
    auto DeltaRotation = FRotator((RestrictedRelativeRotation - CurRelativeRotation).GetNormalized().Pitch*PitchRotationSpeed*DeltaTime,(RestrictedRelativeRotation - CurRelativeRotation).GetNormalized().Yaw*YawRotationSpeed*DeltaTime, 0.0f);
    SetActorRelativeRotation(CurRelativeRotation + DeltaRotation);
    //AddActorLocalRotation(DeltaRotation);
    //FMath::Clamp(TargetDirection - BodyRotation, FRotator(-10.0f, -90.0f, 0.0f), FRotator(50.0f, 90.0f, 0.0f))
  }
}

void AOHSWeapon::SetOwner(AActor * NewOwner)
{
  Super::SetOwner(NewOwner);
  TPSCharacter = Cast<ATPSCharacter>(NewOwner);
}

