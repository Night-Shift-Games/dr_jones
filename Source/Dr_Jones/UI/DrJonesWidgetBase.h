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

	UPROPERTY(Transient)
	TMap<TSubclassOf<UDrJonesWidgetBase>, TObjectPtr<UDrJonesWidgetBase>> ChildWidgets;

public:
	virtual bool Initialize() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateData();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateDataAxis(float AxisValue = 0);

	UDrJonesWidgetBase* GetChildWidget(TSubclassOf<UDrJonesWidgetBase> WidgetClass);
};
