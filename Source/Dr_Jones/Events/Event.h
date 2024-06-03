// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "Event.generated.h"

UCLASS(Blueprintable, Category = "DrJones")
class DR_JONES_API UEvent : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, Category = "DrJones|Event", DisplayName = "Can Be Triggered")
	bool CanBeTriggeredDynamic() const;
	
	virtual bool CanBeTriggered() const;

	UFUNCTION(BlueprintCallable, Category = "DrJones|Event", DisplayName = "Get Game Instance")
	UGameInstance* GetGameInstance() const;

	virtual void Trigger();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText EventName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ProbabilityOfEvent = 0.01;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsOneShot = false;

};
