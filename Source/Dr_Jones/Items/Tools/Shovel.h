// Property of Night Shift Games, all rights reserved.

#pragma once

#include "Tool.h"

#include "Shovel.generated.h"

UCLASS(ClassGroup = (Item), Blueprintable)
class DR_JONES_API AShovel : public ATool
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "DrJones")
	void FillShovel();

	UFUNCTION(BlueprintCallable, Category = "DrJones")
	void EmptyShovel();

	UFUNCTION()
	void PrimaryActionMontageBehavior();

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
	void MontageCompletedEvent(bool bInterrupted);

private:
	bool TraceDig(FHitResult& OutHit) const;
	bool TraceForDesiredDigLocation(FHitResult& OutHit) const;

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
