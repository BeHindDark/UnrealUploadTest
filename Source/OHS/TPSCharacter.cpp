// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSCharacter.h"
#include "DrawDebugHelpers.h"
#include "OHSWeapon.h"
#include "FireControlSystem.h"
//#include "Engine/StaticMeshSocket.h"

// Sets default values
ATPSCharacter::ATPSCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//컴포넌트 세팅
	TurretRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TurretRoot"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	TPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TPCamera"));
	FireControlSystem = CreateDefaultSubobject<UFireControlSystem>(TEXT("FireControlSystem"));
	UpperBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UpperBody"));
	Cockpit = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cockpit"));

	//크기 조정
	GetCapsuleComponent()->SetCapsuleSize(230.0f,230.0f);
	SpringArm->TargetArmLength = 800.0f;
	SpringArm->SocketOffset = FVector(0.0f,0.0f,300.0f);

	//계층구조
	SpringArm->SetupAttachment(GetCapsuleComponent());
	TPCamera->SetupAttachment(SpringArm);
	
	
	//Load SkeletalMesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SpiderMid(TEXT("/Game/Mech_Constructor_Spiders/Meshes_Skeletal/Legs_Spiders/Legs_Spider_Med.Legs_Spider_Med"));
	if(SK_SpiderMid.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_SpiderMid.Object);
	}
	TurretRoot->SetupAttachment(GetMesh(),TEXT("Mount_Top"));

	//Load StaticMesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ST_UpperBody(TEXT("/Game/Mech_Constructor_Spiders/Meshes/Shoulders_Med_Tank.Shoulders_Med_Tank"));
	if(ST_UpperBody.Succeeded())
	{
		UpperBody->SetStaticMesh(ST_UpperBody.Object);
	}
	UpperBody->SetupAttachment(TurretRoot);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ST_Cockpit(TEXT("/Game/Mech_Constructor_Spiders/Meshes/Cockpit_Moto.Cockpit_Moto"));
	if(ST_Cockpit.Succeeded())
	{
		Cockpit->SetStaticMesh(ST_Cockpit.Object);
	}
	Cockpit->SetupAttachment(UpperBody,TEXT("Mount_Weapon_Top"));

	//위치 조정
	GetMesh()->SetRelativeLocationAndRotation(FVector(70.0f, 0.0f, -220.0f), FRotator(0.0f, -90.0f, 0.0f));
	UpperBody->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	Cockpit->SetRelativeRotation(FRotator(0.0f,0.0f,0.0f));


	//속도 조정
	CameraYawSpeed = 120.0f;
	CameraPitchSpeed = 60.0f;
	
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;
	TPCamera->bUsePawnControlRotation=false;

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 30.0f, 0.0f);

	
	//Load AnimationBlueprint

	//콜리젼 설정
	GetCapsuleComponent()->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

	//SetDefaults

	//  Set Sockets
	SocketArray.SetNum(3);
	SocketArray[0] = UpperBody->GetSocketByName(TEXT("Mount_Weapon_L"));
	SocketArray[1] = UpperBody->GetSocketByName(TEXT("Mount_Weapon_R"));
	SocketArray[2] = Cockpit->GetSocketByName(TEXT("Mount_Weapon_Top"));

	//  Set Socket Mothers
	SocketMotherArray.SetNum(3);
	SocketMotherArray[0] = UpperBody;
	SocketMotherArray[1] = UpperBody;
	SocketMotherArray[2] = Cockpit;
	

	//  FireControlSystem Defaults
	AimingRange = 10000.0f;
	AimingLocation = FVector::ZeroVector;

	for(int i=0; i<SocketArray.Num(); i++)
	{
		FireControlSystem->SocketTransforms.Add(FTransform(FRotator::ZeroRotator,FVector::ZeroVector));
	}
	

}

// Called when the game starts or when spawned
void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	AimingLocation = GetActorLocation() + GetActorForwardVector()*AimingRange;
}

void ATPSCharacter::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);
}

void ATPSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Initialize SocketTransform
	for(int i=0; i<SocketArray.Num(); i++)
	{
		SocketArray[i]->GetSocketTransform(FireControlSystem->SocketTransforms[i], SocketMotherArray[i]);
		auto DefaultWeapon = GetWorld()->SpawnActor<AOHSWeapon>(FireControlSystem->SocketTransforms[i].GetLocation(), FireControlSystem->SocketTransforms[i].Rotator());
		if(DefaultWeapon != nullptr)
		{
			DefaultWeapon->WeaponIndex = FireControlSystem->WeaponArray.Num();
			FireControlSystem->WeaponArray.Add(DefaultWeapon);
			DefaultWeapon->ConnectFireControlSystem(FireControlSystem);
			DefaultWeapon->AttachToComponent(SocketMotherArray[i],FAttachmentTransformRules::SnapToTargetIncludingScale,SocketArray[DefaultWeapon->WeaponIndex]->SocketName);
			//FAttachmentTransformRules::SnapToTargetNotIncludingScale
			DefaultWeapon->SetOwner(this);
			UE_LOG(OHS,Warning,TEXT("Weapon( %s ) Attached to Owner ( %s )"),*DefaultWeapon->GetName(),*DefaultWeapon->GetOwner()->GetName());
		}
	}
	
}

void ATPSCharacter::PossessedBy(AController * NewController)
{
	Super::PossessedBy(NewController);
	if(IsPlayerControlled())
	{
		PlayerController = Cast<APlayerController>(GetController());
		if(PlayerController!=nullptr)
		{
			bIsPlayerControlling = true;
		}
	}
}

// Called every frame
void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Camera Control
	if(TPCamera!=nullptr)
	{
		TPCamera->AddRelativeRotation(FRotator(CameraPitchMovement*DeltaTime,0.0f,0.0f));
	}
	if(SpringArm!=nullptr)
	{
		SpringArm->AddRelativeRotation(FRotator(0.0f,CameraYawMovement*DeltaTime,0.0f));
	}
	

	//Get Aimming Target Location

	/*
	APlayerCameraManager *camManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	FVector camLocation = camManager->GetCameraLocation();
	FVector camForward  = camManager->GetCameraRotation().Vector();
	*/
	if(bIsPlayerControlling)
	{
		AimingLocation = GetCameraAimLocation(TPCamera);
	}

	FireControlSystem->TargetLocation = AimingLocation;
	for(int i=0; i<SocketArray.Num(); i++)
	{
		SocketArray[i]->GetSocketTransform(FireControlSystem->SocketTransforms[i], SocketMotherArray[i]);
	}

	TurnUpperBody(GetMesh(), TurretRoot, TEXT("Mount_Top"), DeltaTime);
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

	//Ation Binding
	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &ATPSCharacter::StartFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Released, this, &ATPSCharacter::StopFire);

	
}

/*
 * 주어진 카메라 컴포넌트가 현재 바라보고 있는 지점의 위치를 구한다.
 * 만약 설정된 AimingRange안에서 Visibility 충돌이 없으면 최대거리 지점의 좌표를 return한다.
 @ param : CameraComponent to use
 @ return : Location Vector of Camera Aim
 */
FVector ATPSCharacter::GetCameraAimLocation(UCameraComponent* CurrentCamera)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	for(AActor* Weapon:FireControlSystem->WeaponArray)
	{
		ActorsToIgnore.Add(Weapon);
	}
	FHitResult AimResult;
	FVector AimStart;
	FVector AimDirection;
	//PlayerController->GetPlayerViewPoint(AimStart, AimRotation);
	AimStart = CurrentCamera->GetComponentLocation();
	AimDirection = CurrentCamera->GetForwardVector();
	FVector AimEnd = AimStart + AimDirection * AimingRange;
	FCollisionQueryParams AimParams;
	AimParams.AddIgnoredActors(ActorsToIgnore);

	//DrawDebugLine(GetWorld(),AimStart,AimEnd,FColor::Green,false,1,0,1);

	bool IsHit = GetWorld()->LineTraceSingleByChannel(AimResult, AimStart, AimEnd, ECC_Visibility, AimParams);
	if (IsHit)
	{
		if (AimResult.bBlockingHit)
		{
			return FVector(AimResult.ImpactPoint);
		}
		else
		{
			return AimEnd;
		}
	}
	else
	{
		return AimEnd;
	}
}

void ATPSCharacter::TurnUpperBody(USkeletalMeshComponent* LowerBody,USceneComponent* TurretRoot,FName SocketName,float DeltaTime)
{

	//현재 달려있는 소켓의 좌표계를 받아온다.

	//소켓을 기준으로 했을 때 타켓의 위치를 구한 뒤
	//ToOrientationRotator로 해당 위치를 바라보도록 하는 rotation을 구한다. 이때 roll=0으로 고정된다.
	FRotator RelativeTargetDirection = FTransform(FRotator::ZeroRotator,AimingLocation).GetRelativeTransform(GetActorTransform()).GetLocation().ToOrientationRotator();

	//목표지점을 바라보기 위해 포탑이 가져야하는 상대회전값
	//ClampAngle을 통해 최대 부앙각을 제한시킨다.

	FRotator TargetRelativeRotation = FRotator(0.0f, RelativeTargetDirection.Yaw, 0.0f);

   //소켓에 대한 포탑의 현재 rotation
	
	FRotator CurRelativeRotation = TurretRoot->GetRelativeTransform().Rotator();

	//타겟을 바라보게 하기 위해 회전시켜야 하는 총각도
	FRotator RotationDiff = FRotator::ZeroRotator;
	//덜어내기 용 더미 각도
	FRotator Dummyrot = FRotator::ZeroRotator;

	//두 각도의 차이를 구한 뒤 -180 ~ 180의 값으로 변환하여 RotationDiff에 준다.
	(TargetRelativeRotation - CurRelativeRotation).GetWindingAndRemainder(Dummyrot,RotationDiff);

	//만약 거의 값이 같아서 돌릴필요가 없으면 돌리지 않는다.
	if(!RotationDiff.IsNearlyZero(0.001f))
	{
		//실제로 돌릴각도 (빈 공간)
		FRotator DeltaRotation = FRotator::ZeroRotator;

		//돌릴 Yaw값을 정한다.
		if(FMath::Abs(RotationDiff.Yaw)<=UpperBodyRotationSpeed*DeltaTime)
		{
			//만약 회전시켜야 하는 총각도가 회전속도*DeltaTime 보다 작으면, 그대로 돌린다.
			DeltaRotation.Yaw = RotationDiff.Yaw;
		}
		else
		{
			//회전시켜야 하는 총각도가 회전속도*DeltaTime보다 크면, 회전속도에 맞게끔 방향만 맞춰서 돌린다.
			DeltaRotation.Yaw = FMath::Sign(RotationDiff.Yaw)*UpperBodyRotationSpeed*DeltaTime;
		}

		//현재상대회전 + 돌릴각도 = 새 상대회전
		//굳이 이렇게 한단계 거치는 이유는 본체나 포탑이 너무 빠르게 회전할 경우 Roll값이 조금씩 돌아가는 문제가 있기 때문입니다.
		//부동소수점 연산에서 생기는 찌꺼기 값이 계속 누적되서 일지도...
		FRotator NewRelativeRotation = CurRelativeRotation + DeltaRotation;
		NewRelativeRotation.Pitch = 0.0f;
		NewRelativeRotation.Roll = 0.0f;
		TurretRoot->SetRelativeRotation(NewRelativeRotation);
	}
}

//mouse Y : TPCamera Peach Controll
void ATPSCharacter::LookUp(float NewAxisValue)
{
	CameraPitchMovement = CameraPitchSpeed*NewAxisValue;
}

//mouse X : TPCamera Yaw Controll
void ATPSCharacter::TurnCamera(float NewAxisValue)
{
	CameraYawMovement = CameraYawSpeed*NewAxisValue;
}

//WS : MoveForward or Backward
void ATPSCharacter::MoveFrontBack(float NewAxisValue)
{
	MoveInput = NewAxisValue;
	AddMovementInput(GetActorForwardVector(),NewAxisValue);
}

//AD : Move Right or Left
void ATPSCharacter::MoveRightLeft(float NewAxisValue)
{
	//실제 속도를 받아야할 경우를 대비해 박아둠.
	/*
	//자기 자신을 기준으로 봤을 때 속도
	float forward_speed = FVector::DotProduct(GetVelocity(), GetActorForwardVector());
	*/
	//
	if(FMath::IsNearlyEqual(MoveInput,0.0f,0.01f))
	{
		AddMovementInput(GetActorRightVector(),NewAxisValue);
	}
	else
	{
		if(MoveInput>0.0f)
		{
			AddControllerYawInput(NewAxisValue*BodyYawSpeed);
		}
		else
		{
			AddControllerYawInput(-NewAxisValue*BodyYawSpeed);
		}
	}
}

//QE : Character Yaw Controll
void ATPSCharacter::TurnCharacter(float NewAxisValue)
{
	AddControllerYawInput(NewAxisValue*BodyYawSpeed);
}

void ATPSCharacter::StartFire()
{
	FireControlSystem->OnFireOrder.Broadcast();
}

void ATPSCharacter::StopFire()
{
}
