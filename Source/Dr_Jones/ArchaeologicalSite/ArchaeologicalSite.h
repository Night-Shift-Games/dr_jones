// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ArchaeologicalSite.generated.h"

struct FChunk;

UCLASS()
class DR_JONES_API AArchaeologicalSite : public AActor
{
	GENERATED_BODY()
	
public:
	AArchaeologicalSite();
	virtual void Tick(float DeltaSeconds) override;
};