#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "Player/DrJonesCharacter.h"

#include "DrJonesWidgetBase.generated.h"

UCLASS()
class UDrJonesWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ADrJonesCharacter> Player;
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateData();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateDataAxis(float AxisValue = 0);
};
