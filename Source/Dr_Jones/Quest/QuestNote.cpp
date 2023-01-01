// Property of Night Shift Games, all rights reserved.


#include "QuestNote.h"
#include "Quest.h"

FName AQuestNote::NoteMeshComponentName = TEXT("NoteMesh");

AQuestNote::AQuestNote()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	NoteMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(AQuestNote::NoteMeshComponentName);
	RootComponent = NoteMeshComponent;
}

void AQuestNote::Initialize(const FDataTableRowHandle& QuestRow)
{
	QuestTableRow = QuestRow;
}

void AQuestNote::OnConstruction(const FTransform& Transform)
{
	check(NoteMeshComponent);

	UStaticMesh* Mesh = QuestData ? QuestData->QuestNoteData.NoteMesh : nullptr;
	NoteMeshComponent->SetStaticMesh(Mesh);
}

void AQuestNote::Interact(APawn* Indicator)
{
	K2_OnInteract(Indicator);
}

FString AQuestNote::GetInteractSentence()
{
	const FQuestTableRow* Row = QuestTableRow.GetRow<FQuestTableRow>("QuestRow");
	check(Row);

	// @TODO: TEMPORARY:
	auto Builder = TStringBuilder<2000>();
	Builder.Append(TEXT("Pickup "));
	Builder.Append(Row->QuestContentData.Title.ToString());
	Builder.Append(TEXT(" ("));
	Builder.Append(QuestTableRow.RowName.ToString());
	Builder.Append(TEXT(")"));
	return Builder.ToString();
}
