// Property of Night Shift Games, all rights reserved.

#pragma once

#include "Animation/ArtifactAnimationDataAsset.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "Utilities/NightShiftSettings.h"

#include "ArtifactDatabase.generated.h"

class AArtifact;

UENUM(BlueprintType)
enum class EArtifactRarity : uint8
{
	Common = 0,
	Uncommon,
	Rare,
	VeryRare,
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
struct FArtifactData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Arifact Data")
	FName ArtifactID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arifact Data")
	FText Name = FText::AsCultureInvariant(TEXT("None"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arifact Data", meta = (MultiLine = true))
	FText Description = FText::AsCultureInvariant(TEXT("None"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arifact Data")
	TSoftObjectPtr<UTexture2D> ArtifactIcon;
	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arifact Data")
	TSoftObjectPtr<UArtifactAnimationDataAsset> ArtifactAnimationDataAsset;

#if WITH_EDITORONLY_DATA
	virtual void OnPostDataImport(const UDataTable* InDataTable, const FName InRowName, TArray<FString>& OutCollectedImportProblems) override;
#endif
};

#if WITH_EDITORONLY_DATA
inline void FArtifactData::OnPostDataImport(const UDataTable* InDataTable, const FName InRowName,
	TArray<FString>& OutCollectedImportProblems)
{
	FTableRowBase::OnPostDataImport(InDataTable, InRowName, OutCollectedImportProblems);

	// Get data from database
	FArtifactData* Data = InDataTable->FindRow<FArtifactData>(InRowName, TEXT("Context"));
	
	// Set Artifact ID
	Data->ArtifactID = InRowName;
	
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	TArray<FAssetData> AssetData;
	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.ClassPaths = {
		UStaticMesh::StaticClass()->GetClassPathName(),
		UTexture2D::StaticClass()->GetClassPathName(),
		UArtifactAnimationDataAsset::StaticClass()->GetClassPathName()
	};
	Filter.PackagePaths.Add(TEXT("/Game/DrJones/Regions"));
	Filter.PackagePaths.Add(TEXT("/Game/Dev"));
	AssetRegistryModule.Get().GetAssets(Filter, AssetData);

	// Find and set Static Mesh
	const FName ArtifactStaticMeshPrefix = TEXT("SM_A_");
	const FName ArtifactStaticMeshAssetName = FName(ArtifactStaticMeshPrefix.ToString() + Data->ArtifactID.ToString());

	// Find Icon
	const FName ArtifactIconPrefix = TEXT("I_A_");
	const FName ArtifactIconAssetName = FName(ArtifactIconPrefix.ToString() + Data->ArtifactID.ToString());
	
	const FName ArtifactAnimationDataAssetPrefix = TEXT("DA_A_");
	const FName ArtifactAnimationAssetName = FName(ArtifactAnimationDataAssetPrefix.ToString() + Data->ArtifactID.ToString());

	Data->ArtifactMesh.Empty();
	Data->ArtifactIcon.Reset();
	Data->ArtifactAnimationDataAsset = nullptr;
	
	for (auto& Object : AssetData)
	{
		FString AssetName = Object.AssetName.ToString();
		FString AssetNameWithoutVariantSuffix = AssetName.LeftChop(2);
		if (AssetNameWithoutVariantSuffix.Equals(ArtifactStaticMeshAssetName.ToString()))
		{
			TSoftObjectPtr<UStaticMesh> StaticMesh = TSoftObjectPtr<UStaticMesh>(Object.GetSoftObjectPath());
			Data->ArtifactMesh.AddUnique(StaticMesh);
		}
		if (AssetName.Equals(ArtifactIconAssetName.ToString()))
		{
			const TSoftObjectPtr<UTexture2D> Icon = TSoftObjectPtr<UTexture2D>(Object.GetSoftObjectPath());
			Data->ArtifactIcon = Icon;
		}
		if (AssetName.Equals(ArtifactAnimationAssetName.ToString()))
		{
			const TSoftObjectPtr<UArtifactAnimationDataAsset> SoftArtifactAnimationDataAsset = TSoftObjectPtr<UArtifactAnimationDataAsset>(Object.GetSoftObjectPath());
			Data->ArtifactAnimationDataAsset = SoftArtifactAnimationDataAsset;
		}
	}

	// Check and report errors
	if (Data->ArtifactMesh.IsEmpty())
	{
		OutCollectedImportProblems.Add(Data->ArtifactID.ToString() + TEXT(" do not have any properly named mesh!"));
	}
	if (Data->AgeMax < Data->AgeMin)
	{
		OutCollectedImportProblems.Add(Data->ArtifactID.ToString() + TEXT(" have invalid years data."));
	}
	if (!Data->ArtifactIcon)
	{
		OutCollectedImportProblems.Add(Data->ArtifactID.ToString() + TEXT(" do not have any properly named icon!"));
	}
}
#endif

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