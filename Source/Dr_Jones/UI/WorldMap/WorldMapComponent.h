// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WorldMapComponent.generated.h"


class UWorldMapWidget;
class UWidgetComponent;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), Category = "UI|World Map", meta=(BlueprintSpawnableComponent))
class DR_JONES_API UWorldMapComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWorldMapComponent();

	virtual void BeginPlay() override;

	virtual void OnComponentCreated() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION(BlueprintPure, Category = "UI|World Map")
	UWidgetComponent* GetWidgetComponent() const;

	UPROPERTY(BlueprintReadOnly, Category = "UI|World Map")
	TObjectPtr<UWorldMapWidget> WorldMapWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI|World Map")
	FComponentReference WidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI|World Map")
	TSubclassOf<UWorldMapWidget> WidgetClass;
};
