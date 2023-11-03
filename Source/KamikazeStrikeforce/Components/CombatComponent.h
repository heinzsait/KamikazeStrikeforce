// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KamikazeStrikeforce/HUD/MainHUD.h"
#include "KamikazeStrikeforce/EnumTypes/EnumTypes.h"
#include "CombatComponent.generated.h"

class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KAMIKAZESTRIKEFORCE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();
	friend class AMainCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(AWeapon* weapon);
	void FirePressed(bool isPressed);
	void Reload();

	UPROPERTY(Replicated)
	bool isAiming;

	FORCEINLINE FVector GetHitLocation() { return hitLocation; }

	UFUNCTION(BlueprintCallable)
	void FinishReloading();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void SetAiming(bool _isAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool _isAiming);

	void Fire();

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize hitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastFire(const FVector_NetQuantize hitTarget);

	void TraceCrosshair(FHitResult& result);

	void SetHUDCrosshair(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerReload();

	void HandleReload();

	int AmountToReload();

private:	

	class AMainCharacter* character = nullptr;

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

	FTimerHandle fireTimer;
	bool canFire = true;
	bool CanFire();
	void StartFireTimer();
	void FireTimerFinished();

	void DropWeapon();

	void ReloadWeaponAmmo();


	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int carriedAmmo;

	UPROPERTY(EditAnywhere)
	int startingARAmmo = 100;

	UPROPERTY(EditAnywhere)
	int startingPistolAmmo = 50;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponTypes, int> carriedAmmoMap;

	void InitAmmos();

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState combatState;

	UFUNCTION()
	void OnRep_CombatState();
};
