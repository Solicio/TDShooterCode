// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TopDownCamera.h"

#include "Equipment/Weapons/BaseWeapon.h"

#include "Interaction/InteractableActor.h"

#include "Global/ShooterStaticLibrary.h"

#include "GameFramework/PlayerController.h"
#include "ShooterPC.generated.h"

class AShooterCharacter;
class UPlayerHUDWidget;

/**
 * 
 */
UCLASS()
class TDSHOOTER_API AShooterPC : public APlayerController
{
	GENERATED_BODY()
	
	public:

		AShooterPC();
	
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
		bool ShouldPrintInfo;
	

		//--- Movement/Input Variables ---//

		/** If mouse input should be ignored for rotation of camera  */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement/Input")
		bool IgnoreMouseLookInput;

		/** If the key inputs should be ignored for character movement  */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement/Input")
		bool IgnoreMovementInput;

		/** If the player character should be rotated towards the mouse  */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement/Input")
		bool RotateTowardsMouse;


		/** If the key inputs should be ignored for character movement  */
		//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement/Input")
		//bool SprintKeyDown;

		/** If the key inputs should be ignored for character movement */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement/Input")
		bool MovingForward;

		/** If the key inputs should be ignored for character movement  */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement/Input")
		bool Strafing;


		/** If the left button is down, mianly used for firing. Left Click / Controller button */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement/Input")
		bool LeftButtonDown;

		/** If the right button is down. Left Click / Controller button */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement/Input")
		bool RightButtonDown;


		//--- Camera Variables ---//

		/** Used for spawning the camera that will follow the player */
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
		TSubclassOf<ATopDownCamera> TopDownCameraBP;

		/** The camera the player is using. */
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
		ATopDownCamera* TopDownCamera;


		//--- Interaction Variables ---//


		/** If the player controller should do line traces to see if there is anything interactable under the mouse */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		bool TraceForInteractions;

		/** The interactable actor the player is currently hovering with the mouse */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		AInteractableActor* HoveredActor;
		

		//--- Weapon Variables ---//

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool CanFireWeapon;


		//--- HUD Variables ---//
		
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD")
		TSubclassOf<class UPlayerHUDWidget> PlayerHudWidgetClass;

		/** Reference to the players hud widget */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
		UPlayerHUDWidget* PlayerHUDWidget;


	protected:

		UPROPERTY()
		AShooterCharacter* PlayerCharacter;

		// Trace parameters for the interact line trace
		FCollisionQueryParams TraceParams;

		/** Called for forwards/backward input*/
		void Move(float Value);

		/** Called for side to side input*/
		void Strafe(float Value);

		/** Called when the player wants to jump and override for when the character lands */
		void Jump();
		void StopJumping();

		/** Used for checking sprint key*/
		void Sprint(float Value);

		/** Left clicking */
		void LeftClick(float Value);
		//void LeftClickRelease();

		/** Right clicking */
		void RightClick(float Value);
		//void RightClickRelease();

		/** Scrolling */
		void ScrollUp();
		void ScrollDown();

		/** Interact */
		void Interact();
		void InteractRelease();
		
		/** Reload */
		void Reload();

		/** Weapon Swap */
		void WeaponSwap();

		/** Character Screen */
		void CharacterScreen();

		/** Called to bind functionality to input */
		virtual void SetupInputComponent() override;

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;

		/** Called every frame */
		virtual void Tick(float DeltaTime) override;
};
