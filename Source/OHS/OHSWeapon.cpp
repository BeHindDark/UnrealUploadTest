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
  WeaponIndex = -1;
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

  //�Ʒ� if�� ���� ������� ���� �Լ��� �ھƳְ� ���� ����� ������ �����Դϴ�.
  //FireControlSystem�� ����� ����Ǿ��°�
  if((FireControlSystem!=nullptr)&&(WeaponIndex!=-1))
  {
    //���� ��ġ�� �����ϴ� ��
    if(FireControlSystem->SocketTransforms[WeaponIndex].IsValid())
    {
      //Update TargetLocation
      TargetLocation = FireControlSystem->TargetLocation;

      //���� �޷��ִ� ������ ��ǥ�踦 �޾ƿ´�.
      FTransform SocketTransform = FireControlSystem->SocketTransforms[WeaponIndex];
      
      //������ �������� ���� �� Ÿ���� ��ġ�� ���� ��
      //ToOrientationRotator�� �ش� ��ġ�� �ٶ󺸵��� �ϴ� rotation�� ���Ѵ�. �̶� roll=0���� �����ȴ�.
      FRotator RelativeTargetDirection = FTransform(FRotator::ZeroRotator,TargetLocation).GetRelativeTransform(SocketTransform).GetLocation().ToOrientationRotator();
      
      //��ǥ������ �ٶ󺸱� ���� ��ž�� �������ϴ� ���ȸ����
      //ClampAngle�� ���� �ִ� �ξӰ��� ���ѽ�Ų��.
      FRotator TargetRelativeRotation = FRotator(FMath::ClampAngle(RelativeTargetDirection.Pitch,-20.0f,60.0f),
                                                 RelativeTargetDirection.Yaw,
                                                 0.0f);
      
      //���Ͽ� ���� ��ž�� ���� rotation
      FRotator CurRelativeRotation = GetActorTransform().GetRelativeTransform(SocketTransform).Rotator();

      //Ÿ���� �ٶ󺸰� �ϱ� ���� ȸ�����Ѿ� �ϴ� �Ѱ���
      FRotator RotationDiff = FRotator::ZeroRotator;
      //����� �� ���� ����
      FRotator Dummyrot = FRotator::ZeroRotator;

      //�� ������ ���̸� ���� �� -180 ~ 180�� ���� �������� ����� �������� RotationDiff�� �ش�.
      (TargetRelativeRotation - CurRelativeRotation).GetWindingAndRemainder(Dummyrot, RotationDiff);

      //���� ���� ���� ���Ƽ� �����ʿ䰡 ������ ������ �ʴ´�.
      if(RotationDiff.IsNearlyZero(0.001f))
      {
        bLocked = true;
      }
      else
      {
        bLocked = false;
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
        //���� �̷��� �Ѵܰ� ��ġ�� ������ ��ü�� ��ž�� �ʹ� ������ ȸ���� ��� Roll���� ���ݾ� ���ư��� ������ �ֱ� �����Դϴ�.
        //�ε��Ҽ��� ���꿡�� ����� ��� ���� ��� �����Ǽ� ������...
        FRotator NewRelativeRotation = CurRelativeRotation + DeltaRotation;
        NewRelativeRotation.Roll = 0.0f;
        SetActorRelativeRotation(NewRelativeRotation);
      }
    }
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
