// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Tools/Tool.h"
#include "ItemMontageDispatcher.h"
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
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	const FCharacterToolAnimations& FindAnimationsForTool(const TSubclassOf<ATool>& Tool, bool& bOutFound) const;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	UAnimMontage* FindToolMontage(const TSubclassOf<ATool>& Tool, FName Montage) const;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void DispatchToolAction(TSubclassOf<ATool> Tool, FName Action);

private:
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> CharacterMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	FCharacterAnimations CharacterAnimations;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	TMap<TSubclassOf<ATool>, FCharacterToolAnimations> CharacterToolAnimations;

	UPROPERTY(EditAnywhere, Category = "Animation", meta = (AllowPrivateAccess = true))
	TMap<TSubclassOf<ATool>, TSubclassOf<UItemMontageDispatcher>> ItemMontageDispatchers;

	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	TMap<TSubclassOf<ATool>, TObjectPtr<UItemMontageDispatcher>> ItemMontageDispatcherInstances;
};
