#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "DrJonesWidgetBase.generated.h"

class ADrJonesCharacter;
class UWidgetDataObject;

UCLASS()
class UDrJonesWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnDataUpdated();

	virtual void UpdateData() { OnDataUpdated(); }
	UDrJonesWidgetBase* GetChildWidget(TSubclassOf<UDrJonesWidgetBase> WidgetClass);
	UWidgetDataObject* GetUpdater() const { return Updater; }

	template<typename WidgetClass>
	WidgetClass* GetUpdater() const { return Cast<WidgetClass>(Updater); }

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ADrJonesCharacter> Player;

	UPROPERTY(Transient)
	TMap<TSubclassOf<UDrJonesWidgetBase>, TObjectPtr<UDrJonesWidgetBase>> ChildWidgets;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UWidgetDataObject> UpdaterClass;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UWidgetDataObject> Updater;
};

UCLASS(BlueprintType)
class DR_JONES_API UWidgetDataObject : public UObject
{
	GENERATED_BODY()

public:
	virtual void ResetValuesAfterUsage() {}
};
