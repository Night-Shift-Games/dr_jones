// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DrJonesWidgetBase.h"

#include "ReturnArtifactWidget.generated.h"

class AArtifactCrate;

UCLASS()
class DR_JONES_API UReturnArtifactWidget : public UDrJonesWidgetBase
{
	GENERATED_BODY()

public:
	void OnShow();
	void OnHide();
public:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AArtifactCrate> OwningArtifactCrate;

};
