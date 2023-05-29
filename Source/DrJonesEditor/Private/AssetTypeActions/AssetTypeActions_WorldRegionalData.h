#pragma once

#include "AssetTypeActions_Base.h"
#include "World/WorldData.h"

class FAssetTypeActions_WorldRegionalData : public FAssetTypeActions_Base
{
public:
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_WorldRegionalData", "World Regional Data"); }
	virtual FColor GetTypeColor() const override { return FColor(230, 90, 40); }
	virtual UClass* GetSupportedClass() const override { return UWorldData::StaticClass(); }
	virtual uint32 GetCategories() override { return EAssetTypeCategories::Gameplay; }
	// virtual bool HasActions ( const TArray<UObject*>& InObjects ) const override { return true; }
	// virtual void GetActions(const TArray<UObject*>& InObjects, struct FToolMenuSection& Section) override;
	// virtual void OpenAssetEditor( const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>() ) override;
};
