// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimerManager.h"

#include "ActionComponent.generated.h"

class ADrJonesCharacter;
class UAnimSequence;
class UBlendSpace;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAction);

DECLARE_DELEGATE_OneParam(FActionMontageBehaviorDelegate, ADrJonesCharacter*);

UCLASS(ClassGroup = "Shared Components", meta = (BlueprintSpawnableComponent))
class DR_JONES_API UActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UActionComponent();
	
	UFUNCTION(BlueprintCallable, Category = "DrJones")
	virtual void CallPrimaryAction(ADrJonesCharacter* Character);

	UFUNCTION(BlueprintCallable, Category = "DrJones")
	virtual void CallSecondaryAction(ADrJonesCharacter* Character);

private:
	void PlayPrimaryActionMontage(ADrJonesCharacter* Character) const;
	void PlaySecondaryActionMontage(ADrJonesCharacter* Character) const;
	
public:
	FActionMontageBehaviorDelegate PrimaryActionMontageBehaviorDelegate;
	FActionMontageBehaviorDelegate SecondaryActionMontageBehaviorDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "DrJones|Delegates")
	FAction PrimaryActionDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "DrJones|Delegates")
	FAction SecondaryActionDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DrJones|Animation")
	TObjectPtr<UAnimMontage> PrimaryActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DrJones|Animation")
	TObjectPtr<UAnimMontage> SecondaryActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DrJones|Animation")
	TObjectPtr<UBlendSpace> MovementBlendSpaceOverride;
	
	FTimerHandle AnimationTimer;
};
