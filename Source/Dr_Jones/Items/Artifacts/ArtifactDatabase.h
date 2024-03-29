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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Arifact Data")
	FName ArtifactID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arifact Data")
	FText Name = FText::AsCultureInvariant(TEXT("None"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arifact Data", meta = (MultiLine = true))
	FText Description = FText::AsCultureInvariant(TEXT("None"));
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arifact Data")
	TArray<TSoftObjectPtr<UStaticMesh>> ArtifactMesh;

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
};

UCLASS(Blueprintable)
class UArtifactDatabase : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ForceInlineRow, TitleProperty = "Name"))
	TMap<FName, FArtifactData> ArtifactDataEntries;

#if WITH_EDITORONLY_DATA
	virtual void Serialize(FStructuredArchive::FRecord Record) override;
#endif
};
#if WITH_EDITORONLY_DATA
inline void UArtifactDatabase::Serialize(FStructuredArchive::FRecord Record)
{
	for (auto & KV : ArtifactDataEntries)
	{
		KV.Value.ArtifactID = KV.Key;
	}
	Super::Serialize(Record);
}
#endif

USTRUCT(BlueprintType)
struct FArtifactDatabaseRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arifact Data")
	FArtifactData ArtifactData;
};
