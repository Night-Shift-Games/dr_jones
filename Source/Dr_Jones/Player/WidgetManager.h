// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetManager.generated.h"

UCLASS()
class UWidgetManager : public UObject
{
	GENERATED_BODY()
	
public:
	void AddWidget(const TSubclassOf<UUserWidget> WidgetClass);
	void ShowWidget(const TSubclassOf<UUserWidget> Widget);
	void HideWidget(const TSubclassOf<UUserWidget> Widget);
	void RemoveWidget(const TSubclassOf<UUserWidget> Widget);
	void UpdateWidget(const TSubclassOf<UUserWidget> Widget);
public:
	UPROPERTY()
	TMap<TSubclassOf<UUserWidget>, TObjectPtr<UUserWidget>> Widgets;
};
