// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Components/ActorComponent.h"

#include "JournalComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DR_JONES_API UJournalComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UJournalComponent();

	void SetupPlayerInputComponent(UEnhancedInputComponent& EnhancedInputComponent);

	void OpenJournal();
	
public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> OpenJournalAction;
};
