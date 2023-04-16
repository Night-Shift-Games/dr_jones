// Property of Night Shift Games, all rights reserved.


#include "Animation/CharacterAnimationComponent.h"

// Sets default values for this component's properties
UCharacterAnimationComponent::UCharacterAnimationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bWantsInitializeComponent = true;
}


// Called when the game starts
void UCharacterAnimationComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
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


// Called every frame
void UCharacterAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

const FCharacterToolAnimations& UCharacterAnimationComponent::FindAnimationsForTool(FName Tool, bool& bOutFound) const
{
	if (const FCharacterToolAnimations* Animations = CharacterToolAnimations.Find(Tool))
	{
		bOutFound = true;
		return *Animations;
	}

	bOutFound = false;
	static FCharacterToolAnimations NullAnimations {};
	return NullAnimations;
}

UAnimMontage* UCharacterAnimationComponent::FindToolMontage(FName Tool, FName Montage) const
{
	if (const FCharacterToolAnimations* Animations = CharacterToolAnimations.Find(Tool))
	{
		return Animations->Montages.FindRef(Montage);
	}
	return nullptr;
}

