// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class ELDORIASLASTSTAND_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon|Stats")
	FString WeaponName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon|Stats")
	float Damage;
	
#pragma region Sounds

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Sounds)
	USoundWave* SwordHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Sounds)
	USoundWave* SwordWithdrawSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Sounds)
	USoundWave* SwordPutBackSound;

#pragma endregion 

};
