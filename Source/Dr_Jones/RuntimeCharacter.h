// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RuntimeCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

UCLASS()
class DR_JONES_API ARuntimeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARuntimeCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnUseItem OnActionKeyPressed;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void Turn(float AxisValue);
	void LookUp(float AxisValue);
	void PrimaryAction();
	
};
