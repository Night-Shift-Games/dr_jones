// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DrJonesWidgetBase.h"
#include "Utilities/Utilities.h"

#include "WidgetManager.generated.h"

class ADrJonesCharacter;
class UDrJonesWidgetBase;
class UUserWidget;

UCLASS(Blueprintable, ClassGroup = "Player Components",
	HideCategories = (Variable, Tags, ComponentTick, Activation, AssetUserData, Replication, ComponentReplication, Cooking, Collision))
class UWidgetManager : public UActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
	void AddWidget(const TSubclassOf<UDrJonesWidgetBase> WidgetClass);
	void ShowWidget(const TSubclassOf<UDrJonesWidgetBase> WidgetClass);
	void HideWidget(const TSubclassOf<UDrJonesWidgetBase> WidgetClass);
	void RemoveWidget(const TSubclassOf<UDrJonesWidgetBase> WidgetClass);

	UFUNCTION(BlueprintCallable)
	void SetWidgetVisibility(TSubclassOf<UDrJonesWidgetBase> WidgetClass, ESlateVisibility Visibility) const;

	static void UpdateWidget(const UObject& WorldContextObject, const TSubclassOf<UDrJonesWidgetBase> WidgetClass);
	
	UFUNCTION(BlueprintCallable)
	UDrJonesWidgetBase* GetWidget(const TSubclassOf<UDrJonesWidgetBase> WidgetClass) const;

	template <typename TWidgetClass> 
	static TWidgetClass* GetWidget(const UObject& WorldContextObject, const TSubclassOf<UDrJonesWidgetBase> WidgetClass)
	{
		UDrJonesWidgetBase* FoundWidget = Utilities::GetWidget(WorldContextObject, WidgetClass);
		return FoundWidget ? Cast<TWidgetClass>(FoundWidget) : nullptr;
	}
	
	template<typename TUpdaterClass>
	static TUpdaterClass* GetWidgetUpdater(const UObject& WorldContextObject, const TSubclassOf<UDrJonesWidgetBase> WidgetClass)
	{
		UDrJonesWidgetBase* FoundWidget = Utilities::GetWidget(WorldContextObject, WidgetClass);
		return FoundWidget ? FoundWidget->GetUpdater<TUpdaterClass>() : nullptr;
	}
	
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
