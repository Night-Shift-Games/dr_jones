// Property of Night Shift Games, all rights reserved.

#include "QuestTrigger.h"

#include "Components/BoxComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/TextRenderComponent.h"
#include "Player/DrJonesCharacter.h"
#include "World/Illuminati.h"

AQuestTrigger::AQuestTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

#if WITH_EDITORONLY_DATA
	LabelComponent = CreateEditorOnlyDefaultSubobject<UTextRenderComponent>(TEXT("LabelComponent"));
	if (LabelComponent)
	{
		LabelComponent->SetHorizontalAlignment(EHTA_Center);
		LabelComponent->SetWorldSize(32.0f);
		LabelComponent->SetupAttachment(GetCollisionComponent());
	}
#endif
}

// Called when the game starts or when spawned
void AQuestTrigger::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &AQuestTrigger::OnBeginOverlap);
}

void AQuestTrigger::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	ADrJonesCharacter* Character = Cast<ADrJonesCharacter>(OtherActor);
	if (!Character)
	{
		return;
	}

	UQuestSystemComponent* QuestSystem = AIlluminati::GetQuestSystemInstance(this);
	if (!QuestSystem)
	{
		return;
	}

	UQuestTriggerMessage* Message = NewObject<UQuestTriggerMessage>();
	Message->TriggerIdentifier = TriggerIdentifier;
	QuestSystem->SendQuestMessage(Message);
}

#if WITH_EDITOR
void AQuestTrigger::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const UBoxComponent* Box = Cast<UBoxComponent>(GetCollisionComponent());
	check(Box);

	const FVector Extent = Box->GetScaledBoxExtent();

	if (LabelComponent)
	{
		LabelComponent->SetRelativeLocation(FVector(0.0f, 0.0f, Extent.Z * 1.0f / 3.0f));

		LabelComponent->SetText(FText::FromName(TriggerIdentifier));
		LabelComponent->SetTextRenderColor(Box->ShapeColor);
	}
}
#endif
