// Fill out your copyright notice in the Description page of Project Settings.

#include "BTUCameraActor.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

// Sets default values
ABTUCameraActor::ABTUCameraActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.SetTickFunctionEnable(true);
}

// Called when the game starts or when spawned
void ABTUCameraActor::BeginPlay()
{
	Super::BeginPlay();

	if (GetInstigator())
	{
		// We always want to tick after the actor we're tracking - it avoids visual jitter.
		PrimaryActorTick.AddPrerequisite(GetInstigator(), GetInstigator()->PrimaryActorTick);

	}
}

ABTUChaseCam::ABTUChaseCam()
{
	IdealOffset.Set(0.0f, 1000.0f, 0.0f);
	CameraSpeed_PerAxis.Set(800.0f, 0.0f, 800.0f);
	CameraSpeedLimiter = 800.0f;
	//IdealRotOffset = FRotator(0.0f, 0.0f, 270.0f);
}

void ABTUChaseCam::BeginPlay()
{
	Super::BeginPlay();

	if (GetInstigator())
	{
		SetActorLocation(GetInstigator()->GetActorLocation() + IdealOffset);
		GetCameraComponent()->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(IdealOffset, FVector::ZeroVector).Quaternion());
	}
}

void ABTUChaseCam::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (GetInstigator())
	{
		FVector IdealLocation = GetInstigator()->GetActorLocation() + IdealOffset;
		FVector CurrentLocation = GetActorLocation();
		FVector DeltaLocation = IdealLocation - CurrentLocation;
		FVector MaxMovementThisFrame_PerAxis = CameraSpeed_PerAxis * DeltaSeconds;
		FVector DesiredMovementThisFrame;
		
		// Manage the offest between desired and current camera position
		{
			for (int32 i = 0; i < 3; i++)
			{
				DesiredMovementThisFrame.Component(i) = FMath::Min(FMath::Abs(DeltaLocation.Component(i)), MaxMovementThisFrame_PerAxis.Component(i)) * FMath::Sign(DeltaLocation.Component(i));
			}

			DesiredMovementThisFrame = DesiredMovementThisFrame.GetClampedToMaxSize(CameraSpeedLimiter * DeltaSeconds);
		}

		SetActorLocation(CurrentLocation + DesiredMovementThisFrame);

		// Debug code - adjust as needed, may be a little hacky.
		#if !UE_BUILD_SHIPPING
		if (bShowDebugInfo)
		{
			if (!DesiredMovementThisFrame.IsNearlyZero())
			{
				FVector DebugLineOffset = FVector(0.0f, -IdealOffset.Y, 0.0f);		// This is built with the assumption that the camera always floats the ideal height over the target.
				DrawDebugLine(GetWorld(), CurrentLocation + DebugLineOffset, CurrentLocation + DesiredMovementThisFrame + DebugLineOffset, FColor::Yellow, true, 3.0f);
			}
		}
		#endif
	}
}
