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
	UEvent* SampleRandomEvents(float TriggerChanceModifier);

	UFUNCTION(BlueprintCallable, Category = "DrJones|Events")
	void AddForcedEvent(TSubclassOf<UEvent> Event);

	UFUNCTION(BlueprintPure,  Category = "DrJones|Events")
	UEvent* GetEvent(const TSubclassOf<UEvent> EventClass) const;
	
	void HandleEvents(const FDateTime CurrentTime);
	void TriggerEvent(UEvent& EventToTrigger);

	UEvent* PickEventToTrigger(const float TriggerChanceModifier);
	
	UPROPERTY(BlueprintAssignable, Category = "DrJones|Events")
	FOnEventTriggered OnEventTriggered;
	
protected:
	UPROPERTY()
	TArray<TSubclassOf<UEvent>> ForcedEvents;
	
	UPROPERTY()
	TMap<TSubclassOf<UEvent>, TObjectPtr<UEvent>> StaticEvents;
	
	UPROPERTY()
	TMap<TSubclassOf<UEvent>, TObjectPtr<UEvent>> Events;
	
	FDateTime LastAnyEventFiredDate = FDateTime(1922, 11, 4);
};
