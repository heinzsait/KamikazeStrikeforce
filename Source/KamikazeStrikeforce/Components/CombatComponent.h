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
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	friend class ABaseCharacter;

	void EquipWeapon(AWeapon* weapon);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:	
	AWeapon* equippedWeapon;
	class ABaseCharacter* character;
		
};
