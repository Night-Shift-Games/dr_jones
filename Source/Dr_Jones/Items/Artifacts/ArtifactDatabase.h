// Property of Night Shift Games, all rights reserved.

#pragma once
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"

#include "ArtifactDatabase.generated.h"

class AArtifact;

USTRUCT(BlueprintType)
struct FArtifactData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMesh> ArtifactMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AArtifact> CustomClass = nullptr;
};

UCLASS(Blueprintable)
class UArtifactDatabase : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ForceInlineRow))
	TMap<FName, FArtifactData> ArtifactDataEntries;
};

USTRUCT(BlueprintType)
struct FArtifactDatabaseRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FArtifactData ArtifactData;
};
