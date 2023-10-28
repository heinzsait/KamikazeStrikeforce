// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletShell.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABulletShell::ABulletShell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	shellMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	SetRootComponent(shellMesh);

	shellEjectImpluseForce = 10.0f;

	shellMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	shellMesh->SetSimulatePhysics(true);
	shellMesh->SetEnableGravity(true);
	shellMesh->SetNotifyRigidBodyCollision(true);
}

// Called when the game starts or when spawned
void ABulletShell::BeginPlay()
{
	Super::BeginPlay();
	
	shellMesh->AddImpulse(GetActorForwardVector() * shellEjectImpluseForce);
	shellMesh->OnComponentHit.AddDynamic(this, &ABulletShell::OnHit);
}

void ABulletShell::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (shellSFX)
		UGameplayStatics::PlaySoundAtLocation(this, shellSFX, GetActorLocation());

	SetLifeSpan(5);
}

