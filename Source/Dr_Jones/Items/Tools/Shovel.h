// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tool.h"
#include "ArchaeologicalSite/ExcavationSegment.h"

#include "Shovel.generated.h"

UCLASS(ClassGroup = (Item), Blueprintable)
class DR_JONES_API AShovel : public ATool
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	void FillShovel();
	void EmptyShovel();
	
	UShapeComponent* GetDigCollision() const;

	UFUNCTION(BlueprintPure, Category = "DrJones")
	bool IsFilled() const;

	UFUNCTION(BlueprintCallable, Category = "DrJones")
	void Dig();

	UFUNCTION(BlueprintCallable, Category = "DrJones")
	void Dump();

	UFUNCTION()
	void PrimaryAction();

	UFUNCTION()
	void DispatchMontageNotify(FName NotifyName);

	UFUNCTION()
	void MontageCompletedEvent(bool bInterrupted);

private:
	void DigInExcavationSite(UExcavationSegment& ExcavationSegment, const FVector& Location) const;

	bool TraceDig(FHitResult& OutHit) const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tool Settings")
	float ShovelStrength = 15;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tool Settings");
	float ShovelReach = 250;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tool Settings|FX")
	TObjectPtr<UStaticMesh> ShovelDirt;

private:
	UPROPERTY(Transient)
	mutable TObjectPtr<UShapeComponent> DigCollisionComponent;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMeshComponent> DirtComponent;
};
