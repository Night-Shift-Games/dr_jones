// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "QuestSystem.h"

#include "QuestTrigger.generated.h"

class UTextRenderComponent;

UCLASS(BlueprintType)
class UQuestTriggerMessage : public UObject, public IQuestMessageInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	FName TriggerIdentifier;
};

UCLASS(BlueprintType, HideCategories = (Navigation, HLOD, Physics, Collision, Rendering, "Path Tracing", Tags, Cooking, Replication, Networking, Input, Actor, "Code View", Mobile, "Ray Tracing", "Asset User Data"))
class DR_JONES_API AQuestTrigger : public ATriggerBox
{
	GENERATED_BODY()

public:
	AQuestTrigger();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

protected:
	UPROPERTY(EditInstanceOnly, Category = "Quest")
	FName TriggerIdentifier;

#if WITH_EDITORONLY_DATA
	TObjectPtr<UTextRenderComponent> LabelComponent;
#endif
};
