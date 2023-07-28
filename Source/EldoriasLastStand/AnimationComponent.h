// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnimationComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDORIASLASTSTAND_API UAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAnimationComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	
	/** Is the player currently doing an animation which is Upper Body only? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation|Stats")
	bool CurrentlyUpperBodyAnim;

		
};
