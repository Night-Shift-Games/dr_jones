#include "DrJonesEditor.h"

#include "AssetTypeActions/AssetTypeActions_WorldRegionalData.h"
#include "AssetTypeActions/AssetTypeActions_WorldSpatialData.h"
#include "DetailCustomizations/WorldSpatialDataDetailCustomization.h"

DEFINE_LOG_CATEGORY(LogDrJonesEditor);

#define LOCTEXT_NAMESPACE "FDrJonesEditorModule"

void FDrJonesEditorModule::StartupModule()
{
	UE_LOG(LogDrJonesEditor, Log, TEXT("DrJonesEditor Module startup"));

	AssetTypeActionsArray.Add(MakeShared<FAssetTypeActions_WorldRegionalData>());
	AssetTypeActionsArray.Add(MakeShared<FAssetTypeActions_WorldSpatialData>());
	RegisterAssetTypeActions();

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>( "PropertyEditor" );

	PropertyEditorModule.RegisterCustomClassLayout(UWorldSpatialData::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(FWorldSpatialDataDetailCustomization::MakeInstance));
}

void FDrJonesEditorModule::ShutdownModule()
{
	UE_LOG(LogDrJonesEditor, Log, TEXT("DrJonesEditor Module shutdown"));

	UnregisterAssetTypeActions();

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>( "PropertyEditor" );
	
	PropertyEditorModule.UnregisterCustomClassLayout(UWorldSpatialData::StaticClass()->GetFName());
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
