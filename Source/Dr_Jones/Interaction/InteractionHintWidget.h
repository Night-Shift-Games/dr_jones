// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/DrJonesWidgetBase.h"

#include "InteractionHintWidget.generated.h"

UCLASS()
class DR_JONES_API UInteractionHintWidget : public UDrJonesWidgetBase
{
	GENERATED_BODY()

	virtual bool Initialize() override;
	virtual void UpdateData() override;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bShouldUpdateVisuals = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bShouldBeVisible = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bHasAltInteraction = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FText InteractionText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FText AltInteractionText;
};

UCLASS()
class UInteractionHintWidgetDataObject : public UWidgetDataObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bShouldUpdateVisuals = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bShouldBeVisible = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bHasAltInteraction = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FText InteractionText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FText AltInteractionText;
};
