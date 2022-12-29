// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RuntimeCharacter.h"
#include "Dr_JonesGameModeBase.generated.h"

class UQuestSystemLogic;

/**
 * 
 */
UCLASS()
class DR_JONES_API ADr_JonesGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	ADr_JonesGameModeBase();

	virtual void BeginPlay() override;

	UQuestSystemLogic* GetQuestSystem() const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest System", meta=(AllowPrivateAccess=true))
	TObjectPtr<UQuestSystemLogic> QuestSystem;
};

FORCEINLINE UQuestSystemLogic* ADr_JonesGameModeBase::GetQuestSystem() const
{
	return QuestSystem;
}
