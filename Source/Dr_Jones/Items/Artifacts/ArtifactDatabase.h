// Property of Night Shift Games, all rights reserved.

#pragma once
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"

#include "ArtifactDatabase.generated.h"

class AArtifact;

UENUM(BlueprintType)
enum class EArtifactRarity : uint8
{
	Common,
	Uncommon,
	Rare,
	Legendary,
	Story,
};

UENUM(BlueprintType)
enum class EArtifactWear : uint8
{
	FactoryNew,
	MinimalWear,
	FieldTested,
	WellWorn,
	BattleScarred,
};

UENUM(BlueprintType)
enum class EArtifactSize : uint8
{
	Small,
	Normal,
	Large,
};

USTRUCT(BlueprintType)
struct FArtifactData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMesh> ArtifactMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AArtifact> CustomClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EArtifactRarity Rarity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Usage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EArtifactWear Wear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, UIMin = 0))
	int32 Age;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Culture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EArtifactSize Size;
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
