// Property of Night Shift Games, all rights reserved.

#include "AssetTypeActions_WorldSpatialData.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

void FAssetTypeActions_WorldSpatialData::GetActions(const TArray<UObject*>& InObjects, FToolMenuSection& Section)
{
	TArray<TWeakObjectPtr<UWorldSpatialData>> SpatialDataObjects = GetTypedWeakObjectPtrs<UWorldSpatialData>(InObjects);
	const int32 ObjectsNum = SpatialDataObjects.Num();
	
	Section.AddMenuEntry(TEXT("ImportSpatialData"),
		LOCTEXT("WorldSpatialData_ImportSpatialData", "Import Spatial Data"),
		LOCTEXT("WorldSpatialData_ImportSpatialDataTooltip", "Import Spatial Data from texture"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.ImportIcon"),
		FUIAction(
			FExecuteAction::CreateSP(this, &FAssetTypeActions_WorldSpatialData::ExecuteImportSpatialData, SpatialDataObjects),
			FCanExecuteAction::CreateLambda([ObjectsNum] { return ObjectsNum == 1; })
		));
}

void FAssetTypeActions_WorldSpatialData::ExecuteImportSpatialData(TArray<TWeakObjectPtr<UWorldSpatialData>> InSpatialDataObjects)
{
	if (InSpatialDataObjects.Num() != 1 || !InSpatialDataObjects[0].IsValid())
	{
		return;
	}
	
	const TStrongObjectPtr<UWorldSpatialData> WorldSpatialData = TStrongObjectPtr(InSpatialDataObjects[0].Get());

	FSpatialDataBufferBuilder Builder;
	Builder.AddAttribute<uint8>("Attribute0");
	const TSharedRef<FSpatialDataBuffer> Buffer = Builder.Build(FIntVector4(1024, 512, 16, 1));

	for (int32 Index = 0; Index < 1024 * 512 * 16; ++Index)
	{
		uint8 Value = 127;
		Buffer->WriteAttributeRaw(Index, "Attribute0", &Value, sizeof(Value));
	}

	WorldSpatialData->SetBuffer(Buffer);
}
