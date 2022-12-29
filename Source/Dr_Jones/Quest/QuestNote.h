// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dr_Jones/Interactive/InteractiveObject.h"
#include "QuestNote.generated.h"

class UStaticMesh;


UCLASS(Blueprintable, BlueprintType, Category = "Quest System")
class DR_JONES_API AQuestNote : public AActor, public IInteractiveObject
{
	GENERATED_BODY()

public:
	AQuestNote();

	// IInteractiveObject overrides

	virtual void Interact(APawn* Indicator) override;

	UFUNCTION(BlueprintCallable, Category = "Interactive")
	virtual FString GetInteractSentence() override;

	// End of IInteractiveObject overrides

	UFUNCTION(BlueprintImplementableEvent, Category = "Interactive", meta=(DisplayName="On Interact"))
	void K2_OnInteract(APawn* Indicator);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest System", meta = (AllowPrivateAccess = true))
	UStaticMesh* NoteMesh;
};
