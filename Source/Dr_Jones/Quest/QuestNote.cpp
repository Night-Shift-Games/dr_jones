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

void AQuestNote::Initialize(UQuestData& QuestDataObject)
{
	QuestData = &QuestDataObject;
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
	return TEXT("Pickup Note");
}
