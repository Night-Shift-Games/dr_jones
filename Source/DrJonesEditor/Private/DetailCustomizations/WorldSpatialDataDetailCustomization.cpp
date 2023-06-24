#include "WorldSpatialDataDetailCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "World/WorldSpatialData.h"

#define LOCTEXT_NAMESPACE "WorldSpatialDataDetailCustomization"

FWorldSpatialDataDetailCustomization::FWorldSpatialDataDetailCustomization()
{
}

TSharedRef<IDetailCustomization> FWorldSpatialDataDetailCustomization::MakeInstance()
{
	return MakeShareable(new FWorldSpatialDataDetailCustomization);
}

void FWorldSpatialDataDetailCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized(Objects);

	if (Objects.Num() != 1)
	{
		return;
	}

	const TWeakObjectPtr<UWorldSpatialData> WorldSpatialDataPtr = Cast<UWorldSpatialData>(Objects[0]);
	if (!ensure(WorldSpatialDataPtr.IsValid()))
	{
		return;
	}

	const TSharedPtr<FSpatialDataBuffer> SpatialData = WorldSpatialDataPtr->GetBuffer();
	if (!SpatialData)
	{
		return;
	}
	
	IDetailCategoryBuilder& AttributesCategory = DetailBuilder.EditCategory("Attributes", LOCTEXT("AttributesCategory", "Attributes"));

	const TSharedPtr<const FSpatialDataBufferLayout> Layout = SpatialData->GetLayout();
	check(Layout);

	const TArray<FSpatialDataTexelAttributeDescriptor>& Attributes = Layout->GetAttributes();
	for (const FSpatialDataTexelAttributeDescriptor& Attribute : Attributes)
	{
		FDetailWidgetRow& AttributeRow = AttributesCategory.AddCustomRow(FText::FromName(Attribute.Name));
		AttributeRow.ValueContent()
		[
			SNew(STextBlock)
			.Text(FText::FromName(Attribute.Name))
		];
	}
}
