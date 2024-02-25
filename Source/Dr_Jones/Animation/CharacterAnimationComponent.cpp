// Property of Night Shift Games, all rights reserved.

#include "Animation/CharacterAnimationComponent.h"

#include "Items/Item.h"
#include "Player/DrJonesCharacter.h"

UCharacterAnimationComponent::UCharacterAnimationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	bWantsInitializeComponent = true;
}

void UCharacterAnimationComponent::BeginPlay()
{
	Super::BeginPlay();

	const ADrJonesCharacter* OwningPlayer = GetOwner<ADrJonesCharacter>();
	checkf(OwningPlayer, TEXT("UCharacterAnimationComponent is not owned by any player."));
	
	const USkeletalMeshComponent* CharacterMesh = OwningPlayer->GetMesh();
	CharacterMeshAnimInstance = CharacterMesh->GetAnimInstance();
	checkf(CharacterMeshAnimInstance, TEXT("Character Mesh has no Anim Instance set."));

	CharacterMeshAnimInstance->OnMontageEnded.AddDynamic(this, &UCharacterAnimationComponent::OnMontageCompletedEvent);
	CharacterMeshAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UCharacterAnimationComponent::OnMontageNotifyBeginEvent);
}

void UCharacterAnimationComponent::InitializeComponent()
{
	Super::InitializeComponent();

	const AActor* Owner = GetOwner();
	checkf(Owner, TEXT("Animation Component cannot have no owner."));

	CharacterMeshComponent = Owner->FindComponentByClass<USkeletalMeshComponent>();
	if (!CharacterMeshComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Skeletal Mesh Component not found in the owner of Character Animation Component."));
	}
}

void UCharacterAnimationComponent::PlayMontage(UAnimMontage* Montage)
{
	check(CharacterMeshAnimInstance);
	
	const float MontageLength = CharacterMeshAnimInstance->Montage_Play(Montage);
	const bool bPlayedSuccessfully = (MontageLength > 0.f);
}

void UCharacterAnimationComponent::PlayItemMontage(UAnimMontage* Montage) const
{
	if (!ActiveItemAnimation)
	{
		return;
	}

	UAnimInstance* ItemAnimInstance = CharacterMeshComponent->GetLinkedAnimLayerInstanceByClass(ActiveItemAnimation);
	const float MontageLength = ItemAnimInstance->Montage_Play(Montage);
}

UAnimMontage* UCharacterAnimationComponent::FindItemActionMontage(const AItem& Item, const FName& Action)
{
	FCharacterToolAnimations* Animations = CharacterToolAnimations.Find(Item.GetClass());
	if (!Animations)
	{
		return nullptr;
	}

	const TObjectPtr<UAnimMontage>* Montage = Animations->Montages.Find(Action);
	if (!Montage)
	{
		return nullptr;
	}

	return *Montage;
}

void UCharacterAnimationComponent::SetActiveItemAnimation(const TSubclassOf<UAnimInstance> AnimInstanceClass)
{
	checkf(CharacterMeshComponent, TEXT("Cannot change the active item animation because the mesh component is not set."));
	
	if (ActiveItemAnimation != nullptr)
	{
		CharacterMeshComponent->UnlinkAnimClassLayers(ActiveItemAnimation);
	}
	if (AnimInstanceClass != nullptr)
	{
		CharacterMeshComponent->LinkAnimClassLayers(AnimInstanceClass);
	}
	
	ActiveItemAnimation = AnimInstanceClass;
}

void UCharacterAnimationComponent::OnMontageCompletedEvent(UAnimMontage* Montage, bool bInterrupted)
{
	OnMontageCompleted.Broadcast(bInterrupted);
}

void UCharacterAnimationComponent::OnMontageNotifyBeginEvent(FName NotifyName,
	const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	OnMontageNotifyBegin.Broadcast(NotifyName);
}
