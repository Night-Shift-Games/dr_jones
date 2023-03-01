// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WorldMapComponent.generated.h"


class UWorldMapWidget;
class UWidgetComponent;


USTRUCT(BlueprintType, Category="UI|World Map")
struct FWorldMapMarker
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Position;
};


UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), Category = "UI|World Map", meta=(BlueprintSpawnableComponent))
class DR_JONES_API UWorldMapComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWorldMapComponent();

	virtual void BeginPlay() override;

	virtual void OnComponentCreated() override;

	UFUNCTION(BlueprintCallable)
	void UpdateConstruct();

	virtual void PostInitProperties() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION(BlueprintPure, Category = "UI|World Map")
	UWidgetComponent* GetWidgetComponent() const;

	UPROPERTY(BlueprintReadOnly, Category = "UI|World Map")
	TObjectPtr<UWorldMapWidget> WorldMapWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI|World Map")
	TObjectPtr<UWidgetComponent> WidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI|World Map")
	TSubclassOf<UWorldMapWidget> WidgetClass;
};
