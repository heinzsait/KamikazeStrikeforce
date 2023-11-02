// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KamikazeStrikeforce/EnumTypes/EnumTypes.h"
#include "Weapon.generated.h"

UCLASS()
class KAMIKAZESTRIKEFORCE_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;

	void ShowPickupWidget(bool bShowWidget);
	virtual void Fire(const FVector hitLocation);

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetWeaponState(EWeaponState state);

	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return weaponMesh; }

	UPROPERTY(EditAnywhere, Category = CrossHairs)
	class UTexture2D* centerCH;
	UPROPERTY(EditAnywhere, Category = CrossHairs)
	UTexture2D* topCH;
	UPROPERTY(EditAnywhere, Category = CrossHairs)
	UTexture2D* bottomCH;
	UPROPERTY(EditAnywhere, Category = CrossHairs)
	UTexture2D* leftCH;
	UPROPERTY(EditAnywhere, Category = CrossHairs)
	UTexture2D* rightCH;

	FORCEINLINE float GetZoomFOV() { return zoomedFOV; }
	FORCEINLINE float GetZoomSpeed() { return zoomSpeed; }

	FORCEINLINE EWeaponTypes GetWeaponType() { return weaponType; }

	FORCEINLINE int GetAmmo() { return ammo; }
	FORCEINLINE int GetMagCapacity() { return magCapacity; }
	FORCEINLINE int GetRoomInMag() { return (magCapacity - ammo); }


	UPROPERTY(EditAnywhere)
	float fireDelay = 0.15f;

	UPROPERTY(EditAnywhere)
	bool isAutomatic;

	void AddAmmo(int ammoAmt);
	void DropWeapon();
	void UpdateHUDAmmo();

	bool IsEmpty();

	UPROPERTY(EditAnywhere)
	USoundBase* equipSFX;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	

	UPROPERTY(EditAnywhere)
	EWeaponTypes weaponType;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* weaponMesh;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* areaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState)
	EWeaponState weaponState;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* pickupWidget;

	UPROPERTY(EditAnywhere)
	class UAnimationAsset* fireAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABulletShell> bulletShellClass;

	UPROPERTY(EditAnywhere)
	FName bulletShellSpawnSocketName = FName("AmmoEject");

	UPROPERTY(EditAnywhere)
	float zoomedFOV = 30.0f;

	UPROPERTY(EditAnywhere)
	float zoomSpeed = 20.0f;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo)
	int ammo = 20;

	UPROPERTY(EditAnywhere)
	int magCapacity = 20;

	class ABasePlayerController* playerController = nullptr;

	class ABaseCharacter* character = nullptr;

	UFUNCTION()
	void OnRep_Ammo();

	void UpdateAmmo();
};
