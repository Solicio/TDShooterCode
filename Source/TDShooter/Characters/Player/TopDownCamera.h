// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ShooterCharacter.h"

#include "GameFramework/Actor.h"
#include "TopDownCamera.generated.h"

class AShooterPC;

UCLASS()
class TDSHOOTER_API ATopDownCamera : public AActor
{
	GENERATED_BODY()

	public:

		ATopDownCamera();

		/** Root component to hold all the other components */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Top Down Camera")
		USceneComponent* SceneRoot;

		/** Follow camera */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Top Down Camera")
		UCameraComponent* FollowCamera;


		/** If the camera should be following the target. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Top Down Camera")
		bool UseTiltedCameraView;

		/** The character in the game that this camera will follow. */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Top Down Camera")
		AActor* FollowTarget;

		/** If the camera should be following the target. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Top Down Camera")
		bool ShouldFollowTarget;

		/** If the camera should look at a point inbetween the mouse and the character */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Top Down Camera")
		bool UsePointInBetween;

		/** Sets the cameras follow target. Call function with nullptr to remove follow target. */
		UFUNCTION(BlueprintCallable, Category = "Top Down Camera")
		void SetFollowTarget(AActor* Target);

		/** Sets the cameras player character. */
		UFUNCTION(Category = "Top Down Camera")
		void SetPlayerCharacterAndController(AShooterCharacter* Character, AShooterPC* PC);


		//--- Movement Variables ---//

		/** How far above the target the camera should be. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Top Down Camera")
		float FollowHeight;

		/** THe closest point the camera can be to the target */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Top Down Camera")
		float FollowHeightMin;

		/** The furthest point away from the target the camera can be. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Top Down Camera")
		float FollowHeightMax;


		/** How far away the camera is to the player. Only used when UseTiltedCameraView == true. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Top Down Camera")
		float AwayDistance;

		/** The closest the camera can get to the player. Only used when UseTiltedCameraView == true. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Top Down Camera")
		float AwayDistanceMin;

		/** The furthest the camera can get to the player. Only used when UseTiltedCameraView == true. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Top Down Camera")
		float AwayDistanceMax;


		/** How far above the target the camera should be. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Top Down Camera")
		float MaxMouseDistClamp;

		/** How far above the target the camera should be. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Top Down Camera")
		float MouseDistMulti;

		/** How fast the camera will follow the target */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Top Down Camera")
		float FollowSpeed;


	protected:
		
		UPROPERTY()
		AShooterCharacter* PlayerCharacter;

		UPROPERTY()
		AShooterPC* PlayerController;

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

		// Called every frame
		virtual void Tick(float DeltaSeconds) override;
	
};
