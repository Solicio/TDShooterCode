// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "TopDownCamera.h"

#include "ShooterPC.h"

// Sets default values
ATopDownCamera::ATopDownCamera()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// The root component
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SceneRoot);

	ShouldFollowTarget = true;
	UseTiltedCameraView = true;
	UsePointInBetween = true;

	FollowHeight = 800.0f;
	FollowHeightMin = 400.0f;
	FollowHeightMax = 800.0f;

	MaxMouseDistClamp = 600.0f;
	MouseDistMulti = 0.3f;

	AwayDistance = 400.0f;
	AwayDistanceMin = 200.0f;
	AwayDistanceMax = 400.0f;

	FollowSpeed = 10.0f;

	bReplicates = false;
}

// Called when the game starts or when spawned
void ATopDownCamera::BeginPlay()
{
	Super::BeginPlay();

	if (UseTiltedCameraView)
	{
		FollowCamera->SetWorldRotation(FRotator(-60.0f, 0.0f, 0.0f));
	}
	else
	{
		FollowCamera->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
	}
}

// Called every frame
void ATopDownCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FollowTarget)
	{
		// Moves the camera into follow position
		// Currently hardcoded with Z being up.
		if (ShouldFollowTarget)
		{
			FVector TargetLocation = FollowTarget->GetActorLocation();
			if (UsePointInBetween)
			{
				// Create plane with actors up vector and a up vector
				FPlane RotationPlane = FPlane(FollowTarget->GetActorLocation(), FVector::UpVector);

				// Get mouse location in the world
				FVector MouseWorldDir(0.0f);
				FVector MouseWorldPosition(0.0f);
				PlayerController->DeprojectMousePositionToWorld(MouseWorldPosition, MouseWorldDir);

				// Find out where a line between the camera and the mouse world position intersects with the plane.
				FVector IntersectPoint;
				IntersectPoint = FMath::LinePlaneIntersection(GetActorLocation(), MouseWorldPosition, RotationPlane);

				// Get a point between the mouse and the target.
				FVector MouseDirection = IntersectPoint - FollowTarget->GetActorLocation();
				float MouseDistance = MouseDirection.Size();
				MouseDistance = FMath::Clamp(MouseDistance, 0.0f, MaxMouseDistClamp);
				MouseDirection.Normalize();

				TargetLocation = FollowTarget->GetActorLocation() + (MouseDirection * (MouseDistMulti * MouseDistance));
			}

			// The location the camera should be.
			if (UseTiltedCameraView)
			{
				//TargetLocation = FollowTarget->GetActorLocation();
				TargetLocation.X -= AwayDistance;
				TargetLocation.Z += FollowHeight;

				// If the camera view is changed during play.
				if (FMath::IsNearlyEqual(FollowCamera->GetComponentRotation().Pitch, -90.0f, 1.0f))
				{
					FollowCamera->SetWorldRotation(FRotator(-60.0f, 0.0f, 0.0f));
				}
			}
			else
			{
				//TargetLocation = FollowTarget->GetActorLocation();
				TargetLocation.Z += FollowHeight;

				// If the camera view is changed during play.
				if (FMath::IsNearlyEqual(FollowCamera->GetComponentRotation().Pitch, -60.0f, 1.0f))
				{
					FollowCamera->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
				}
			}

			// Interpolate from the current camera location towards the target location.
			FVector NewLocation = GetActorLocation();
			NewLocation.X = FMath::FInterpTo(NewLocation.X, TargetLocation.X, DeltaTime, FollowSpeed);
			NewLocation.Y = FMath::FInterpTo(NewLocation.Y, TargetLocation.Y, DeltaTime, FollowSpeed);
			NewLocation.Z = FMath::FInterpTo(NewLocation.Z, TargetLocation.Z, DeltaTime, FollowSpeed);

			SetActorLocation(NewLocation);
		}
	}
}

void ATopDownCamera::SetFollowTarget(AActor* Target)
{
	FollowTarget = (Target != nullptr) ? Target : nullptr;
}

void ATopDownCamera::SetPlayerCharacterAndController(AShooterCharacter* Character, AShooterPC* PC)
{
	if (Character == nullptr && PC == nullptr)
		return;

	PlayerCharacter = Character;
	PlayerController = PC;
}