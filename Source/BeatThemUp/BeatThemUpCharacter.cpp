// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeatThemUpCharacter.h"
#include "BTUCameraActor.h"
#include "PaperFlipbookComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "PaperFlipbook.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);

//////////////////////////////////////////////////////////////////////////
// ABeatThemUpCharacter

const FName ABeatThemUpCharacter::MoveRightBind("MoveRight");
const FName ABeatThemUpCharacter::MoveUpBind("MoveUp");
const FName ABeatThemUpCharacter::JumpBind("Jump");


ABeatThemUpCharacter::ABeatThemUpCharacter()
{	
	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(31.0f);
	GetCapsuleComponent()->SetCapsuleRadius(14.0f);

	// Prevent all automatic rotation behavior on the camera, character, and camera component
	//GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure character movement
	MoveSpeed = 1000.0f;
	//GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->GravityScale = 0.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MaxFlySpeed = 600.0f;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, -1.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

	// 	TextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
	// 	TextComponent->SetRelativeScale3D(FVector(3.0f, 3.0f, 3.0f));
	// 	TextComponent->SetRelativeLocation(FVector(35.0f, 5.0f, 20.0f));
	// 	TextComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	// 	TextComponent->SetupAttachment(RootComponent);

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	bReplicates = true;
}

//////////////////////////////////////////////////////////////////////////
// Animation

void ABeatThemUpCharacter::UpdateAnimation()
{
	//const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = FMath::Square(GetInputAxisValue(MoveRightBind));

	// Are we moving or standing still?
	UPaperFlipbook* DesiredAnimation = (PlayerSpeedSqr > 0.0f) ? RunningAnimation : IdleAnimation;
	if (GetSprite()->GetFlipbook() != DesiredAnimation)
	{
		GetSprite()->SetFlipbook(DesiredAnimation);
	}
}

void ABeatThemUpCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	MoveChar(DeltaSeconds);
	UpdateCharacter();
}


//////////////////////////////////////////////////////////////////////////
// Input

void ABeatThemUpCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Note: the 'Jump' action and the 'MoveRight' axis are bound to actual keys/buttons/sticks in DefaultInput.ini (editable from Project Settings..Input)
	PlayerInputComponent->BindAction(JumpBind, IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(JumpBind, IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis(MoveRightBind);
	PlayerInputComponent->BindAxis(MoveUpBind);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &ABeatThemUpCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ABeatThemUpCharacter::TouchStopped);
}

/** old movement code, might be useful later 
void ABeatThemUpCharacter::MoveRight(float Value)
{
	//UpdateChar();

	// Apply the input to the character motion
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
	//UE_LOG(LogTemp, Warning, TEXT("Right Value is %f"), Value);
} */

void ABeatThemUpCharacter::MoveChar(float DeltaSeconds)
{
	const float RightValue = GetInputAxisValue(MoveRightBind);
	const float UpValue = GetInputAxisValue(MoveUpBind);

	const FVector MoveDirection = FVector(RightValue, 0.f, UpValue).GetClampedToMaxSize(1.0f);
	const FVector Movement = MoveDirection * MoveSpeed * DeltaSeconds;
	if (Movement.SizeSquared() > 0.0f)
	{
		const FRotator NewRotation = FRotator(0.f, Movement.Rotation().Yaw, 0.f);
		//UE_LOG(LogTemp, Warning, TEXT("Right Value is %f"), RightValue);
		//UE_LOG(LogTemp, Warning, TEXT("Up Value is %f"), UpValue);
		RootComponent->MoveComponent(Movement, NewRotation, true);
	}
}

void ABeatThemUpCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Jump on any touch
	Jump();
}

void ABeatThemUpCharacter::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Cease jumping once touch stopped
	StopJumping();
}

void ABeatThemUpCharacter::UpdateCharacter()
{
	// Update animation to match the motion
	UpdateAnimation();

	// Now setup the rotation of the controller based on the direction we are travelling
	const FVector PlayerVelocity = GetVelocity();
	float TravelDirection = PlayerVelocity.X;
	// Set the rotation so that the character faces his direction of travel.
	if (Controller != nullptr)
	{
		if (TravelDirection < 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
		}
		else if (TravelDirection > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
	}
}

void ABeatThemUpCharacter::SetupCamera(UWorld* World)
{
	if (!CameraActorToSpawn)
	{
		CameraActorToSpawn = ABTUCameraActor::StaticClass();
	}
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	BTUCamera = World->SpawnActor<ABTUCameraActor>(CameraActorToSpawn->GetDefaultObject()->GetClass(), GetTransform(), SpawnParameters);
	check(BTUCamera);
	//MyCamera->Instigator = this;	// This is too late! BeginPlay has already been called on the new camera. Use SpawnParameters instead.
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetViewTarget(BTUCamera);
	}
}

void ABeatThemUpCharacter::BeginPlay()
{
	UWorld* World = GetWorld();

	check(World);
	Super::BeginPlay();

	SetupCamera(World);
}
