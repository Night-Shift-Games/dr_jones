// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Event.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "EventSubsystem.generated.h"

inline TAutoConsoleVariable CVarEventSystem (TEXT("NS.Events"), 0, TEXT("Debug info about events"));

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEventTriggered, UEvent*, Event);

UCLASS()
class DR_JONES_API UEventSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	void HandleEvents(const FDateTime CurrentTime);
	void TriggerEvent(UEvent& EventToTrigger);

	UPROPERTY(BlueprintAssignable, Category = "DrJones|Events")
	FOnEventTriggered OnEventTriggered;
	
protected:
	TMap<TSubclassOf<UEvent>, TStrongObjectPtr<UEvent>> Events;
	
	FDateTime LastEventFiredDate = FDateTime(1922, 11, 4);
};
