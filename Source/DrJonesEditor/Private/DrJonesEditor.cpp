#include "DrJonesEditor.h"

#include "AssetTypeActions/AssetTypeActions_WorldRegionalData.h"
#include "AssetTypeActions/AssetTypeActions_WorldSpatialData.h"

DEFINE_LOG_CATEGORY(LogDrJonesEditor);

#define LOCTEXT_NAMESPACE "FDrJonesEditorModule"

void FDrJonesEditorModule::StartupModule()
{
	UE_LOG(LogDrJonesEditor, Log, TEXT("DrJonesEditor Module startup"));

	AssetTypeActionsArray.Add(MakeShared<FAssetTypeActions_WorldRegionalData>());
	AssetTypeActionsArray.Add(MakeShared<FAssetTypeActions_WorldSpatialData>());
	RegisterAssetTypeActions();
}

void FDrJonesEditorModule::ShutdownModule()
{
	UE_LOG(LogDrJonesEditor, Log, TEXT("DrJonesEditor Module shutdown"));

	UnregisterAssetTypeActions();
}

void FDrJonesEditorModule::RegisterAssetTypeActions()
{
	for (const TSharedRef<IAssetTypeActions>& AssetTypeActions : AssetTypeActionsArray)
	{
		UE_LOG(LogDrJonesEditor, Log, TEXT("Registering Asset Type Actions for %s."), *AssetTypeActions->GetName().ToString());
		FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(AssetTypeActions);
	}
}

void FDrJonesEditorModule::UnregisterAssetTypeActions()
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return;
	}
	
	for (const TSharedRef<IAssetTypeActions>& AssetTypeActions : AssetTypeActionsArray)
	{
		FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(AssetTypeActions);
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDrJonesEditorModule, DrJonesEditor)
