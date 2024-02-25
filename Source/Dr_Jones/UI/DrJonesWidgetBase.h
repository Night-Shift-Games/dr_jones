#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "DrJonesWidgetBase.generated.h"

class ADrJonesCharacter;

UCLASS()
class UDrJonesWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ADrJonesCharacter> Player;

	UPROPERTY(Transient)
	TMap<TSubclassOf<UDrJonesWidgetBase>, TObjectPtr<UDrJonesWidgetBase>> ChildWidgets;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateData();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateDataAxis(float AxisValue = 0);

	UDrJonesWidgetBase* GetChildWidget(TSubclassOf<UDrJonesWidgetBase> WidgetClass);
};
