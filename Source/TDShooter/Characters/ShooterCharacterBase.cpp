// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "ShooterCharacterBase.h"


// Sets default values
AShooterCharacterBase::AShooterCharacterBase()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthMax = 500.0f;
	CharacterDead = false;
	ShouldRadgollOnDeath = true;

	InitialMeshLoc = FVector(0.0f, 0.0f, -96.0f);
	InitialMeshRot = FRotator(0.0f, -90.0f, 0.0f);

	// Set the default to none, since we need there to be a change to the variable to set the collision responses.
	// Required for rep notify to trigger.
	Faction = EFactions::F_None;

	// Tag that all character will have.
	Tags.Add("ShooterCharacter");
}

void AShooterCharacterBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AShooterCharacterBase, EnableJumpAnim, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AShooterCharacterBase, EquippedWeapon, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AShooterCharacterBase, SecondaryEquippedWeapon, COND_OwnerOnly);
	//DOREPLIFETIME(AShooterCharacterBase, EquippedWeapon);
	//DOREPLIFETIME(AShooterCharacterBase, SecondaryEquippedWeapon);
	DOREPLIFETIME(AShooterCharacterBase, Faction);
	DOREPLIFETIME(AShooterCharacterBase, HealthCurrent);
	DOREPLIFETIME(AShooterCharacterBase, CharacterDead);
}

// Called when the game starts or when spawned
void AShooterCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	HealthCurrent = HealthMax;

	// Set so the mesh does not collide whit projectiles
	GetMesh()->SetCollisionResponseToChannel(COLLISION_PROJ_F1, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_PROJ_F2, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_PROJ_F3, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(COLLISION_PROJ_F4, ECollisionResponse::ECR_Overlap);

	// Set the initial faction on the server
	if (Role == ROLE_Authority)
		SetFaction(InitialFaction);
}

// Called every frame
void AShooterCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If the character is dead but is moving for some reason, stop it
	if (Role == ROLE_Authority && CharacterDead && GetCharacterMovement()->Velocity.Size() > 0.0f)
		GetCharacterMovement()->Velocity = FVector::ZeroVector;

	if (ShouldPrintInfo && Role == ROLE_Authority)
	{
		if (EquippedWeapon)
		{
			if (!InfiniteAmmo)
			{
				// Look through the ammo container array and find the matching weapon type element
				FAmmoType& DesiredElement = *(AmmoContainer.AmmoTypes.FindByPredicate([&](FAmmoType Type) { return Type.AmmoType == EquippedWeapon->WeaponType; }));
				PrintText(GetName() + ": " + GetEnumValueToString<EWeaponTypes>("EWeaponTypes", EquippedWeapon->WeaponType) + ": " + FString::FromInt(EquippedWeapon->AmmoCountCurrent) + " / " + FString::FromInt(DesiredElement.AmmoCount), -1, 0.0f, FColor::Green);
			}
			else
			{
				PrintText(GetName() + ": " + GetEnumValueToString<EWeaponTypes>("EWeaponTypes", EquippedWeapon->WeaponType) + ": " + FString::FromInt(EquippedWeapon->AmmoCountCurrent) + " /" + " infinite", -1, 0.0f, FColor::Green);
				PrintText(GetName() + ", HP: " + FString::SanitizeFloat(HealthCurrent) + " / " + FString::SanitizeFloat(HealthMax), -1, 0.0f, FColor::Red);
			}
		}
	}
}

bool AShooterCharacterBase::EquipWeapon(ABaseWeapon* NewWeapon, int32 Slot)
{
	if (NewWeapon == nullptr && Role == ROLE_Authority)
		return false;

	// Should drop currently equiped weapon if there already is a weapon in the secondary slot.

	// If there already is a equiped weapon but there is an empty space in the secondary slot
	// Move the currently equiped weapon into that slot
	//if (EquippedWeapon != nullptr && SecondaryEquippedWeapon == nullptr)
	//{
	//	SecondaryEquippedWeapon = EquippedWeapon;

	//	// Move the new secondary weapon to the characters back
	//	if (SecondaryEquippedWeapon)
	//	{
	//		if (SecondaryEquippedWeapon->WeaponType == EWeaponTypes::WT_RocketLauncher)
	//			SecondaryEquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "Socket_BackWeapon_RL");
	//		else
	//			SecondaryEquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "Socket_BackWeapon");
	//	}

	//	EquippedWeapon = nullptr;
	//}


	// If the character has weapons in both slots, drop the weapon on the ground.
	//else if (EquippedWeapon != nullptr && SecondaryEquippedWeapon != nullptr)
	//{
	//	// DROP WEAPON ON FLOOR

	//	// Destroy the weapon actor.
	//	EquippedWeapon->Destroy();
	//	EquippedWeapon = nullptr;
	//}

	// Main weapon slot
	if (Slot == 0)
	{
		// Set the new weapon and attach it to the character
		if (EquippedWeapon == nullptr)
		{
			EquippedWeapon = NewWeapon;
			
			if (EquippedWeapon->WeaponType == EWeaponTypes::WT_RocketLauncher)
				EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "Socket_RightHand_RL");
			else
				EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "Socket_RightHand");

			UpdateEquippedInvWeapons(false);
			return true;
		}
	}
	// Secondary weapon slot
	else if (Slot == 1)
	{
		// Set the new weapon and attach it to the character
		if (SecondaryEquippedWeapon == nullptr)
		{
			SecondaryEquippedWeapon = NewWeapon;

			if (SecondaryEquippedWeapon->WeaponType == EWeaponTypes::WT_RocketLauncher)
				SecondaryEquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "Socket_BackWeapon_RL");
			else
				SecondaryEquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "Socket_BackWeapon");

			UpdateEquippedInvWeapons(false);
			return true;
		}
	}


	return false;
}

bool AShooterCharacterBase::FireWeapon()
{
	// On client, call the server function
	if (Role < ROLE_Authority)
	{
		ServerFireWeapon();
		return false;
	}

	// On the server but there is no weapon
	if (Role == ROLE_Authority && EquippedWeapon == nullptr)
		return false;

	// If the weapon has no ammo or if it cannot be fire atm dure to fire or reload delay stop trying to fire.
	if (EquippedWeapon->AmmoCountCurrent == 0 || !EquippedWeapon->CanFire())
		return false;

	// Fire the weapon. The weapon returns true if the weapon fired
	// Note: Only the server will get the return value. (Since Server functions cannot have return values)
	bool Success = EquippedWeapon->FireWeapon(this, Faction);
	
	RefreshEquippedInvWeapons();
	
	return Success;
}

void AShooterCharacterBase::ServerFireWeapon_Implementation()
{
	// Call the previous function to fire the projectiles on the server
	FireWeapon();
}

bool AShooterCharacterBase::ServerFireWeapon_Validate()
{
	return true;
}


bool AShooterCharacterBase::ReloadWeapon()
{
	// On client, call the server function
	if (Role < ROLE_Authority)
	{
		ServerReloadWeapon();
		return false;
	}

	// On the server but there is no weapon
	if (Role == ROLE_Authority && EquippedWeapon == nullptr)
		return false;

	// If character cannot reload
	if (EquippedWeapon->AmmoCountCurrent >= EquippedWeapon->ScaledStats.MagazineSize || EquippedWeapon->ReloadDelay > 0.0f)
		return false;

	// If the character has infinite ammo, just refill the weapon, if there is ammo missing and is not currently reloading.
	if (InfiniteAmmo)
	{
		EquippedWeapon->AmmoCountCurrent = EquippedWeapon->ScaledStats.MagazineSize;
		EquippedWeapon->ReloadWeapon();
		//PlayReloadAnim();
		RefreshEquippedInvWeapons();
		return true;
	}
	
	for (auto Itr(AmmoContainer.AmmoTypes.CreateIterator()); Itr; Itr++)
	{
		if (EquippedWeapon->WeaponType == (*Itr).AmmoType)
		{
			// First check how much is needed to be added to the current weapon to max it out
			int32 MissingFromWeapon = EquippedWeapon->ScaledStats.MagazineSize - EquippedWeapon->AmmoCountCurrent;

			// If the container does not contain enough to fill the weapon, take everything that is left in the container
			int32 SubtractedFromContainer = ((*Itr).AmmoCount < MissingFromWeapon) ? (*Itr).AmmoCount : MissingFromWeapon;

			// Fill the weapon and remove from the container
			(*Itr).AmmoCount -= SubtractedFromContainer;
			EquippedWeapon->AmmoCountCurrent += SubtractedFromContainer;
			EquippedWeapon->ReloadWeapon();
			//PlayReloadAnim();
		}
	}

	RefreshEquippedInvWeapons();
	return true;
}

void AShooterCharacterBase::ServerReloadWeapon_Implementation()
{
	// Call the previous function to reload the weapon on the server
	ReloadWeapon();
}

bool AShooterCharacterBase::ServerReloadWeapon_Validate()
{
	return true;
}

bool AShooterCharacterBase::SwitchEquippedWeapons()
{
	// On client, call the server function
	if (Role < ROLE_Authority)
	{
		ServerSwitchEquippedWeapons();
		return false;
	}

	// Stop player from switching when the weapon is reloading
	if (EquippedWeapon && (EquippedWeapon->ReloadDelay > 0.0f))
		return false;	

	ABaseWeapon* TempWeapon = EquippedWeapon;
	
	// Switch the places
	EquippedWeapon = SecondaryEquippedWeapon;
	SecondaryEquippedWeapon = TempWeapon;

	// Set the new equiped weapon into the characters hands
	if (EquippedWeapon)
	{
		// Make it so the player can't fire for a bit when switching weapons.
		EquippedWeapon->FireDelay = 0.5f;

		if (EquippedWeapon->WeaponType == EWeaponTypes::WT_RocketLauncher)
			EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "Socket_RightHand_RL");
		else
			EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "Socket_RightHand");
	}
	
	// Move the new secondary weapon to the characters back
	if (SecondaryEquippedWeapon)
	{
		if (SecondaryEquippedWeapon->WeaponType == EWeaponTypes::WT_RocketLauncher)
			SecondaryEquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "Socket_BackWeapon_RL");
		else
			SecondaryEquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "Socket_BackWeapon");
	}

	UpdateEquippedInvWeapons(true);
	return true;
}

void AShooterCharacterBase::ServerSwitchEquippedWeapons_Implementation()
{
	// Call the previous function to reload the weapon on the server
	SwitchEquippedWeapons();
}

bool AShooterCharacterBase::ServerSwitchEquippedWeapons_Validate()
{
	return true;
}

void AShooterCharacterBase::SetCharacterDead(bool IsDead)
{
	if (Role == ROLE_Authority)
	{
		CharacterDead = IsDead;
		OnRep_CharacterDead();
	}
}

void AShooterCharacterBase::OnRep_CharacterDead()
{
	if (CharacterDead)
	{
		// Make the character freeze
		GetCharacterMovement()->GravityScale = 0.0f;
		GetCharacterMovement()->Velocity = FVector::ZeroVector;

		// Turn on the ragdoll
		if(ShouldRadgollOnDeath)
			GetMesh()->SetSimulatePhysics(true);
		
		// Set the collision so that everything passes through the capsule
		GetCapsuleComponent()->SetCollisionProfileName("PawnDead");
	}
	else if (!CharacterDead)
	{
		// Make the character unfreeze
		GetCharacterMovement()->GravityScale = 1.0f;

		// Turn off the ragdoll
		if (ShouldRadgollOnDeath)
		{
			GetMesh()->SetSimulatePhysics(false);
			GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			GetMesh()->SetRelativeLocation(InitialMeshLoc);
			GetMesh()->SetRelativeRotation(InitialMeshRot);
		}

		// Set the collision back to the default. Also setting the faction projectile collision responses.
		GetCapsuleComponent()->SetCollisionProfileName("Pawn");
		OnRep_CollisionRespone();
	}

}

void AShooterCharacterBase::SetEnableJumpAnim(bool NewValue)
{
	EnableJumpAnim = NewValue;

	// If the next statement succeeds, we are not the authority. We are client, which means we need to call it on the server too.
	if (Role == ROLE_AutonomousProxy)
	{
		ServerSetEnableJumpAnim(NewValue);
	}
}

void AShooterCharacterBase::ServerSetEnableJumpAnim_Implementation(bool NewValue)
{
	// Call the previous function to once again set the value, but this time on the server.
	SetEnableJumpAnim(NewValue);
}

bool AShooterCharacterBase::ServerSetEnableJumpAnim_Validate(bool NewValue)
{
	return true;
}



void AShooterCharacterBase::SetFaction(EFactions NewFaction)
{	
	// Make sure that we are on the server
	if (Role == ROLE_Authority)
	{
		Faction = NewFaction;
		OnRep_CollisionRespone();
	}
}

void AShooterCharacterBase::OnRep_CollisionRespone()
{
	// Depending on what new faction the character is set as. Set the different collision responses to the different projectiles.
	// If the faction is set to Faction1, the faction1 projectiles should not collide, but the others should.
	switch (Faction)
	{
		case EFactions::F_Faction1:
			GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJ_F1, ECollisionResponse::ECR_Overlap);
			GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJ_F2, ECollisionResponse::ECR_Block);
			GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJ_F3, ECollisionResponse::ECR_Block);
			GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJ_F4, ECollisionResponse::ECR_Block);
			break;
		case EFactions::F_Faction2:
			GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJ_F1, ECollisionResponse::ECR_Block);
			GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJ_F2, ECollisionResponse::ECR_Overlap);
			GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJ_F3, ECollisionResponse::ECR_Block);
			GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJ_F4, ECollisionResponse::ECR_Block);
			break;
		case EFactions::F_Faction3:
			GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJ_F1, ECollisionResponse::ECR_Block);
			GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJ_F2, ECollisionResponse::ECR_Block);
			GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJ_F3, ECollisionResponse::ECR_Overlap);
			GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJ_F4, ECollisionResponse::ECR_Block);
			break;
		case EFactions::F_Faction4:
			GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJ_F1, ECollisionResponse::ECR_Block);
			GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJ_F2, ECollisionResponse::ECR_Block);
			GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJ_F3, ECollisionResponse::ECR_Block);
			GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_PROJ_F4, ECollisionResponse::ECR_Overlap);
			break;
	};
	
}


void AShooterCharacterBase::SetCharacterRot(FVector Direction)
{
	SetActorRotation(FRotator(0.0f, Direction.Rotation().Yaw, 0.0f));

	// If the next statement succeeds, we are not the authority. We are client, which means we need to call it on the server too.
	if (Role < ROLE_Authority)
	{
		ServerSetCharacterRot(Direction);
	}
}

void AShooterCharacterBase::ServerSetCharacterRot_Implementation(FVector Direction)
{
	// Call the previous function to once again set the rotation, but this time on the server.
	SetCharacterRot(Direction);
}

bool AShooterCharacterBase::ServerSetCharacterRot_Validate(FVector Direction)
{
	return true;
}



void AShooterCharacterBase::PlayReloadAnim()
{
	// If the next statement succeeds, we are the authority. Call the multicast function
	if (Role == ROLE_Authority)
	{
		MulticastPlayReloadAnim();
	}
}

void AShooterCharacterBase::MulticastPlayReloadAnim_Implementation()
{
	// Play the monatge on the client
	PlayAnimMontage(ReloadMontage);
}

bool AShooterCharacterBase::MulticastPlayReloadAnim_Validate()
{
	return true;
}

void AShooterCharacterBase::UpdateEquippedInvWeapons(bool SwappedWeapons)
{

}

void AShooterCharacterBase::RefreshEquippedInvWeapons()
{

}
