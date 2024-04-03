#include "ArtifactFactory.h"

#include "Artifact.h"
#include "ArtifactDatabase.h"
#include "Managment/Dr_JonesGameModeBase.h"

AArtifact* UArtifactFactory::ConstructArtifactFromDatabase(const UObject& WorldContextObject, const FName& ArtifactID)
{
	const FArtifactData* ArtifactData = PullArtifactDataFromDatabase(ArtifactID);
	return ConstructArtifact(WorldContextObject, *ArtifactData);
}

FArtifactData* UArtifactFactory::PullArtifactDataFromDatabase(const FName& ArtifactID)
{
	// Get a database.
	UArtifactDatabase* ArtifactDatabase = GWorld && GWorld->IsGameWorld() ? GWorld->GetAuthGameMode<ADr_JonesGameModeBase>()->GetArtifactDataBase() : nullptr;
	if (!ArtifactDatabase)
	{
		return nullptr;
	}
	// Find object in database.
	FArtifactData* ArtifactData = ArtifactDatabase->ArtifactDataEntries.Find(ArtifactID);
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
	SpawnParameters.Name = MakeUniqueObjectName(WorldContextObject.GetWorld(), ArtifactData.CustomClass ? ArtifactData.CustomClass : AArtifact::StaticClass(), ArtifactData.ArtifactID, EUniqueObjectNameOptions::GloballyUnique);
	
	if (ArtifactData.CustomClass)
	{
		return ConstructArtifact(WorldContextObject, ArtifactData.CustomClass);
	}
	
	AArtifact* NewArtifact = WorldContextObject.GetWorld()->SpawnActor<AArtifact>(SpawnParameters);
	NewArtifact->SetActorLabel(SpawnParameters.Name.ToString());

	if (NewArtifact)
	{
		NewArtifact->SetupArtifact(ArtifactData);
	}
	
	return NewArtifact;
}