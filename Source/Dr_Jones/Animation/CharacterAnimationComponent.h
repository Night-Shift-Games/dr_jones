// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAnimationComponent.generated.h"


class ATool;


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


UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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
	const FCharacterToolAnimations& FindAnimationsForTool(FName Tool, bool& bOutFound) const;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	UAnimMontage* FindToolMontage(FName Tool, FName Montage) const;

private:
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> CharacterMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	FCharacterAnimations CharacterAnimations;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = true))
	TMap<FName, FCharacterToolAnimations> CharacterToolAnimations;
};
