// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactive/InteractiveObject.h"
#include "QuestNote.generated.h"

class UStaticMesh;
class UQuestData;


UCLASS(Blueprintable, BlueprintType, Category="Quest System")
class DR_JONES_API AQuestNote : public AActor, public IInteractiveObject
{
	GENERATED_BODY()

public:
	static FName NoteMeshComponentName;

	AQuestNote();

	/** Call before spawn to set the Quest data*/
	void Initialize(UQuestData& QuestDataObject);

	virtual void OnConstruction(const FTransform& Transform) override;

	// IInteractiveObject overrides

	virtual void Interact(APawn* Indicator) override;

	UFUNCTION(BlueprintCallable, Category="Interactive")
	virtual FString GetInteractSentence() override;

	// End of IInteractiveObject overrides

	UFUNCTION(BlueprintImplementableEvent, Category="Interactive", meta=(DisplayName="On Interact"))
	void K2_OnInteract(APawn* Indicator);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<UStaticMeshComponent> NoteMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true, ExposeOnSpawn=true))
	TObjectPtr<UQuestData> QuestData;
};
