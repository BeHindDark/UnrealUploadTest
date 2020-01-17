// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSCharacter.h"
#include "DrawDebugHelpers.h"
#include "OHSWeapon.h"
#include "FireControlSystem.h"

// Sets default values
ATPSCharacter::ATPSCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//컴포넌트 세팅
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	TPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BOXCOLLISION"));
	FireControlSystem = CreateDefaultSubobject<UFireControlSystem>(TEXT("FIRECONTROLSYSTEM"));
	
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

	//속도 조정
	CameraYawSpeed = 120.0f;
	CameraPitchSpeed = 60.0f;
	
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;
	TPCamera->bUsePawnControlRotation=false;

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 30.0f, 0.0f);

	//Load SkeletalMesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SpiderMid(TEXT("/Game/Mech_Constructor_Spiders/Meshes_Skeletal/Legs_Spiders/Legs_Spider_Med.Legs_Spider_Med"));
	if(SK_SpiderMid.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_SpiderMid.Object);
	}

	//Load AnimationBlueprint

	//콜리젼 설정
	BoxCollision->SetCollisionProfileName(TEXT("Pawn"));

	//SetDefaults
	
	//  Set Sockets
	SocketArray.Add(GetMesh()->GetSocketByName(TEXT("Mount_Top")));

	//  FireControlSystem Defaults
	AimingRange = 10000.0f;
	AimingLocation = FVector::ZeroVector;


}

// Called when the game starts or when spawned
void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	AimingLocation = GetActorLocation() + GetActorForwardVector()*10000.0;
}

void ATPSCharacter::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);
}

void ATPSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Initialize SocketTransform
	for(const USkeletalMeshSocket * Socket : SocketArray)
	{
		FireControlSystem->SocketTransforms.Add(Socket->GetSocketTransform(GetMesh())); 
	}
	auto DefaultWeapon = GetWorld()->SpawnActor<AOHSWeapon>(FVector::ZeroVector,FRotator::ZeroRotator);
	if(DefaultWeapon != nullptr)
	{
		DefaultWeapon->WeaponIndex = FireControlSystem->WeaponArray.Num();
		FireControlSystem->WeaponArray.Add(DefaultWeapon);
		DefaultWeapon->ConnectFireControlSystem(FireControlSystem);
		DefaultWeapon->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketArray[DefaultWeapon->WeaponIndex]->SocketName);
		DefaultWeapon->SetOwner(this);
		UE_LOG(OHS,Warning,TEXT("Weapon( %s ) Attached to Owner ( %s )"),*DefaultWeapon->GetName(),*DefaultWeapon->GetOwner()->GetName());
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
	TPCamera->AddRelativeRotation(FRotator(CameraPitchMovement*DeltaTime, 0.0f, 0.0f));
	SpringArm->AddRelativeRotation(FRotator(0.0f, CameraYawMovement*DeltaTime, 0.0f));

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
		FireControlSystem->SocketTransforms[i] = SocketArray[i]->GetSocketTransform(GetMesh());
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
	FHitResult AimResult;
	FVector AimStart;
	FVector AimDirection;
	//PlayerController->GetPlayerViewPoint(AimStart, AimRotation);
	AimStart = CurrentCamera->GetComponentLocation();
	AimDirection = CurrentCamera->GetForwardVector();
	FVector AimEnd = AimStart + AimDirection * AimingRange;
	FCollisionQueryParams AimParams;

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
