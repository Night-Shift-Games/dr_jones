// Property of Night Shift Games, all rights reserved.

#pragma once
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"

#include "ArtifactDatabase.generated.h"

class AArtifact;

UENUM(BlueprintType)
enum class EArtifactRarity : uint8
{
	Common = 0,
	Uncommon,
	Rare,
	Legendary,
	Story,
};

UENUM(BlueprintType)
enum class EArtifactWear : uint8
{
	FactoryNew = 0,
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arifact Data")
	TObjectPtr<UStaticMesh> ArtifactMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arifact Data")
	TSubclassOf<AArtifact> CustomClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arifact Data")
	EArtifactRarity Rarity = EArtifactRarity::Common;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arifact Data")
	FName Usage = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arifact Data")
	EArtifactWear Wear = EArtifactWear::FactoryNew;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arifact Data", meta = (ClampMin = 0, UIMin = 0))
	int32 AgeMax = 3000;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arifact Data", meta = (ClampMin = 0, UIMin = 0))
	int32 AgeMin = 2000;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arifact Data")
	FName Culture = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arifact Data")
	EArtifactSize Size = EArtifactSize::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arifact Data")
	FText Name = FText::AsCultureInvariant(TEXT("None"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arifact Data")
	FText Description = FText::AsCultureInvariant(TEXT("None"));
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arifact Data")
	FArtifactData ArtifactData;
};
