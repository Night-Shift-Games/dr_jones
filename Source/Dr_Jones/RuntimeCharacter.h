// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Storage/ToolComponent.h"
#include "Interactive/InteractiveObject.h"
#include "RuntimeCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

UCLASS(Blueprintable)
class DR_JONES_API ARuntimeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ARuntimeCharacter();
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnUseItem OnActionKeyPressed;
	
	UPROPERTY()
	UToolComponent* ToolComponent;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void Turn(float AxisValue);
	void LookUp(float AxisValue);
	void PrimaryAction();
	void Interact();
	void SwitchItem(float AxisValue);
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
