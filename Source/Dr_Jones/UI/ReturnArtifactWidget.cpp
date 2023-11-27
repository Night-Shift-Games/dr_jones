// Property of Night Shift Games, all rights reserved.

#include "ReturnArtifactWidget.h"

#include "Kismet/GameplayStatics.h"

void UReturnArtifactWidget::OnShow()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	PlayerController->SetShowMouseCursor(true);
	PlayerController->SetInputMode(FInputModeUIOnly());
}

void UReturnArtifactWidget::OnHide()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	PlayerController->SetShowMouseCursor(false);
	PlayerController->SetInputMode(FInputModeGameOnly());
}
