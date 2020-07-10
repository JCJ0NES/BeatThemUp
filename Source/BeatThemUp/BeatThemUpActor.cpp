// Fill out your copyright notice in the Description page of Project Settings.


#include "BeatThemUpActor.h"

// Sets default values
ABeatThemUpActor::ABeatThemUpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TotalDamage = 200;
	DamageTimeInSeconds = 1.0f;

}

// Called when the game starts or when spawned
void ABeatThemUpActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABeatThemUpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABeatThemUpActor::PostInitProperties()
{
	Super::PostInitProperties();

	CalculateValues();
}

#if WITH_EDITOR
void ABeatThemUpActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	CalculateValues();

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void ABeatThemUpActor::CalculateValues()
{
	DamagePerSecond = TotalDamage / DamageTimeInSeconds;
}

