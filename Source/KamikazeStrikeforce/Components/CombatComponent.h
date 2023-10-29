// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KamikazeStrikeforce/HUD/BaseHUD.h"
#include "CombatComponent.generated.h"

class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KAMIKAZESTRIKEFORCE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();
	friend class ABaseCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(AWeapon* weapon);

	UPROPERTY(Replicated)
	bool isAiming;

	FORCEINLINE FVector GetHitLocation() { return hitLocation; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void SetAiming(bool _isAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool _isAiming);

	void FirePressed(bool isPressed);

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize hitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastFire(const FVector_NetQuantize hitTarget);

	void TraceCrosshair(FHitResult& result);

	void SetHUDCrosshair(float DeltaTime);

private:	

	class ABaseCharacter* character;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* equippedWeapon;

	UFUNCTION()
	void OnRep_EquippedWeapon();

	float baseWalkSpeed;
	float aimWalkSpeed;
		
	bool isFirePressed;

	FHitResult hitResult;
	FVector hitLocation;

	FHUDPackage HUDPackage;
	float crosshairVelocityFactor;
	float crosshairInAirFactor;
	float crosshairAimFactor;
	float crosshairShootFactor;

	float defaultFOV;
	float currentFOV;
	
	UPROPERTY(EditAnywhere)
	float zoomedFOV = 30.0f;

	UPROPERTY(EditAnywhere)
	float zoomSpeed = 20.0f;

	void LerpFOV(float DeltaTime);

};
