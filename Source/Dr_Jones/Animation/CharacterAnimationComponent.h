// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Item.h"
#include "CharacterAnimationComponent.generated.h"


USTRUCT(BlueprintType, Category = "Animation")
struct FCharacterAnimations
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimSequence> Idle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UBlendSpace> Movement;
};


USTRUCT(BlueprintType, Category = "Animation")
struct FCharacterToolAnimations
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimSequence> Idle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TMap<FName, TObjectPtr<UAnimMontage>> Montages;
};

USTRUCT(BlueprintType, Category = "Animation|Item")
struct FItemIKData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Animation|Item")
	FVector IKTrackLocation = FVector::ZeroVector;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMontageNotifyBeginDelegate, FName, NotifyName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMontageCompletedDelegate, bool, bInterrupted);

UCLASS( Blueprintable, BlueprintType, ClassGroup=(Animation), meta=(BlueprintSpawnableComponent) )
class DR_JONES_API UCharacterAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCharacterAnimationComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;

public:
	void PlayMontage(UAnimMontage* Montage);

	UAnimMontage* FindItemActionMontage(const AItem& Item, const FName& Action);

	UFUNCTION()
	void OnMontageCompletedEvent(UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION()
	void OnMontageNotifyBeginEvent(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

public:
	UPROPERTY(BlueprintAssignable, Category = "Animation")
	FOnMontageNotifyBeginDelegate OnMontageNotifyBegin;

	UPROPERTY(BlueprintAssignable, Category = "Animation")
	FOnMontageCompletedDelegate OnMontageCompleted;

	UPROPERTY(BlueprintReadOnly, Category = "Animation|Item")
	FItemIKData ItemIKData;

private:
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> CharacterMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	FCharacterAnimations CharacterAnimations;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	TMap<TSubclassOf<AItem>, FCharacterToolAnimations> CharacterToolAnimations;

	UPROPERTY(Transient)
	TObjectPtr<UAnimInstance> CharacterMeshAnimInstance;
};
