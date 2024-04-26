#include "ArtifactFactory.h"

#include "Artifact.h"
#include "ArtifactDatabase.h"
#include "Managment/Dr_JonesGameModeBase.h"
#include "Utilities/NightShiftSettings.h"

AArtifact* UArtifactFactory::ConstructArtifactFromDatabase(const UObject& WorldContextObject, const FName& ArtifactID)
{
	const FArtifactData* ArtifactData = PullArtifactDataFromDatabase(ArtifactID);
	return ConstructArtifact(WorldContextObject, *ArtifactData);
}

FArtifactData* UArtifactFactory::PullArtifactDataFromDatabase(const FName& ArtifactID)
{
	// Get a database.
	const UDataTable* ArtifactDataTable = GWorld && GWorld->IsGameWorld() ? GWorld->GetAuthGameMode<ADr_JonesGameModeBase>()->GetArtifactDataBase() : GetDefault<UNightShiftSettings>()->ArtifactDataTable.LoadSynchronous();
	if (!ArtifactDataTable)
	{
		return nullptr;
	}
	// Find object in database.
	FArtifactData* ArtifactData = ArtifactDataTable->FindRow<FArtifactData>(ArtifactID, TEXT("Context"));
	if (!ArtifactData)
	{
		UE_LOG(LogTemp, Error, TEXT("Artifact named %s not found in the Database!"), *ArtifactID.ToString());
		return nullptr;
	}
	return ArtifactData;
}

AArtifact* UArtifactFactory::ConstructArtifact(const UObject& WorldContextObject, TSubclassOf<AArtifact> ArtifactClass)
{
	// Get world
	UWorld* World = WorldContextObject.GetWorld();
	check(World);
	
	return World->SpawnActor<AArtifact>(ArtifactClass);
}

AArtifact* UArtifactFactory::ConstructArtifact(const UObject& WorldContextObject, const FArtifactData& ArtifactData)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Name = MakeUniqueObjectName(WorldContextObject.GetWorld(), ArtifactData.CustomClass ? ArtifactData.CustomClass : TSubclassOf<AArtifact>(AArtifact::StaticClass()), ArtifactData.ArtifactID, EUniqueObjectNameOptions::GloballyUnique);
	
	if (ArtifactData.CustomClass)
	{
		return ConstructArtifact(WorldContextObject, ArtifactData.CustomClass);
	}
	
	AArtifact* NewArtifact = WorldContextObject.GetWorld()->SpawnActor<AArtifact>(SpawnParameters);
#if WITH_EDITOR
	NewArtifact->SetActorLabel(SpawnParameters.Name.ToString());
#endif
	
	if (NewArtifact)
	{
		NewArtifact->SetupArtifact(ArtifactData);
	}
	
	return NewArtifact;
}