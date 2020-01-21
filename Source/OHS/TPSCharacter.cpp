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
	
	//������Ʈ ����
	TurretRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TurretRoot"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	TPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TPCamera"));
	FireControlSystem = CreateDefaultSubobject<UFireControlSystem>(TEXT("FireControlSystem"));
	UpperBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UpperBody"));
	Cockpit = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cockpit"));

	//ũ�� ����
	GetCapsuleComponent()->SetCapsuleSize(230.0f,230.0f);
	SpringArm->TargetArmLength = 800.0f;
	SpringArm->SocketOffset = FVector(0.0f,0.0f,300.0f);

	//��������
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

	//��ġ ����
	GetMesh()->SetRelativeLocationAndRotation(FVector(70.0f, 0.0f, -220.0f), FRotator(0.0f, -90.0f, 0.0f));
	UpperBody->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	Cockpit->SetRelativeRotation(FRotator(0.0f,0.0f,0.0f));


	//�ӵ� ����
	CameraYawSpeed = 120.0f;
	CameraPitchSpeed = 60.0f;
	
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;
	TPCamera->bUsePawnControlRotation=false;

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 30.0f, 0.0f);

	
	//Load AnimationBlueprint

	//�ݸ��� ����
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
 * �־��� ī�޶� ������Ʈ�� ���� �ٶ󺸰� �ִ� ������ ��ġ�� ���Ѵ�.
 * ���� ������ AimingRange�ȿ��� Visibility �浹�� ������ �ִ�Ÿ� ������ ��ǥ�� return�Ѵ�.
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

	//���� �޷��ִ� ������ ��ǥ�踦 �޾ƿ´�.

	//������ �������� ���� �� Ÿ���� ��ġ�� ���� ��
	//ToOrientationRotator�� �ش� ��ġ�� �ٶ󺸵��� �ϴ� rotation�� ���Ѵ�. �̶� roll=0���� �����ȴ�.
	FRotator RelativeTargetDirection = FTransform(FRotator::ZeroRotator,AimingLocation).GetRelativeTransform(GetActorTransform()).GetLocation().ToOrientationRotator();

	//��ǥ������ �ٶ󺸱� ���� ��ž�� �������ϴ� ���ȸ����
	//ClampAngle�� ���� �ִ� �ξӰ��� ���ѽ�Ų��.

	FRotator TargetRelativeRotation = FRotator(0.0f, RelativeTargetDirection.Yaw, 0.0f);

   //���Ͽ� ���� ��ž�� ���� rotation
	
	FRotator CurRelativeRotation = TurretRoot->GetRelativeTransform().Rotator();

	//Ÿ���� �ٶ󺸰� �ϱ� ���� ȸ�����Ѿ� �ϴ� �Ѱ���
	FRotator RotationDiff = FRotator::ZeroRotator;
	//����� �� ���� ����
	FRotator Dummyrot = FRotator::ZeroRotator;

	//�� ������ ���̸� ���� �� -180 ~ 180�� ������ ��ȯ�Ͽ� RotationDiff�� �ش�.
	(TargetRelativeRotation - CurRelativeRotation).GetWindingAndRemainder(Dummyrot,RotationDiff);

	//���� ���� ���� ���Ƽ� �����ʿ䰡 ������ ������ �ʴ´�.
	if(!RotationDiff.IsNearlyZero(0.001f))
	{
		//������ �������� (�� ����)
		FRotator DeltaRotation = FRotator::ZeroRotator;

		//���� Yaw���� ���Ѵ�.
		if(FMath::Abs(RotationDiff.Yaw)<=UpperBodyRotationSpeed*DeltaTime)
		{
			//���� ȸ�����Ѿ� �ϴ� �Ѱ����� ȸ���ӵ�*DeltaTime ���� ������, �״�� ������.
			DeltaRotation.Yaw = RotationDiff.Yaw;
		}
		else
		{
			//ȸ�����Ѿ� �ϴ� �Ѱ����� ȸ���ӵ�*DeltaTime���� ũ��, ȸ���ӵ��� �°Բ� ���⸸ ���缭 ������.
			DeltaRotation.Yaw = FMath::Sign(RotationDiff.Yaw)*UpperBodyRotationSpeed*DeltaTime;
		}

		//������ȸ�� + �������� = �� ���ȸ��
		//���� �̷��� �Ѵܰ� ��ġ�� ������ ��ü�� ��ž�� �ʹ� ������ ȸ���� ��� Roll���� ���ݾ� ���ư��� ������ �ֱ� �����Դϴ�.
		//�ε��Ҽ��� ���꿡�� ����� ��� ���� ��� �����Ǽ� ������...
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
	//���� �ӵ��� �޾ƾ��� ��츦 ����� �ھƵ�.
	/*
	//�ڱ� �ڽ��� �������� ���� �� �ӵ�
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
