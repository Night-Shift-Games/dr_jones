// Property of Night Shift Games, all rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "ActionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAction);

UCLASS(ClassGroup = "Shared Components", meta = (BlueprintSpawnableComponent))
class DR_JONES_API UActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UActionComponent();
	
	UFUNCTION(BlueprintCallable, Category = "DrJones")
	virtual void CallPrimaryAction();

	UFUNCTION(BlueprintCallable, Category = "DrJones")
	virtual void CallSecondaryAction();
	
public:
	UPROPERTY(BlueprintAssignable, Category = "DrJones|Delegates")
	FAction PrimaryActionDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "DrJones|Delegates")
	FAction SecondaryActionDelegate;
	
};
