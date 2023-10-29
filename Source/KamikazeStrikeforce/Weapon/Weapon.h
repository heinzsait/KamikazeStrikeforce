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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	

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
};
