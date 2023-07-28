// Copyright Epic Games, Inc. All Rights Reserved.

#include "EldoriasLastStandCharacter.h"

#include <string>

#include "AnimationComponent.h"
#include "AttackComponent.h"
#include "InventoryComponent.h"
#include "QuestComponent.h"
#include "StatsComponent.h"
#include "JetpackComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

//////////////////////////////////////////////////////////////////////////
// AEldoriasLastStandCharacter

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT(text), false);
#define printint(num) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, FString::Printf(TEXT("%i"), num), false);

void AEldoriasLastStandCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Do this here so it happens every time on BeginPlay.
	normalSpeed = GetCharacterMovement()->MaxWalkSpeed;
}

float AEldoriasLastStandCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	// We do "-<value>" so that it actually deals damage.
	StatsComp->ModifyHealth(-Damage, EventInstigator);

	UGameplayStatics::PlaySound2D(DamageCauser, HitSound, 1, 1);
	
	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

AEldoriasLastStandCharacter::AEldoriasLastStandCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create an Attack Component
	AttackComp = CreateDefaultSubobject<UAttackComponent>(TEXT("AttackComp"));
	
	StatsComp = CreateDefaultSubobject<UStatsComponent>(TEXT("StatsComp"));
	
	QuestComp = CreateDefaultSubobject<UQuestComponent>(TEXT("QuestComp"));
	
	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComp"));
	
	JetpackComponent = CreateDefaultSubobject<UJetpackComponent>(TEXT("JetpackComp"));

	AnimationComponent = CreateDefaultSubobject<UAnimationComponent>(TEXT("AnimationComp"));
}

//////////////////////////////////////////////////////////////////////////
// Input

void AEldoriasLastStandCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AEldoriasLastStandCharacter::DoJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AEldoriasLastStandCharacter::StopJump);

	PlayerInputComponent->BindAction("Sprint/Dodge", IE_Pressed, this, &AEldoriasLastStandCharacter::WaitForSprint);
	PlayerInputComponent->BindAction("Sprint/Dodge", IE_Released, this, &AEldoriasLastStandCharacter::DoDodge);

	PlayerInputComponent->BindAction("LightAttack", IE_Pressed, this, &AEldoriasLastStandCharacter::DoLightAttack);

	PlayerInputComponent->BindAction("UseDown", IE_Pressed, this, &AEldoriasLastStandCharacter::UseDownQuickslot);

	// Axis controls.

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AEldoriasLastStandCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AEldoriasLastStandCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AEldoriasLastStandCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AEldoriasLastStandCharacter::LookUpAtRate);
}

void AEldoriasLastStandCharacter::TurnAtRate(float Rate)
{
	if(!CanMove())
		return;
	
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AEldoriasLastStandCharacter::LookUpAtRate(float Rate)
{
	if(!CanMove())
		return;
	
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AEldoriasLastStandCharacter::DoJump()
{
	if(!CanMove() || GetMesh()->GetAnimInstance()->Montage_IsActive(nullptr))
    	return;

	Jump();

	GetWorld()->GetTimerManager().ClearTimer(GroundedTimerHandle);
	GroundedTimerHandle.Invalidate();

	JetpackComponent->StartJetpack();
}

void AEldoriasLastStandCharacter::CheckGround()
{
	if(!IsGrounded())
		return;
	
	if(currentMovementState == MovementState::Jetpack)
	{
		currentMovementState = MovementState::Walking;
		GetWorld()->GetTimerManager().ClearTimer(GroundedTimerHandle);
		GroundedTimerHandle.Invalidate();
	}
}

void AEldoriasLastStandCharacter::StopJump()
{
	JetpackComponent->EndJetpack();
	
	if(currentMovementState == MovementState::Jetpack)
	{
		GetWorld()->GetTimerManager().SetTimer(GroundedTimerHandle, this, &AEldoriasLastStandCharacter::CheckGround, .01f, true);
	}
	
	StopJumping();
}

void AEldoriasLastStandCharacter::MoveForward(float Value)
{
	if(!CanMove())
		return;
	
	// Disallow movement if montage is currently playing, but allow if montage is Equip/Un-equip.
	if(GetMesh()->GetAnimInstance()->Montage_IsActive(nullptr) &&
		!(GetMesh()->GetAnimInstance()->Montage_IsActive(AttackComp->EquipSword) 
		|| GetMesh()->GetAnimInstance()->Montage_IsActive(AttackComp->WithdrawSword)))
	{
		return;
	}
	
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AEldoriasLastStandCharacter::MoveRight(float Value)
{

	if(!CanMove())
		return;
	
	if(GetMesh()->GetAnimInstance()->Montage_IsActive(nullptr) &&
		!(GetMesh()->GetAnimInstance()->Montage_IsActive(AttackComp->EquipSword) 
		|| GetMesh()->GetAnimInstance()->Montage_IsActive(AttackComp->WithdrawSword)))
	{
		return;
	}
	
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AEldoriasLastStandCharacter::WaitForSprint()
{

	if(!CanMove())
		return;
	
	if(GetMesh()->GetAnimInstance()->Montage_IsActive(nullptr) &&
			!(GetMesh()->GetAnimInstance()->Montage_IsActive(AttackComp->EquipSword) 
			|| GetMesh()->GetAnimInstance()->Montage_IsActive(AttackComp->WithdrawSword)))
	{
		return;
	}

	WaitingForSprint = true;
	
	// This will auto die when finished.
	GetWorld()->GetTimerManager().SetTimer(SprintTimerHandle, this, &AEldoriasLastStandCharacter::DoSprint, SprintThreshold, false);
}

void AEldoriasLastStandCharacter::DoSprint()
{
	WaitingForSprint = false;

	if(currentMovementState == MovementState::Jetpack)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		return;
	}
	
	if((GetMesh()->GetAnimInstance()->Montage_IsActive(nullptr) &&
		!(GetMesh()->GetAnimInstance()->Montage_IsActive(AttackComp->EquipSword) 
		|| GetMesh()->GetAnimInstance()->Montage_IsActive(AttackComp->WithdrawSword))) || currentMovementState == MovementState::Dodging)
	{
		return;
	}

	currentMovementState = MovementState::Sprinting;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	
	// We'll still clear here, just in-case (even though it should have auto died).
	GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);
	SprintTimerHandle.Invalidate();
}

void AEldoriasLastStandCharacter::DoDodge()
{
	if(!CanMove() || !IsGrounded())
		return;
	
	// The function name is a bit misleading. This function can either do a dodge or stop the sprinting.
	// We do a check here to see if we're sprinting or not, then we go ahead with whatever we need to do.

	if(currentMovementState == MovementState::Jetpack)
	{
		GetCharacterMovement()->MaxWalkSpeed = normalSpeed;
		WaitingForSprint = false;
		return;
	}
	
	if(GetMesh()->GetAnimInstance()->Montage_IsActive(nullptr) || currentMovementState == MovementState::Dodging)
	{
		WaitingForSprint = false;
		return;
	}

	if(currentMovementState == MovementState::Sprinting)
	{
		currentMovementState = MovementState::Walking;
		GetCharacterMovement()->MaxWalkSpeed = normalSpeed;
		WaitingForSprint = false;
	} else
	{
		// Make sure we don't dodge if we're currently in an animation or we're waiting for sprint.
		// This makes sure that we don't accidentally dodge when after letting go of sprint during a dodge.
		if(GetMesh()->GetAnimInstance()->Montage_IsActive(nullptr) || !WaitingForSprint)
		{
			return;
		}
		
		WaitingForSprint = false;

		currentMovementState = MovementState::Dodging;
		GetCharacterMovement()->GroundFriction = 2;

		// Move the player forward.
		// Moved this code into anim notifiers to time it correctly.
		/*
		FVector launchVector = GetActorForwardVector();
		launchVector = (launchVector * RollForwardMultiplier) + (GetActorUpVector() * RollUpMultiplier);
		LaunchCharacter(launchVector, true, false);
		*/

		PlayAnimMontage(StandingRoll);
		
		// Clear and Invalidate timers, Making sure we won't accidentally sprint when we don't want to.
		GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);
		SprintTimerHandle.Invalidate();
	}
}

void AEldoriasLastStandCharacter::DoLightAttack()
{

	if(!CanMove() || currentMovementState == MovementState::Jetpack
		|| !IsGrounded() || InventoryComp->CurrentWeapon == nullptr)
		return;
	
	if(!AttackComp->WeaponEquipped)
	{
		PlayAnimMontage(AttackComp->EquipSword);
		AttackComp->WeaponEquipped = true;
		GetWorld()->GetTimerManager().SetTimer(UnequipTimerHandle, this, &AEldoriasLastStandCharacter::DoUnequip, TimeForUnequip, false);
	} else
	{
		
		if(!GetMesh()->GetAnimInstance()->Montage_IsActive(nullptr) ||
			GetMesh()->GetAnimInstance()->Montage_IsActive(AttackComp->Swing1) ||
			GetMesh()->GetAnimInstance()->Montage_IsActive(AttackComp->Swing2))
		{
			
			if(!AttackComp->ComboWindowOpen)
			{

				// If there is already a combo going on or we're already doing an then ignore this.
				if(AttackComp->CombosDone > 0 || GetMesh()->GetAnimInstance()->Montage_IsActive(nullptr))
				{
					return;
				}
				
				print("Combo Window Not Open and starting new attack.");
				PlayAnimMontage(AttackComp->Swing1);
				AttackComp->ComboWindowOpen = false;
				AttackComp->CombosDone = 0;
			}
			else
			{
				if(AttackComp->CombosDone >= AttackComp->AmountOfCombosAllowed)
				{
					return;
				}
			
				// if even (0 counts as even here).
				if(AttackComp->CombosDone % 2 == 0)
				{
					PlayAnimMontage(AttackComp->Swing2);
					AttackComp->ComboWindowOpen = false;
				}
				else
				{
					PlayAnimMontage(AttackComp->Swing1);
					AttackComp->ComboWindowOpen = false;
				}
				
				AttackComp->CombosDone += 1;

				UGameplayStatics::PlaySound2D(this, ComboSound, 1, AttackComp->CombosDone / AttackComp->AmountOfCombosAllowed);

				GetWorld()->GetWorldSettings()->SetTimeDilation(.25f);
				
				GetWorld()->GetTimerManager().ClearTimer(ResetTimeDilationHandle);
				GetWorld()->GetTimerManager().SetTimer(ResetTimeDilationHandle, this, &AEldoriasLastStandCharacter::ResetTimeDilation, .05f, false);
			}
			
			GetWorld()->GetTimerManager().ClearTimer(UnequipTimerHandle);
			GetWorld()->GetTimerManager().SetTimer(UnequipTimerHandle, this, &AEldoriasLastStandCharacter::DoUnequip, TimeForUnequip, false);
		}
	}
}

void AEldoriasLastStandCharacter::DoUnequip()
{
	// This makes sure we don't un-equip during jetpack, dodge, not being on the floor or if a montage is currently playing.
	// It will rerun this code again in .1 seconds, meaning we don't lag the game by doing it constantly.
	if(currentMovementState == MovementState::Jetpack || currentMovementState == MovementState::Dodging || !IsGrounded() || GetMesh()->GetAnimInstance()->Montage_IsActive(nullptr))
	{
		GetWorld()->GetTimerManager().SetTimer(UnequipTimerHandle, this, &AEldoriasLastStandCharacter::DoUnequip, .1f, false);
		return;
	}

	// Check for if the weapon is equipped just in-case it somehow isn't.
	if(AttackComp->WeaponEquipped)
	{
		PlayAnimMontage(AttackComp->WithdrawSword);
		AttackComp->WeaponEquipped = false;
		GetWorld()->GetTimerManager().ClearTimer(UnequipTimerHandle);
		
	}
}

void AEldoriasLastStandCharacter::ResetTimeDilation()
{
	GetWorld()->GetWorldSettings()->SetTimeDilation(1.0f);
}

void AEldoriasLastStandCharacter::UseDownQuickslot()
{
	if(currentMovementState == MovementState::Dodging || InventoryComp->InventoryOpen || StatsComp->Dead)
	{
		return;
	}

	InventoryComp->UseEquippedItem();
}