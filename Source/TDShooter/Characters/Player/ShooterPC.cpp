// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "ShooterPC.h"

#include "ShooterCharacter.h"
#include "../../HUD/PlayerHUDWidget.h"
#include "Loot/LootContainers/IA_Container_Weapon.h"

AShooterPC::AShooterPC()
{
	IgnoreMouseLookInput = false;
	IgnoreMovementInput = false;
	RotateTowardsMouse = true;
	CanFireWeapon = true;
	TraceForInteractions = true;
}

// Called to bind functionality to input
void AShooterPC::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Jumping
	InputComponent->BindAction("Jump", IE_Pressed, this, &AShooterPC::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &AShooterPC::StopJumping);

	// Mouse click
	InputComponent->BindAxis("LeftButton", this, &AShooterPC::LeftClick);
	//InputComponent->BindAction("LeftButton", IE_Released, this, &AShooterPC::LeftClickRelease);
	InputComponent->BindAxis("RightButton", this, &AShooterPC::RightClick);
	//InputComponent->BindAction("RightButton", IE_Released, this, &AShooterPC::RightClickRelease);

	// Movement
	InputComponent->BindAxis("Move", this, &AShooterPC::Move);
	InputComponent->BindAxis("Strafe", this, &AShooterPC::Strafe);
	InputComponent->BindAxis("Sprint", this, &AShooterPC::Sprint);

	// Scrolling
	InputComponent->BindAction("ScrollUp", IE_Pressed, this, &AShooterPC::ScrollUp);
	InputComponent->BindAction("ScrollDown", IE_Pressed, this, &AShooterPC::ScrollDown);

	// Interact
	InputComponent->BindAction("Interact", IE_Pressed, this, &AShooterPC::Interact);
	InputComponent->BindAction("Interact", IE_Released, this, &AShooterPC::InteractRelease);

	// Reload
	InputComponent->BindAction("Reload", IE_Pressed, this, &AShooterPC::Reload);

	// Weapon Swap
	InputComponent->BindAction("WeaponSwap", IE_Pressed, this, &AShooterPC::WeaponSwap);

	InputComponent->BindAction("CharacterScreen", IE_Pressed, this, &AShooterPC::CharacterScreen);
}

void AShooterPC::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<AShooterCharacter>(GetCharacter());

	// Spawn the camera that will follow the character
	if (GetCharacter() != nullptr && TopDownCameraBP != nullptr)
	{
		FVector SpawnLoc = FVector(PlayerCharacter->GetActorLocation().X, PlayerCharacter->GetActorLocation().Y, PlayerCharacter->GetActorLocation().Z + 300.0f);
		TopDownCamera = SpawnBP<ATopDownCamera>(GetWorld(), TopDownCameraBP, SpawnLoc, FRotator(0.0f, 0.0f, 0.0f));
		SetViewTarget(TopDownCamera);

		TopDownCamera->SetFollowTarget(PlayerCharacter);
		TopDownCamera->SetPlayerCharacterAndController(PlayerCharacter, this);
	}
	
	// Spawn the HUD Widget, make sure to only spawn it on the local controller
	if (IsLocalPlayerController() && PlayerCharacter && PlayerHudWidgetClass != nullptr)
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(this, PlayerHudWidgetClass);
		
		// Add it to viewport and set owners
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();
			PlayerHUDWidget->SetControllerAndCharacter();
		}
	}

	// Interact Trace params
	TraceParams = FCollisionQueryParams(FName(TEXT("Interact Trace")), false, nullptr);
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = false;
}

void AShooterPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RotateTowardsMouse && PlayerCharacter && !PlayerCharacter->CharacterDead && TopDownCamera)
	{
		// Create plane with the actor location and the up vector
		FPlane RotationPlane = FPlane(PlayerCharacter->GetActorLocation(), FVector::UpVector);

		// Get mouse location in the world
		FVector MouseWorldDir;
		FVector MouseWorldPosition;
		DeprojectMousePositionToWorld(MouseWorldPosition, MouseWorldDir);

		// Find out where a line between the camera and the mouse world position intersects with the plane.
		FVector IntersectPoint;
		IntersectPoint = FMath::LinePlaneIntersection(TopDownCamera->GetActorLocation(), MouseWorldPosition, RotationPlane);

		// Set the muzzle look at loc using the intersect point.
		PlayerCharacter->SetMuzzleLookAt(IntersectPoint);

		// Get the direction towards the intersect and set the rotation on the character
		FVector IntersectDirection = IntersectPoint - PlayerCharacter->GetActorLocation();
		//PlayerCharacter->SetActorRotation(FRotator(0.0f, IntersectDirection.Rotation().Yaw, 0.0f));
		PlayerCharacter->SetCharacterRot(IntersectDirection);
	}

	if (TraceForInteractions && TopDownCamera && PlayerCharacter && PlayerHUDWidget)
	{
		// Get mouse position in world
		FVector MouseWorldDir;
		FVector MouseWorldPosition;
		DeprojectMousePositionToWorld(MouseWorldPosition, MouseWorldDir);

		// Create a direction based on mouse position in the world and the camera
		FVector TraceDirection = MouseWorldPosition - TopDownCamera->GetActorLocation();
		TraceDirection.Normalize();

		// Do a trace to see if any interactable objects can be found under the mouse
		FHitResult HitResult(ForceInit);
		FVector StartFVector = TopDownCamera->GetActorLocation();
		FVector EndFVector = StartFVector + (TraceDirection * 3000.0f);

		// If the trace hits a object
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartFVector, EndFVector, CHANNEL_INTERACT_TRACE, TraceParams))
		{
			//PrintText("Hit Something", -1, 0.0f);
			
			// Set the hit object to be the hovered interactable actor
			if (HitResult.GetActor() != nullptr && HitResult.GetActor()->ActorHasTag("Interactable"))
			{
				AInteractableActor* InteractActor = Cast<AInteractableActor>(HitResult.GetActor());
				
				// If it's a loot container, show tooltip on the hud
				if (InteractActor && InteractActor->ActorHasTag("LootContainer"))
				{
					AIA_Container_Weapon* InteractWeaponCont = Cast<AIA_Container_Weapon>(InteractActor);

					if (InteractWeaponCont)
						PlayerHUDWidget->ShowWeaponTooltip(InteractWeaponCont->StoredWeapon);
				}

				// Check the distance between the character and interact object
				float Distance = FVector::Dist(InteractActor->GetActorLocation(), PlayerCharacter->GetActorLocation());

				// Only set the hovered actor if it can be interacted using the interact key and if the player is in range of it.
				if (InteractActor && !InteractActor->RangeTriggeredInteraction && Distance < InteractActor->InteractRange)
				{
					if (HoveredActor != InteractActor)
					{
						HoveredActor = InteractActor;
					}
				}
				else
				{
					HoveredActor = nullptr;
					//PlayerHUDWidget->HideWeaponTooltip();
				}
			}
		}
		else
		{
			//PrintText("Did Not Hit Something", -1, 0.0f);
			
			// Reset the variable when nothing is hit.
			HoveredActor = nullptr;
			PlayerHUDWidget->HideWeaponTooltip();
		}
		//DrawDebugLine(GetWorld(), StartFVector, EndFVector, FColor::Red, false, 0.0f, 0, 5.0f);
	}
}

//--- Input ---//

//--- Mouse
void AShooterPC::LeftClick(float Value)
{
	LeftButtonDown = (Value != 0);

	if (PlayerHUDWidget)
		PlayerHUDWidget->LeftButtonDown = LeftButtonDown;

	if (PlayerCharacter == nullptr || (PlayerCharacter && PlayerCharacter->EquippedWeapon == nullptr) || (PlayerCharacter && PlayerCharacter->CharacterDead))
		return;

	if (Value != 0 && !PlayerCharacter->GetCharacterMovement()->IsFalling())
	{
		// First check if the character should reload, if there is no ammo left to use reload the weapon instead
		if (PlayerCharacter->EquippedWeapon->AmmoCountCurrent == 0)
		{
			PlayerCharacter->ReloadWeapon();
		}
		else if(PlayerCharacter->EquippedWeapon->CanFire())
		{
			// Call the fire weapon function on the base class.
			PlayerCharacter->FireWeapon();
		}
	}
}

void AShooterPC::RightClick(float Value)
{
	RightButtonDown = (Value != 0);

	if (PlayerCharacter == nullptr || (PlayerCharacter && PlayerCharacter->CharacterDead))
		return;
}

////--- Movement
void AShooterPC::Move(float Value)
{
	if (PlayerCharacter == nullptr || (PlayerCharacter && PlayerCharacter->CharacterDead))
		return;

	if (Value != 0.0f && !IgnoreMovementInput)
	{
		MovingForward = true;
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = PlayerCharacter->NormalWalkSpeed;
		//PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = (SprintKeyDown == true) ? PlayerCharacter->NormalWalkSpeed * PlayerCharacter->SprintMultiplier : PlayerCharacter->NormalWalkSpeed;
		PlayerCharacter->AddMovementInput(FVector::ForwardVector, Value);
	}
	else
	{
		MovingForward = false;
	}
}

void AShooterPC::Strafe(float Value)
{
	if (PlayerCharacter == nullptr || (PlayerCharacter && PlayerCharacter->CharacterDead))
		return;

	if (Value != 0.0f && !IgnoreMovementInput)
	{
		Strafing = true;
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = PlayerCharacter->NormalWalkSpeed;
		//PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = (SprintKeyDown == true) ? PlayerCharacter->NormalWalkSpeed * PlayerCharacter->SprintMultiplier : PlayerCharacter->NormalWalkSpeed;
		PlayerCharacter->AddMovementInput(FVector::RightVector, Value);
	}
	else
	{
		Strafing = false;
	}
}

void AShooterPC::Jump()
{
	if (PlayerCharacter == nullptr || (PlayerCharacter && PlayerCharacter->CharacterDead))
		return;

	if (!IgnoreMovementInput && PlayerCharacter->GetCharacterMovement()->IsWalking())
	{
		PlayerCharacter->GetCharacterMovement()->JumpZVelocity = (MovingForward || Strafing == true) ? PlayerCharacter->RunningJumpZVelocity : PlayerCharacter->StandingJumpZVelocity;
		PlayerCharacter->Jump();
		PlayerCharacter->SetEnableJumpAnim(true);
	}

	//PrintText("Jump", -1, 2.0f, FColor::Green);
}

void AShooterPC::StopJumping()
{
	if (PlayerCharacter == nullptr || (PlayerCharacter && PlayerCharacter->CharacterDead))
		return;

	PlayerCharacter->StopJumping();

	//PrintText("StopJumping", -1, 2.0f, FColor::Green);
}

void AShooterPC::Sprint(float Value)
{
	if (PlayerCharacter == nullptr || (PlayerCharacter && PlayerCharacter->CharacterDead))
		return;

	/*SprintKeyDown = Value != 0;*/

	//if (Value != 0)
	//{
	//	SprintKeyDown = true;
	//	//PrintText("Sprint", -1, 2.0f, FColor::Green);
	//}
	//else
	//{
	//	SprintKeyDown = false;
	//}
}

void AShooterPC::ScrollUp()
{
//	//PrintText("ScrollUp", -1, 2.0f, FColor::Green);
//	TopDownCamera->FollowHeight -= 30.0f;
//	TopDownCamera->FollowHeight = FMath::Clamp(TopDownCamera->FollowHeight, TopDownCamera->FollowHeightMin, TopDownCamera->FollowHeightMax);
//
//	TopDownCamera->AwayDistance -= 10.0f;
//	TopDownCamera->AwayDistance = FMath::Clamp(TopDownCamera->AwayDistance, TopDownCamera->AwayDistanceMin, TopDownCamera->AwayDistanceMax);
}

void AShooterPC::ScrollDown()
{
//	//PrintText("ScrollDown", -1, 2.0f, FColor::Green);
//	TopDownCamera->FollowHeight += 30.0f;
//	TopDownCamera->FollowHeight = FMath::Clamp(TopDownCamera->FollowHeight, TopDownCamera->FollowHeightMin, TopDownCamera->FollowHeightMax);
//
//	TopDownCamera->AwayDistance += 10.0f;
//	TopDownCamera->AwayDistance = FMath::Clamp(TopDownCamera->AwayDistance, TopDownCamera->AwayDistanceMin, TopDownCamera->AwayDistanceMax);
}

void AShooterPC::Interact()
{
	if (PlayerCharacter == nullptr || (PlayerCharacter && PlayerCharacter->CharacterDead))
		return;

	if (HoveredActor && HoveredActor->Active && !HoveredActor->InteractedWith)
	{
		// Tell the character that it the player wants to interact with the hovered actor.
		PlayerCharacter->InteractWith(HoveredActor);
	}
}

void AShooterPC::InteractRelease()
{
	if (PlayerCharacter == nullptr || (PlayerCharacter && PlayerCharacter->CharacterDead))
		return;

}

void AShooterPC::Reload()
{
	if (PlayerCharacter == nullptr || (PlayerCharacter && PlayerCharacter->CharacterDead))
		return;

	PlayerCharacter->ReloadWeapon();
}

void AShooterPC::WeaponSwap()
{
	if (PlayerCharacter == nullptr || (PlayerCharacter && PlayerCharacter->CharacterDead))
		return;

	PlayerCharacter->SwitchEquippedWeapons();
}

void AShooterPC::CharacterScreen()
{
	if (PlayerCharacter == nullptr || PlayerHUDWidget == nullptr)
		return;

	// Update it if it should show
	if (!PlayerHUDWidget->CharacterScreenVisible)
		PlayerHUDWidget->UpdateCharacterScreen();

	// Toggle the screen
	PlayerHUDWidget->CharacterScreenVisible = !PlayerHUDWidget->CharacterScreenVisible;
	
}