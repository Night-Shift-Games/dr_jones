#pragma once

#include "Tool.h"

#include "Pickaxe.generated.h"

UCLASS()
class DR_JONES_API APickaxe : public ATool
{
	GENERATED_BODY()
public:
	
	UFUNCTION()
	void Dig();
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
