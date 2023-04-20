// Property of Night Shift Games, all rights reserved.


#include "Animation/CharacterAnimationComponent.h"

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

	// Initialize Tool Montage Dispatchers:

	ItemMontageDispatcherInstances.Reserve(ItemMontageDispatchers.Num());
	for (auto& [ToolClass, DispatcherClass] : ItemMontageDispatchers)
	{
		if (ToolClass && DispatcherClass)
		{
			UItemMontageDispatcher* Dispatcher = NewObject<UItemMontageDispatcher>(this, DispatcherClass);
			check(Dispatcher);

			ItemMontageDispatcherInstances.Emplace(ToolClass, Dispatcher);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid Tool or Dispatcher class specified in the Character Animation Component."));
		}
	}
}


// Called every frame
void UCharacterAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

const FCharacterToolAnimations& UCharacterAnimationComponent::FindAnimationsForTool(const TSubclassOf<ATool>& Tool, bool& bOutFound) const
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

UAnimMontage* UCharacterAnimationComponent::FindToolMontage(const TSubclassOf<ATool>& Tool, FName Montage) const
{
	if (const FCharacterToolAnimations* Animations = CharacterToolAnimations.Find(Tool))
	{
		return Animations->Montages.FindRef(Montage);
	}
	return nullptr;
}

void UCharacterAnimationComponent::DispatchToolAction(TSubclassOf<ATool> Tool, FName Action)
{
	if (UItemMontageDispatcher* Dispatcher = ItemMontageDispatcherInstances.FindRef(Tool))
	{
		Dispatcher->Dispatch(Action);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Montage Dispatcher not found for tool %s"), *Tool->GetName());
	}
}

