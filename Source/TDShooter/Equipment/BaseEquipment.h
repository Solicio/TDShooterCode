// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BaseEquipment.generated.h"

/** All the different equipment types there are */
UENUM(BlueprintType)
enum class EEquipmentTypes : uint8
{
	ET_None				UMETA(DisplayName = "None"),
	ET_Weapon			UMETA(DisplayName = "Weapon"),

};

UCLASS()
class TDSHOOTER_API ABaseEquipment : public AActor
{
	GENERATED_BODY()
	
	public:	

		ABaseEquipment();

		/** The type this equipment is */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Equipment")
		EEquipmentTypes EquipmentType;

		/** The name of the base equipment, important, used for weapon spawning etc.*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Equipment")
		FName EquipmentName;

		/** Icon that will be used in inventory */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Equipment")
		UTexture2D* EquipmentIcon;


	protected:

		// Called when the game starts or when spawned
		virtual void BeginPlay() override;
	
		// Called every frame
		virtual void Tick(float DeltaTime) override;
};
