// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "DrJonesAnimNotify.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDrJonesAnimNotifyHit)

UCLASS()
class DR_JONES_API UDrJonesAnimNotify : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
	FOnDrJonesAnimNotifyHit OnNotifyHit;
};
