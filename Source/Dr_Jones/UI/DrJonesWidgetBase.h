#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "DrJonesWidgetBase.generated.h"

UCLASS()
class UDrJonesWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateData();	
};
