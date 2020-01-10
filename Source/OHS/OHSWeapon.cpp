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

  //��������
  RootComponent = Sceneroot;
  WeaponMesh->SetupAttachment(Sceneroot);

  //��ġ/ȸ�� ����
 
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

  //������ ��ü�� TPSCharacter�� �����͸� ���� �ִ����� ��������
  //���߿� FireControlSystem�̶�� ActorComponent�κ��� ���� �޾ƿ� ����
  if(FireControlSystem!=nullptr)
  {
    TargetLocation = FireControlSystem->TargetLocation;
    //��ž�� �ٶ���� �ϴ� ���� (rotation()�� ���� roll�� ������ 0�� ���´�)
    FRotator TargetDirection = (TargetLocation - GetActorLocation()).Rotation();

    //���� ��ü�� ����
    FRotator BodyRotation = FireControlSystem->BodyRotation;

    //��ǥ������ �ٶ󺸱� ���� ��ž�� ���ȸ�� ��
    FRotator TargetRelativeRotation = FRotator(FMath::ClampAngle(TargetDirection.Pitch - BodyRotation.Pitch,-20.0f,60.0f),
                                                TargetDirection.Yaw - BodyRotation.Yaw,
                                                0.0f);

    //�̹� �ش� ���� �α��� �ٶ󺸰� �ִٸ� ���� ������ ������� ���� �� �������� ���Ľ�Ű��
    if(TargetDirection.Equals(GetActorRotation(),0.0001f))
    {
      SetActorRelativeRotation(TargetRelativeRotation);
      bLocked = true;
    }
    else
    {
      //���� ��ü�� ���� ���ȸ��
      FRotator CurRelativeRotation = GetActorRotation()- BodyRotation;

      //Ÿ���� �ٶ󺸰� �ϱ� ���� ȸ�����Ѿ� �ϴ� �Ѱ���
      FRotator RotationDiff = TargetRelativeRotation - CurRelativeRotation;

      //������ �������� (�� ����)
      FRotator DeltaRotation = FRotator::ZeroRotator;

      //���� Pitch���� ���Ѵ�.
      if(FMath::Abs(RotationDiff.Pitch)<=PitchRotationSpeed*DeltaTime)
      {
        //���� ȸ�����Ѿ� �ϴ� �Ѱ����� ȸ���ӵ�*DeltaTime ���� ������, �״�� ������.
        DeltaRotation.Pitch = RotationDiff.Pitch;
      }
      else
      {
        //ȸ�����Ѿ� �ϴ� �Ѱ����� ȸ���ӵ�*DeltaTime���� ũ��, ȸ���ӵ��� �°Բ� ���⸸ ���缭 ������.
        DeltaRotation.Pitch = FMath::Sign(RotationDiff.Pitch)*PitchRotationSpeed*DeltaTime;
      }

      //Yaw�� ���������� ���������� ���Ѵ�.
      if(FMath::Abs(RotationDiff.Yaw)<=PitchRotationSpeed*DeltaTime)
      {
        DeltaRotation.Yaw = RotationDiff.Yaw;
      }
      else
      {
        DeltaRotation.Yaw = FMath::Sign(RotationDiff.Yaw)*YawRotationSpeed*DeltaTime;
      }

      //������ȸ�� + �������� = �� ���ȸ��
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
