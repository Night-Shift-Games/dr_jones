// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tool.h"
#include "Player/DrJonesCharacter.h"

#include "Shovel.generated.h"

UCLASS(ClassGroup = (Item), Blueprintable)
class DR_JONES_API AShovel : public ATool
{
	GENERATED_BODY()

public:
	void PlayFX(const FHitResult& Hit);
	void FillShovel();

protected:
	UFUNCTION(BlueprintCallable, Category = "DrJones")
	void Dig();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tool Settings")
	float ShovelStrength = 15;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tool Settings");
	float ShovelReach = 250;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tool Settings|FX")
	TObjectPtr<UStaticMesh> ShovelDirt;
	
protected:
	TObjectPtr<UStaticMeshComponent> DirtComponent;
	TObjectPtr<ADrJonesCharacter> Player;
	bool bFilled = false;
};
