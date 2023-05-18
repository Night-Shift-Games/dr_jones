// Property of Night Shift Games, all rights reserved.


#include "Animation/CharacterAnimationComponent.h"

#include "Player/DrJonesCharacter.h"

// Sets default values for this component's properties
UCharacterAnimationComponent::UCharacterAnimationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	bWantsInitializeComponent = true;
}


// Called when the game starts
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

void UCharacterAnimationComponent::OnMontageCompletedEvent(UAnimMontage* Montage, bool bInterrupted)
{
	OnMontageCompleted.Broadcast(bInterrupted);
}

void UCharacterAnimationComponent::OnMontageNotifyBeginEvent(FName NotifyName,
	const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	OnMontageNotifyBegin.Broadcast(NotifyName);
}

