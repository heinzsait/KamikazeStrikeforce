// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "KamikazeStrikeforce/Weapon/Weapon.h"
#include "KamikazeStrikeforce/Character/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, equippedWeapon);
	DOREPLIFETIME(UCombatComponent, isAiming);
}

void UCombatComponent::EquipWeapon(AWeapon* weapon)
{	
	if (character && weapon)
	{
		const USkeletalMeshSocket* handSocket = character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (handSocket)
		{
			equippedWeapon = weapon;
			equippedWeapon->SetWeaponState(EWeaponState::Equipped);
			equippedWeapon->SetOwner(character);
			handSocket->AttachActor(equippedWeapon, character->GetMesh());

			character->GetCharacterMovement()->bOrientRotationToMovement = false;
			character->bUseControllerRotationYaw = true;
		}
	}
}


void UCombatComponent::SetAiming(bool _isAiming)
{
	isAiming = _isAiming;
	ServerSetAiming(_isAiming);
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (equippedWeapon && character)
	{
		character->GetCharacterMovement()->bOrientRotationToMovement = false;
		character->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool _isAiming)
{
	isAiming = _isAiming;
}