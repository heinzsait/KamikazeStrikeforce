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
enum class EWeaponFireType : uint8
{
	HitScan UMETA(DisplayName = "Hit Scan"),
	Projectile UMETA(DisplayName = "Projectile"),
	Shotgun UMETA(DisplayName = "Shotgun"),

	MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EWeaponTypes : uint8
{
	AssaultRifle UMETA(DisplayName = "AssaultRifle"),
	RocketLauncher UMETA(DisplayName = "RocketLauncher"),
	Pistol UMETA(DisplayName = "Pistol"),
	SMG UMETA(DisplayName = "SMG"),
	Shotgun UMETA(DisplayName = "Shotgun"),
	Sniper UMETA(DisplayName = "Sniper"),
	GrenadeLauncher UMETA(DisplayName = "GrenadeLauncher"),

	MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	Unoccupied UMETA(DisplayName = "Unoccupied"),
	Reloading UMETA(DisplayName = "Reloading"),
	ThrowingGrenade UMETA(DisplayName = "Throwing Grenade"),

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

UENUM(BlueprintType)
enum class EAvatar : uint8
{
	None UMETA(DisplayName = "None"),
	Manny UMETA(DisplayName = "Manny"),
	Quinn UMETA(DisplayName = "Quinn"),
	Hazard UMETA(DisplayName = "Hazard"),
	Grind UMETA(DisplayName = "Grind"),

	MAX UMETA(DisplayName = "DefaultMAX")
};