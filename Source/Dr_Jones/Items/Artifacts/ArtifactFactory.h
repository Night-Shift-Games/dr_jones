// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ArtifactFactory.generated.h"

class AArtifact;
struct FArtifactData;

UCLASS()
class UArtifactFactory : public UObject
{
	GENERATED_BODY()
	
public:
	static AArtifact* ConstructArtifactFromDatabase(const UObject& WorldContextObject, const FName& ArtifactID);
	static FArtifactData* PullArtifactDataFromDatabase(const FName& ArtifactID);
	static AArtifact* ConstructArtifact(const UObject& WorldContextObject, TSubclassOf<AArtifact> ArtifactClass);
	static AArtifact* ConstructArtifact(const UObject& WorldContextObject, const FArtifactData& ArtifactData);
};
