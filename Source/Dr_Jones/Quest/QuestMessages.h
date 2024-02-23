#pragma once

#include "CoreMinimal.h"
#include "Quest/QuestSystem.h"

#include "QuestMessages.generated.h"

class AArtifact;

UCLASS(Blueprintable)
class UArtifactCollectedQuestMessage : public UObject, public IQuestMessageInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<AArtifact> Artifact;
};

UCLASS(Blueprintable)
class UArtifactCleanedQuestMessage : public UObject, public IQuestMessageInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<AArtifact> Artifact;
};
