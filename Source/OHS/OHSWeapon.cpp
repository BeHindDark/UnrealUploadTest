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

	TurnTowardDirectAim(DeltaTime);
	
}

void AOHSWeapon::ConnectFireControlSystem(UFireControlSystem * NewFireControlSystem)
{
	if(NewFireControlSystem!=nullptr)
	{
		FireControlSystem = NewFireControlSystem;
		NewFireControlSystem->OnFireOrder.AddUObject(this, &AOHSWeapon::OnFireOrder);
	}
}

/*
 * 무기를 FireControlSystem의 지시에 따라 설정된 회전속도로 회전시킨다.
 * 중력을 무시하고 목표지점을 그대로 바라본다.
 * 중력이 작용하지 않는 투사체, 또는 히트스캔형 무기일 경우 사용
 @ see also : TurnTowardProjectileAim()
 */
void AOHSWeapon::TurnTowardDirectAim(float DeltaTime)
{
	//FireControlSystem과 제대로 연결되었는가
	if(!((FireControlSystem!=nullptr)&&(WeaponIndex!=-1)))
	{
		UE_LOG(OHS, Warning, TEXT("%s / %s : FireControlSystem Connection Error"), *LINE_INFO, *GetNameSafe(this));
		return;
	}
	
	//소켓 위치가 존재하는 가
	if(!FireControlSystem->SocketTransforms[WeaponIndex].IsValid())
	{
		UE_LOG(OHS, Warning, TEXT("%s / %s : Socket Transform Error"), *LINE_INFO, *GetNameSafe(this));
		return;
	}

	//Update TargetLocation
	TargetLocation = FireControlSystem->TargetLocation;

	//현재 달려있는 소켓의 좌표계를 받아온다.

	FTransform SocketTransform = FireControlSystem->SocketTransforms[WeaponIndex];

	//소켓을 기준으로 했을 때 타켓의 위치를 구한 뒤
	//ToOrientationRotator로 해당 위치를 바라보도록 하는 rotation을 구한다. 이때 roll=0으로 고정된다.
	FRotator RelativeTargetDirection = FTransform(FRotator::ZeroRotator,TargetLocation).GetRelativeTransform(SocketTransform).GetLocation().ToOrientationRotator();

	//목표지점을 바라보기 위해 포탑이 가져야하는 상대회전값
	//ClampAngle을 통해 최대 부앙각을 제한시킨다.
	FRotator TargetRelativeRotation = FRotator(FMath::ClampAngle(RelativeTargetDirection.Pitch,-20.0f,60.0f),
												RelativeTargetDirection.Yaw,
												0.0f);

	//소켓에 대한 포탑의 현재 rotation
	FRotator CurRelativeRotation = GetActorTransform().GetRelativeTransform(SocketTransform).Rotator();

	//타겟을 바라보게 하기 위해 회전시켜야 하는 총각도
	FRotator RotationDiff = FRotator::ZeroRotator;
	//덜어내기 용 더미 각도
	FRotator Dummyrot = FRotator::ZeroRotator;

	//두 각도의 차이를 구한 뒤 -180 ~ 180의 값으로 변환하여 RotationDiff에 준다.
	(TargetRelativeRotation - CurRelativeRotation).GetWindingAndRemainder(Dummyrot,RotationDiff);

	//만약 거의 값이 같아서 돌릴필요가 없으면 돌리지 않는다.
	if(RotationDiff.IsNearlyZero(0.001f))
	{
		bLocked = true;
	}
	else
	{
		bLocked = false;
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
		//굳이 이렇게 한단계 거치는 이유는 본체나 포탑이 너무 빠르게 회전할 경우 Roll값이 조금씩 돌아가는 문제가 있기 때문입니다.
		//부동소수점 연산에서 생기는 찌꺼기 값이 계속 누적되서 일지도...
		FRotator NewRelativeRotation = CurRelativeRotation + DeltaRotation;
		NewRelativeRotation.Roll = 0.0f;
		SetActorRelativeRotation(NewRelativeRotation);
	}
}

void AOHSWeapon::OnFireOrder()
{
	UE_LOG(OHS, Warning, TEXT("Fire!!!! (%s)"), *GetNameSafe(this));
}
