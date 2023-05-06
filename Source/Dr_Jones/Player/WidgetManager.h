// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "WidgetManager.generated.h"

class UDrJonesWidgetBase;
class ADrJonesCharacter;
class UUserWidget;

UCLASS()
class UWidgetManager : public UActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
	void AddWidget(const TSubclassOf<UDrJonesWidgetBase> WidgetClass);
	void ShowWidget(const TSubclassOf<UDrJonesWidgetBase> Widget);
	void HideWidget(const TSubclassOf<UDrJonesWidgetBase> Widget);
	void RemoveWidget(const TSubclassOf<UDrJonesWidgetBase> Widget);
	UDrJonesWidgetBase* GetWidget(const TSubclassOf<UDrJonesWidgetBase> Widget) const;
	void RequestWidgetUpdate(const TSubclassOf<UDrJonesWidgetBase> Widget, TOptional<float> AxisValue) const;
public:
	UPROPERTY(EditAnywhere, Category = "Widgets", meta = (DisplayName = "Widgets"))
	TSet<TSubclassOf<UDrJonesWidgetBase>> BeginPlayWidgets;
	
	UPROPERTY(Transient)
	TMap<TSubclassOf<UDrJonesWidgetBase>, TObjectPtr<UDrJonesWidgetBase>> Widgets;

	UPROPERTY(Transient)
	TObjectPtr<ADrJonesCharacter> OwningPlayer;

	UPROPERTY(Transient)
	TObjectPtr<APlayerController> OwningController;
	
};
