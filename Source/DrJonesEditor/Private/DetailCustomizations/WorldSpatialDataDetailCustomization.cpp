#include "WorldSpatialDataDetailCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SSpinBox.h"
#include "World/WorldSpatialData.h"

#define LOCTEXT_NAMESPACE "WorldSpatialDataDetailCustomization"

FWorldSpatialDataDetailCustomization::FWorldSpatialDataDetailCustomization()
	: VisualizationGeoLocation(FGeoLocation { 45.0f, 20.0f })
	, VisualizationYear(1920.0f)
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

	WorldSpatialDataPtr = Cast<UWorldSpatialData>(Objects[0]);
	if (!ensure(WorldSpatialDataPtr.IsValid()))
	{
		return;
	}

	const TSharedPtr<FSpatialDataBuffer> SpatialData = WorldSpatialDataPtr->GetBuffer();
	if (!SpatialData)
	{
		return;
	}
	
	IDetailCategoryBuilder& RawAttributesCategory = DetailBuilder.EditCategory("RawAttributes", LOCTEXT("RawAttributesCategory", "Raw Attributes"));
	RawAttributesCategory.InitiallyCollapsed(true);
	RawAttributesCategory.RestoreExpansionState(false);
	
	const TSharedPtr<const FSpatialDataBufferLayout> Layout = SpatialData->GetLayout();
	check(Layout);

	const TArray<FSpatialDataTexelAttributeDescriptor>& Attributes = Layout->GetAttributes();
	for (const FSpatialDataTexelAttributeDescriptor& Attribute : Attributes)
	{
		FDetailWidgetRow& AttributeRow = RawAttributesCategory.AddCustomRow(FText::FromName(Attribute.Name));
		AttributeRow
		.NameContent()
		[
			SNew(STextBlock)
			.Font(DetailBuilder.GetDetailFont())
			.Text(FText::FromName(Attribute.Name))
		]
		.ValueContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(FMargin(2.0f, 4.0f))
				[
					SNew(STextBlock)
					.Text(UEnum::GetDisplayValueAsText(Attribute.Type))
				]
				+ SVerticalBox::Slot()
				.Padding(FMargin(2.0f, 4.0f))
				[
					SNew(STextBlock)
					.Font(DetailBuilder.GetDetailFont())
					.Text(FText::FromString(FString::Printf(TEXT("Size: %i"), Attribute.Size)))
				]
				+ SVerticalBox::Slot()
				.Padding(FMargin(2.0f, 4.0f))
				[
					SNew(STextBlock)
					.Font(DetailBuilder.GetDetailFont())
					.Text(FText::FromString(FString::Printf(TEXT("Offset: %i"), Attribute.Stride)))
				]
			]
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text_Lambda([this, Name = Attribute.Name]
				{
					const uint8 SampledValue = WorldSpatialDataPtr->SampleAtLocation<uint8>(Name, VisualizationGeoLocation);
					return FText::Format(LOCTEXT("AttributeValue", "Value: {0}"), FText::AsNumber(SampledValue));
				})
			]
		];
	}

	IDetailCategoryBuilder& VisualizationCategory = DetailBuilder.EditCategory("Visualization", LOCTEXT("VisualizationCategory", "Visualization"));

	VisualizationCategory.AddCustomRow(LOCTEXT("GeoLocationRow", "Geo Location"))
	.NameContent()
	[
		SNew(STextBlock)
		.Font(DetailBuilder.GetDetailFont())
		.Text(LOCTEXT("GeoLocationRowName", "Geo Location"))
	]
	.ValueContent()
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.MinDesiredWidth(100.0f)
				.Text(LOCTEXT("GeoLocationLatitude", "Latitude"))
			]
			+ SHorizontalBox::Slot()
			[
				SNew(SSpinBox<float>)
				.MinDesiredWidth(200.0f)
				.Value_Lambda([this] { return VisualizationGeoLocation.Latitude; })
				.MinValue(-90.0f)
				.MaxValue(90.0f)
				.OnValueChanged_Lambda([this](float Value) { VisualizationGeoLocation.Latitude = Value; })
			]
		]
		+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.MinDesiredWidth(100.0f)
				.Text(LOCTEXT("GeoLocationLongitude", "Longitude"))
			]
			+ SHorizontalBox::Slot()
			[
				SNew(SSpinBox<float>)
				.MinDesiredWidth(200.0f)
				.Value_Lambda([this] { return VisualizationGeoLocation.Longitude; })
				.MinValue(-180.0f)
				.MaxValue(180.0f)
				.OnValueChanged_Lambda([this](float Value) { VisualizationGeoLocation.Longitude = Value; })
			]
		]
	];
	
	VisualizationCategory.AddCustomRow(LOCTEXT("Year", "Year"))
	.NameContent()
	[
		SNew(STextBlock)
		.Font(DetailBuilder.GetDetailFont())
		.Text(LOCTEXT("YearRowName", "Year"))
	]
	.ValueContent()
	[
		SNew(SSpinBox<float>)
		.MinDesiredWidth(300.0f)
		.Value_Lambda([this] { return VisualizationYear; })
		.MinValue(-20000.0f)
		.MaxValue(2000.0f)
		.OnValueChanged_Lambda([this](float Value) { VisualizationYear = Value; })
	];

	TSharedPtr<SVerticalBox> AttributesVerticalBox;
	
	VisualizationCategory.AddCustomRow(LOCTEXT("SampledAttributesRow", "Sampled Attributes"))
	.NameContent()
	[
		SNew(STextBlock)
		.Font(DetailBuilder.GetDetailFont())
		.Text(LOCTEXT("SampledAttributesRowName", "Sampled Attributes"))
	]
	.ValueContent()
	[
		SAssignNew(AttributesVerticalBox, SVerticalBox)
	];

	TSet<FName> AvailableAttributes = WorldSpatialDataPtr->FindAvailableAttributeNamesExcludingYear();
	for (const FName& AttributeName : AvailableAttributes)
	{
		AttributesVerticalBox.ToSharedRef()
		->AddSlot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(2.0f, 4.0f)
			[
				SNew(STextBlock)
				.MinDesiredWidth(200.0f)
				.Text(FText::FromName(AttributeName))
			]
			+ SHorizontalBox::Slot()
			.Padding(2.0f, 4.0f)
			[
				SNew(STextBlock)
				.Text_Lambda([this, AttributeName]
				{
					const float Value = WorldSpatialDataPtr->SampleByteNormalized(AttributeName, VisualizationGeoLocation, VisualizationYear);
					return FText::AsNumber(Value);
				})
			]
		];
	}
}
