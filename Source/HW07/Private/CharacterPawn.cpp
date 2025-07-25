// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

// Sets default values
ACharacterPawn::ACharacterPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);
	CapsuleComp->SetSimulatePhysics(false);

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	SkeletalMeshComp->SetupAttachment(RootComponent);
	SkeletalMeshComp->SetSimulatePhysics(false);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
}

// Called when the game starts or when spawned
void ACharacterPawn::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem
				= LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				check(MyInputMappingContext);

				Subsystem->AddMappingContext(MyInputMappingContext, 0);
			}
		}
	}
}

// Called every frame
void ACharacterPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACharacterPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		check(MoveAction);
		check(LookAction);

		EnhancedInput->BindAction(
			MoveAction,
			ETriggerEvent::Triggered,
			this,
			&ACharacterPawn::Move);

		EnhancedInput->BindAction(
			LookAction,
			ETriggerEvent::Triggered,
			this,
			&ACharacterPawn::Look);
	}
}

void ACharacterPawn::Move(const FInputActionValue& Value)
{
	const FVector2D MoveInput = Value.Get<FVector2D>();
	const float DeltaTime = GetWorld()->GetDeltaSeconds();

	AddActorLocalOffset(GetActorForwardVector() * MoveInput.X * MoveSpeed * DeltaTime, true);
	AddActorLocalOffset(GetActorRightVector() * MoveInput.Y * MoveSpeed * DeltaTime, true);
}

void ACharacterPawn::Look(const FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();
	const float DeltaTime = GetWorld()->GetDeltaSeconds();

	FRotator ClampedRotation = SpringArmComp->GetRelativeRotation();
	ClampedRotation.Pitch = FMath::Clamp(ClampedRotation.Pitch, -80.0f, 80.0f);
	ClampedRotation.Yaw = 0;
	ClampedRotation.Roll = 0;

	SpringArmComp->SetRelativeRotation(ClampedRotation);

	AddActorLocalRotation(FRotator(0.0f, LookInput.X * RotationSpeed * DeltaTime, 0.0f));
	SpringArmComp->AddLocalRotation(FRotator(-LookInput.Y * RotationSpeed * DeltaTime, 0.0f, 0.0f));
}
