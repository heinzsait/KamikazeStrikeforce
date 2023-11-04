// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Animation/AnimationAsset.h"
#include "KamikazeStrikeforce/Character/MainCharacter.h"
#include "KamikazeStrikeforce/PlayerController/MainPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "BulletShell.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	weaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	SetRootComponent(weaponMesh);

	weaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	weaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	weaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	weaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	areaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Area Sphere"));
	areaSphere->SetupAttachment(RootComponent);
	areaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	areaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	pickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Pickup Widget"));
	pickupWidget->SetupAttachment(RootComponent);
}


// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		areaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		areaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		areaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		areaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}

	if (pickupWidget)
	{
		pickupWidget->SetVisibility(false);
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, weaponState);
	DOREPLIFETIME(AWeapon, ammo);
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();

	if (!Owner)
	{
		playerController = nullptr;
		character = nullptr;
	}
	else
	{
		UpdateHUDAmmo();
	}
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AWeapon::SetWeaponState(EWeaponState state)
{
	weaponState = state;
	switch (weaponState)
	{
	case EWeaponState::Initial:

		break;

	case EWeaponState::Equipped:
		ShowPickupWidget(false);
		areaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		weaponMesh->SetSimulatePhysics(false);
		weaponMesh->SetEnableGravity(false);
		weaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (weaponType == EWeaponTypes::SMG)
		{			
			weaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			weaponMesh->SetEnableGravity(true);
			weaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		}
		break;

	case EWeaponState::Dropped:
		if (HasAuthority())
		{
			areaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		weaponMesh->SetSimulatePhysics(true);
		weaponMesh->SetEnableGravity(true);
		weaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		weaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		weaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		weaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		break;

	default:
		break;
	}
}

void AWeapon::OnRep_WeaponState()
{
	switch (weaponState)
	{
	case EWeaponState::Initial:

		break;

	case EWeaponState::Equipped:
		ShowPickupWidget(false);
		weaponMesh->SetSimulatePhysics(false);
		weaponMesh->SetEnableGravity(false);
		weaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (weaponType == EWeaponTypes::SMG)
		{
			weaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			weaponMesh->SetEnableGravity(true);
			weaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		}
		break;

	case EWeaponState::Dropped:
		weaponMesh->SetSimulatePhysics(true);
		weaponMesh->SetEnableGravity(true);
		weaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		weaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		weaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		weaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		break;

	default:

		break;
	}
}

void AWeapon::OnRep_Ammo()
{
	UpdateHUDAmmo();
}

void AWeapon::UpdateAmmo()
{
	ammo = FMath::Clamp(ammo - 1, 0, magCapacity);

	UpdateHUDAmmo();
}

void AWeapon::UpdateHUDAmmo()
{
	if (!character)
		character = Cast<AMainCharacter>(GetOwner());
	if (character)
	{
		if (!playerController)
			playerController = character->GetPlayerController();
		if (playerController)
		{
			playerController->SetHUDAmmo(ammo);
		}
	}
}

bool AWeapon::IsEmpty()
{
	return (ammo <= 0);
}

void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (pickupWidget)
	{
		pickupWidget->SetVisibility(bShowWidget);
	}
}

void AWeapon::Fire(const FVector hitLocation)
{
	if (fireAnimation)
	{
		weaponMesh->PlayAnimation(fireAnimation, false);
	}

	if (bulletShellClass)
	{
		auto spawnSocket = GetWeaponMesh()->GetSocketByName(bulletShellSpawnSocketName);
		if (spawnSocket)
		{
			FTransform socketTransform = spawnSocket->GetSocketTransform(GetWeaponMesh());
			if (bulletShellClass)
			{
				if (GetWorld())
				{
					GetWorld()->SpawnActor<ABulletShell>(bulletShellClass, socketTransform);
				}
			}
		}
	}

	UpdateAmmo();
}


void AWeapon::AddAmmo(int ammoAmt)
{
	ammo = FMath::Clamp(ammo + ammoAmt, 0, magCapacity);
	UpdateHUDAmmo();
}

void AWeapon::DropWeapon()
{
	SetWeaponState(EWeaponState::Dropped);
	weaponMesh->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	SetOwner(nullptr);
	playerController = nullptr;
	character = nullptr;
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainCharacter* otherCharacter = Cast<AMainCharacter>(OtherActor);
	if(otherCharacter && pickupWidget)
	{
		otherCharacter->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMainCharacter* otherCharacter = Cast<AMainCharacter>(OtherActor);
	if (otherCharacter)
	{
		otherCharacter->SetOverlappingWeapon(nullptr);
	}
}



