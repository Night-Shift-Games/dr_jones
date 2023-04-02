// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tool.h"
#include "Player/DrJonesCharacter.h"
#include "ArchaeologicalSite/ArchaeologicalSite.h"

#include "Shovel.generated.h"

UCLASS(ClassGroup = (Item), meta = (BlueprintSpawnableComponent))
class DR_JONES_API AShovel final : public ATool
{
	GENERATED_BODY()

public:

	virtual void UseToolPrimaryAction() override;
	void PlayFX(FHitResult Hit);
	void FillShovel();

protected:

	void Dig();
	TObjectPtr<UStaticMeshComponent> DirtComponent;
	TObjectPtr<UStaticMeshComponent> PlayerReference;
	TObjectPtr<ADrJonesCharacter> Player;
	bool bFilled = false;

public:

	UPROPERTY(EditAnywhere)
	float ShovelStrength = 15;

	UPROPERTY(EditAnywhere);
	float ShovelReach = 250;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> ShovelDirt;

};
