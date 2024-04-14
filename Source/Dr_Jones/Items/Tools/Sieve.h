// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tool.h"
#include "Sieve.generated.h"

UCLASS()
class DR_JONES_API ASieve : public ATool
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void Shake();
	
	void SpawnArtifacts();

	UPROPERTY(EditAnywhere, Category = "DrJones|Animation")
	TObjectPtr<UAnimMontage> ShakeMontage;
};
