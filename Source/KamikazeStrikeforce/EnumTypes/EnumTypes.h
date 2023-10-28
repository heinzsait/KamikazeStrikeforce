#pragma once

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Initial UMETA(DisplayName = "Initial State"),
	Equipped UMETA(DisplayName = "Equipped"),
	Dropped UMETA(DisplayName = "Dropped"),

	MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class ETurnInPlace : uint8
{
	None UMETA(DisplayName = "None"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right"),

	MAX UMETA(DisplayName = "DefaultMAX")
};