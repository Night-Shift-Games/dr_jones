// Property of Night Shift Games, all rights reserved.

#include "DrJonesAnimNotify.h"

void UDrJonesAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	OnNotifyHit.Broadcast();
}
