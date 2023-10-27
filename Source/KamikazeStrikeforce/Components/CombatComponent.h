// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void SetAiming(bool _isAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool _isAiming);

private:	

	class ABaseCharacter* character;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* equippedWeapon;

	UFUNCTION()
	void OnRep_EquippedWeapon();
		
};
