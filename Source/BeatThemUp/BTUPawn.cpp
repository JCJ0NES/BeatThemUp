// Fill out your copyright notice in the Description page of Project Settings.


#include "BTUPawn.h"

// Sets default values
ABTUPawn::ABTUPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABTUPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABTUPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABTUPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

