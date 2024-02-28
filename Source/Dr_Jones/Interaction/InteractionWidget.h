// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/DrJonesWidgetBase.h"

#include "InteractionWidget.generated.h"

UCLASS()
class DR_JONES_API UInteractionWidget : public UDrJonesWidgetBase
{
	GENERATED_BODY()

	virtual bool Initialize() override;
	virtual void UpdateData() override;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool HasAltInteraction = false;
};

UCLASS()
class UInteractionWidgetDataObject : public UWidgetDataObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool HasAltInteraction = false;
};
