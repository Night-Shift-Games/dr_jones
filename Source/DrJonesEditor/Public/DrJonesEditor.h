#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class IAssetTypeActions;

DECLARE_LOG_CATEGORY_EXTERN(LogDrJonesEditor, All, All);

class FDrJonesEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterAssetTypeActions();
	void UnregisterAssetTypeActions();

private:
	TArray<TSharedRef<IAssetTypeActions>> AssetTypeActionsArray;
};
