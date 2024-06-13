// Property of Night Shift Games, all rights reserved.

#include "Artifact.h"

#include "ArtifactFactory.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "StaticMeshLODResourcesToDynamicMesh.h"
#include "Components/DynamicMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/StaticMeshSocket.h"
#include "Equipment/EquipmentComponent.h"
#include "GeometryScript/MeshAssetFunctions.h"
#include "Interaction/InteractableComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Managment/Dr_JonesGameModeBase.h"
#include "Quest/QuestMessages.h"
#include "Utilities/LocalMeshOctree.h"
#include "Utilities/Utilities.h"
#include "World/Illuminati.h"

AArtifact::AArtifact()
{
	ArtifactMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArtifactMesh"));
	SetRootComponent(ArtifactMeshComponent);

	ArtifactDynamicMesh = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("ArtifactDynamicMesh"));
	ArtifactDynamicMesh->SetupAttachment(ArtifactMeshComponent);

	LocalMeshOctree = CreateDefaultSubobject<ULocalMeshOctree>(TEXT("ArtifactMeshOctree"));

	if (ArtifactStaticMesh)
	{
		UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ArtifactMeshComponent);
		StaticMeshComponent->SetStaticMesh(ArtifactStaticMesh);
	}
}

void AArtifact::BeginPlay()
{
	Super::BeginPlay();

	InteractableComponent->InteractDelegate.AddDynamic(this, &AArtifact::PickUp);

	SetupDynamicArtifact();
}

void AArtifact::OnConstruction(const FTransform& Transform)
{
	SetupDynamicArtifact();

	if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ArtifactMeshComponent))
	{
		StaticMeshComponent->SetStaticMesh(ArtifactStaticMesh);
		// ArtifactDynamicMaterial = StaticMeshComponent->CreateDynamicMaterialInstance(0);
	}
}

#if WITH_EDITOR
void AArtifact::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AArtifact, ArtifactID))
	{
		// Pull Data from Database
		const FArtifactData* ArtifactData = UArtifactFactory::PullArtifactDataFromDatabase(ArtifactID);
		if (!ArtifactData)
		{
			return;
		}
		// Set Data
		SetupArtifact(*ArtifactData);
	}
	if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ArtifactMeshComponent))
	{
		StaticMeshComponent->SetStaticMesh(ArtifactStaticMesh);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void AArtifact::PickUp(ADrJonesCharacter* Taker)
{
	checkf(Taker, TEXT("Player is missing!"));
	UEquipmentComponent* EquipmentComponent = Taker->GetEquipment();
	if (!EquipmentComponent && !EquipmentComponent->CanPickUpItem())
	{
		return;
	}
	EquipmentComponent->AddItem(this);
	OnArtifactPickup.ExecuteIfBound(this);
	OnArtifactPickedUp(Taker);

	AIlluminati::SendQuestMessage<UArtifactCollectedQuestMessage>(this, [&](UArtifactCollectedQuestMessage* ArtifactCollectedMessage)
	{
		ArtifactCollectedMessage->Artifact = this;
	});
}

UMeshComponent* AArtifact::GetMeshComponent() const
{
	if (IsDynamic())
	{
		return ArtifactDynamicMesh;
	}
	else
	{
		return ArtifactMeshComponent;
	}
}

void AArtifact::SetupArtifact(const FArtifactData& ArtifactData)
{
	ArtifactID = ArtifactData.ArtifactID;
	ArtifactStaticMesh = !ArtifactData.ArtifactMesh.IsEmpty() ? ArtifactData.ArtifactMesh[FMath::RandRange(0, ArtifactData.ArtifactMesh.Num() - 1)].LoadSynchronous() : nullptr;
	if (ArtifactStaticMesh)
	{
		Cast<UStaticMeshComponent>(ArtifactMeshComponent)->SetStaticMesh(ArtifactStaticMesh);
	}
	ArtifactName = ArtifactData.Name;
	ArtifactDescription = ArtifactData.Description;
	ArtifactAge = FMath::RandRange(ArtifactData.AgeMin, ArtifactData.AgeMax);
	ItemIcon = ArtifactData.ArtifactIcon.LoadSynchronous();
	ArtifactUsage = ArtifactData.Usage;
	ArtifactCulture = ArtifactData.Culture;
	ArtifactSize = ArtifactData.Size;
	// TODO: Artifact Rarity should be based on Usage & Wear & Age / Wear.
	ArtifactRarity = ArtifactData.Rarity;
	// TODO: Artifact Wear should be rand based on Age.
	ArtifactWear = ArtifactData.Wear;

	// TODO: Generate these from the above values?
	DirtData.DustAmount = FMath::FRand();
	DirtData.MudAmount = FMath::FRand();
	DirtData.RustAmount = FMath::FRand();
	DirtData.MoldAmount = FMath::FRand();

	SetupDynamicArtifact();
}

void AArtifact::SetupDynamicArtifact()
{
	using namespace UE::Geometry;

	// TODO: Unhardcode and fill out the rest.
	DirtData.MudMPI = FMaterialParameterInfo(TEXT("DirtIntensity"), BlendParameter, 0);

	// TODO: Generate these from the above values?
	DirtData.DustAmount = FMath::FRand();
	DirtData.MudAmount = FMath::FRand();
	DirtData.RustAmount = FMath::FRand();
	DirtData.MoldAmount = FMath::FRand();

	if (!ArtifactStaticMesh)
	{
		return;
	}

	WhispersOfThePastData.Reset();

	FString SocketName;
	for (UStaticMeshSocket* Socket : ArtifactStaticMesh->Sockets)
	{
		// TODO: Unhardcode socket prefix
		static const FString SocketPrefix = TEXT("ACS_");

		Socket->SocketName.ToString(SocketName);
		if (SocketName.StartsWith(SocketPrefix))
		{
			FArtifactWhisperOfThePastData& Data = WhispersOfThePastData.Emplace_GetRef();
			Data.InteractableTransform = FTransform(Socket->RelativeRotation, Socket->RelativeLocation, Socket->RelativeScale);
		}
	}

	FStaticMeshRenderData* RenderData = ArtifactStaticMesh->GetRenderData();
	if (!RenderData || RenderData->LODResources.IsEmpty())
	{
		return;
	}

	const FStaticMeshLODResources* LODResources = &RenderData->LODResources[0];
	FDynamicMesh3 DynamicMesh;
	FStaticMeshLODResourcesToDynamicMesh Converter;
	Converter.Convert(LODResources, {}, DynamicMesh);

	check(ArtifactDynamicMesh);
	ArtifactDynamicMesh->SetMesh(MoveTemp(DynamicMesh));
	ArtifactDynamicMesh->EditMesh([&](FDynamicMesh3& Mesh)
	{
		using namespace UE::Geometry;
		if (!ensure(Mesh.HasAttributes()))
		{
			Mesh.EnableAttributes();
		}
		FDynamicMeshAttributeSet* Attributes = Mesh.Attributes();

		// Don't worry if this triggers - it's just a remainder to opierdolic patryk G.
		if (!ensure(Attributes->HasPrimaryColors()))
		{
			Attributes->EnablePrimaryColors();
			FDynamicMeshColorOverlay* ColorOverlay = Attributes->PrimaryColors();
			for (int32 TriID = 0; TriID < Mesh.TriangleCount(); ++TriID)
			{
				FVector4f Color = FLinearColor::White;
				const int32 A = ColorOverlay->AppendElement(FVector4f(Color));
				const int32 B = ColorOverlay->AppendElement(FVector4f(Color));
				const int32 C = ColorOverlay->AppendElement(FVector4f(Color));
				ColorOverlay->SetTriangle(TriID, FIndex3i(A, B, C));
			}
		}
		FDynamicMeshColorOverlay* ColorOverlay = Attributes->PrimaryColors();

		TArray<int32> VertexElements;
		for (int32 VtxID : Mesh.VertexIndicesItr())
		{
			// Vtx color based on multiple face vertices
			FVector4f AvgColor = FVector4f::Zero();
			ColorOverlay->GetVertexElements(VtxID, VertexElements);
			for (int32 Elem : VertexElements)
			{
				FVector4f ElemColor;
				ColorOverlay->GetElement(Elem, ElemColor);
				AvgColor += ElemColor;
			}
			AvgColor /= VertexElements.Num();

			FVector4f FinalColor;

			auto SetupColor = [&](float& OutColor, float Amount)
			{
				// TODO: Hardcoded thresholds!
				if (Amount < 0.15f)
				{
					OutColor = 0.0f;
					return;
				}

				const float RandVal = FMath::FRand();
				// For now treat Red (X) as probability
				OutColor = RandVal * FMath::Max(/*Base:*/ 0.3f, AvgColor.X) * Amount;
				// Scale this thing up because of the amount of modulation that's just taken place
				OutColor = FMath::Pow(OutColor, 0.3f);
				if (OutColor < 0.2f)
				{
					OutColor = 0.0f;
				}
			};

			SetupColor(FinalColor.X, DirtData.DustAmount);
			SetupColor(FinalColor.Y, DirtData.MudAmount);
			SetupColor(FinalColor.Z, DirtData.RustAmount);
			SetupColor(FinalColor.W, DirtData.MoldAmount);

			for (int32 Elem : VertexElements)
			{
				ColorOverlay->SetElement(Elem, FinalColor);
			}
		}
	});

	check(ArtifactMeshComponent);
	ArtifactMeshComponent->SetVisibility(false);

	ArtifactDynamicMaterial = UMaterialInstanceDynamic::Create(ArtifactStaticMesh->GetMaterial(0), ArtifactDynamicMesh);
	ArtifactDynamicMesh->SetMaterial(0, ArtifactDynamicMaterial);

	ArtifactDynamicMaterial->SetScalarParameterValueByInfo(DirtData.RustMPI, 1.0f);
	ArtifactDynamicMaterial->SetScalarParameterValueByInfo(DirtData.MudMPI, 1.0f);
	ArtifactDynamicMaterial->SetScalarParameterValueByInfo(DirtData.DustMPI, 1.0f);

	LocalMeshOctree->BuildFromMesh(ArtifactStaticMesh);
}

void AArtifact::VertexPaint(const FVector& LocalPosition, const FVector& LocalNormal, const FColor& Color, const FVector4f& ChannelMask, float BrushRadiusWS, float BrushFalloffPow) const
{
	check(IsInGameThread());

	if (!IsDynamic())
	{
		return;
	}

	if (!ensureMsgf(BrushRadiusWS > 0.0f, TEXT("Zero or less radius is not going to produce any result.")))
	{
		return;
	}

	static TArray<FLocalMeshOctreeVertex> Vertices;
	Vertices.Reset();

	check(LocalMeshOctree);
	LocalMeshOctree->FindVerticesInBoundingBox(LocalPosition, FVector(BrushRadiusWS), Vertices);

	ArtifactDynamicMesh->EditMesh([&](FDynamicMesh3& Mesh)
	{
		using namespace UE::Geometry;
		if (!ensure(Mesh.HasAttributes()))
		{
			Mesh.EnableAttributes();
		}
		FDynamicMeshAttributeSet* Attributes = Mesh.Attributes();

		if (!ensure(Attributes->HasPrimaryColors()))
		{
			Attributes->EnablePrimaryColors();
		}
		FDynamicMeshColorOverlay* ColorOverlay = Attributes->PrimaryColors();

		for (const FLocalMeshOctreeVertex& Vertex : Vertices)
		{
			const double DistanceSquaredToVertex = FVector::DistSquared(Vertex.Position, LocalPosition);
			const double BrushRadiusSquared = FMath::Square(BrushRadiusWS);
			if (DistanceSquaredToVertex > BrushRadiusSquared)
			{
				continue;
			}

			if (Vertex.Normal.Dot(LocalNormal) <= 0)
			{
				continue;
			}

			const float Alpha = FMath::Pow(static_cast<float>((BrushRadiusSquared - DistanceSquaredToVertex) / BrushRadiusSquared), BrushFalloffPow);

			for (const int32 Tri : Mesh.VtxTrianglesItr(Vertex.Index))
			{
				const int32 Elem = ColorOverlay->GetElementIDAtVertex(Tri, Vertex.Index);
				if (!ensure(Elem >= 0))
				{
					continue;
				}
				FLinearColor LinearColor(Color);
				LinearColor *= ChannelMask;
				FLinearColor ElemValue = ColorOverlay->GetElement(Elem);
				ColorOverlay->SetElement(Elem, FMath::Lerp(ElemValue, LinearColor, Alpha));
			}
		}
	});
}

void AArtifact::CleanCompletely() const
{
	if (!IsDynamic())
	{
		return;
	}

	ArtifactDynamicMesh->EditMesh([&](FDynamicMesh3& Mesh)
	{
		using namespace UE::Geometry;
		if (!ensure(Mesh.HasAttributes()))
		{
			Mesh.EnableAttributes();
		}
		FDynamicMeshAttributeSet* Attributes = Mesh.Attributes();

		if (!ensure(Attributes->HasPrimaryColors()))
		{
			Attributes->EnablePrimaryColors();
		}
		FDynamicMeshColorOverlay* ColorOverlay = Attributes->PrimaryColors();

		for (const int32 Elem : ColorOverlay->ElementIndicesItr())
		{
			ColorOverlay->SetElement(Elem, FVector4f(0));
		}
	});
}

void AArtifact::OnRemovedFromEquipment()
{
	Super::OnRemovedFromEquipment();

	const ADrJonesCharacter* Player = GetInstigator<ADrJonesCharacter>();
	const FVector GroundLocation = GetLocationOfItemAfterDropdown();
	const FRotator Rotation = Player ? FRotator(0.0, Player->GetActorRotation().Yaw, 0.0) : FRotator::ZeroRotator; 

	SetActorLocationAndRotation(GroundLocation, Rotation);
}

bool AArtifact::IsDynamic() const
{
	return ArtifactDynamicMesh->GetMesh() && LocalMeshOctree->MeshVertexOctree.GetNumNodes() > 0;
}

TArray<FName> AArtifact::GetArtifactIDs()
{
	const UDataTable* ArtifactDatabase = GetDefault<UNightShiftSettings>()->ArtifactDataTable.LoadSynchronous();
	return ArtifactDatabase->GetRowNames();
}

UWorld* UArtifactInteractionMode::GetWorld() const
{
	if (CurrentArtifact)
	{
		return CurrentArtifact->GetWorld();
	}
	return UObject::GetWorld();
}

void UArtifactInteractionMode::Begin(ADrJonesCharacter& Character, AArtifact& Artifact)
{
	if (!ensureMsgf(!CurrentArtifact, TEXT("The artifact cleaning mode is already active.")))
	{
		return;
	}

	CurrentArtifact = &Artifact;
	ControllingCharacter = &Character;

	if (APlayerController* Controller = GetControllingCharacter()->GetController<APlayerController>())
	{
		OnBindInput(*Controller);
	}

	OnBegin();
}

void UArtifactInteractionMode::End(ADrJonesCharacter& Character)
{
	if (!ensureMsgf(CurrentArtifact, TEXT("The artifact cleaning mode is not active.")))
	{
		return;
	}

	if (APlayerController* Controller = GetControllingCharacter()->GetController<APlayerController>())
	{
		OnUnbindInput(*Controller);
	}

	ensure(&Character == ControllingCharacter);
	OnEnd();

	CurrentArtifact = nullptr;
	ControllingCharacter = nullptr;
}

void UArtifactInteractionMode::OnBindInput(APlayerController& Controller)
{
	if (!InputMappingContext)
	{
		return;
	}

	const ULocalPlayer* LocalPlayer = Controller.GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->AddMappingContext(InputMappingContext, ArtifactInteractionMappingContextPriority);
}

void UArtifactInteractionMode::OnUnbindInput(APlayerController& Controller)
{
	if (!InputMappingContext)
	{
		return;
	}

	const ULocalPlayer* LocalPlayer = Controller.GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->RemoveMappingContext(InputMappingContext);
}

void UArtifactCleaningMode::OnBegin()
{
	// TODO: Switch on different cleaning tools
	CurrentPaintChannelMask = FVector4f(1);
}

void UArtifactCleaningMode::OnEnd()
{
}

void UArtifactCleaningMode::OnBindInput(APlayerController& Controller)
{
	Super::OnBindInput(Controller);

	if (UEnhancedInputComponent* InputComponent = Cast<UEnhancedInputComponent>(Controller.InputComponent); ensure(InputComponent))
	{
		if (BrushStrokeAction)
		{
			BrushStrokeActionBindingHandle = InputComponent->BindAction(BrushStrokeAction, ETriggerEvent::Triggered, this, &UArtifactCleaningMode::TickBrushStroke).GetHandle();
		}
	}
}

void UArtifactCleaningMode::OnUnbindInput(APlayerController& Controller)
{
	Super::OnUnbindInput(Controller);

	if (UEnhancedInputComponent* InputComponent = Cast<UEnhancedInputComponent>(Controller.InputComponent); ensure(InputComponent))
	{
		InputComponent->RemoveBindingByHandle(BrushStrokeActionBindingHandle);
	}
}

void UArtifactCleaningMode::TickBrushStroke()
{
	if (!ensure(GetCurrentArtifact()))
	{
		return;
	}

	AArtifact* Artifact = GetCurrentArtifact();
	if (!Artifact->IsDynamic())
	{
		return;
	}

	if (Artifact->CleaningProgress >= 1.0f)
	{
		return;
	}

	const FHitResult HitResult = Utilities::GetPlayerSightTarget(300.0f, *this, ECC_Visibility, true);
	if (HitResult.IsValidBlockingHit())
	{
		const FTransform& Transform = Artifact->GetActorTransform();
		const FVector LocalPosition = Transform.InverseTransformPosition(HitResult.Location);
		const FVector LocalNormal = Transform.InverseTransformVector(HitResult.Normal);
		Artifact->VertexPaint(LocalPosition, LocalNormal, FColor(0), CurrentPaintChannelMask, 5);
	}

	float TotalDirtValue = 0.0f;
	float CurrentDirtValue = 0.0f;
	check(Artifact->GetDynamicMeshComponent());
	Artifact->GetDynamicMeshComponent()->ProcessMesh([&](const FDynamicMesh3& Mesh)
	{
		using namespace UE::Geometry;
		if (!ensure(Mesh.HasAttributes()))
		{
			return;
		}
		const FDynamicMeshAttributeSet* Attributes = Mesh.Attributes();

		if (!ensure(Attributes->HasPrimaryColors()))
		{
			return;
		}
		const FDynamicMeshColorOverlay* ColorOverlay = Attributes->PrimaryColors();

		for (const int32 Elem : ColorOverlay->ElementIndicesItr())
		{
			const FVector4f Color = ColorOverlay->GetElement(Elem);
			// TODO: Unhardcode the component count
			TotalDirtValue += 3.0f;
			CurrentDirtValue += Color.X;
			CurrentDirtValue += Color.Y;
			CurrentDirtValue += Color.Z;
		}
	});

	if (!ensureMsgf(TotalDirtValue > 0, TEXT("Total dirt value was 0, which probably means the mesh has no vertices.")))
	{
		return;
	}

	if (TotalDirtValue > 0)
	{
		Artifact->CleaningProgress = FMath::Clamp(1.0f - CurrentDirtValue / TotalDirtValue, 0.0f, 1.0f);
	}
	else
	{
		Artifact->CleaningProgress = 1.0f;
	}

	if (Artifact->CleaningProgress >= CleaningCompletedThreshold)
	{
		Artifact->CleaningProgress = 1.0f;
		Artifact->CleanCompletely();
		OnArtifactCleaned.Broadcast(Artifact);
	}
}

void UArtifactIdentificationMode::Tick(float DeltaSeconds)
{
	AArtifact* Artifact = GetCurrentArtifact();
	if (!Artifact || !Artifact->IsDynamic())
	{
		TryChangePointedSphere(nullptr);
		return;
	}

	const FHitResult HitResult = Utilities::GetPlayerSightTarget(300.0f, *this, ECC_Visibility, true);
	if (!HitResult.IsValidBlockingHit() || HitResult.GetActor() != Artifact)
	{
		TryChangePointedSphere(nullptr);
		return;
	}

	UArtifactIdentificationSphereComponent* Sphere = Cast<UArtifactIdentificationSphereComponent>(HitResult.GetComponent());
	if (Sphere && Sphere->bVisibleOnlyInFront)
	{
		ADrJonesCharacter* Owner = GetTypedOuter<ADrJonesCharacter>();
		if (!Owner)
		{
			TryChangePointedSphere(nullptr);
			return;
		}

		APlayerController* Controller = Owner->GetController<APlayerController>();
		if (!Controller)
		{
			TryChangePointedSphere(nullptr);
			return;
		}

		int32 ViewportX, ViewportY;
		Controller->GetViewportSize(ViewportX, ViewportY);
		FVector2D ScreenCenter = FVector2D(ViewportX / 2.0, ViewportY / 2.0);

		FVector WorldLocation, WorldDirection;
		if (!Controller->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection))
		{
			TryChangePointedSphere(nullptr);
			return;
		}

		// right vector because blender snaps things with Y as front, and we're setting up the sockets using snap's system
		const double Dot = FVector::DotProduct(-Sphere->GetRightVector(), WorldDirection);
		if (Dot <= 0)
		{
			TryChangePointedSphere(nullptr);
			return;
		}
	}

	TryChangePointedSphere(Sphere);
}

void UArtifactIdentificationMode::OnBegin()
{
	check(GetCurrentArtifact());
	for (auto It = GetCurrentArtifact()->WhispersOfThePastData.CreateConstIterator(); It; ++It)
	{
		const FArtifactWhisperOfThePastData& Data = *It;
		const FTransform& Transform = Data.InteractableTransform;
		UArtifactIdentificationSphereComponent* Sphere = ExactCast<UArtifactIdentificationSphereComponent>(GetCurrentArtifact()->AddComponentByClass(UArtifactIdentificationSphereComponent::StaticClass(), true, Transform, true));
		check(Sphere);

		Sphere->SetupAttachment(GetCurrentArtifact()->GetMeshComponent());
		// TODO: Unhardcode sphere radius
		Sphere->InitSphereRadius(3.0f);
		Sphere->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

		GetCurrentArtifact()->FinishAddComponent(Sphere, true, Transform);
		GetCurrentArtifact()->AddInstanceComponent(Sphere);

		Sphere->Type = EArtifactIdentificationSphereType::WhispersOfThePast;
		Sphere->IndexInWOTPArray = It.GetIndex();
	}
}

void UArtifactIdentificationMode::OnEnd()
{
	AArtifact* Artifact = GetCurrentArtifact();
	check(Artifact);

	TArray<UArtifactIdentificationSphereComponent*, TInlineAllocator<8>> Spheres;
	Artifact->GetComponents(Spheres);
	for (UArtifactIdentificationSphereComponent* Sphere : Spheres)
	{
		Artifact->RemoveInstanceComponent(Sphere);
		Sphere->DestroyComponent();
	}
}

void UArtifactIdentificationMode::OnBindInput(APlayerController& Controller)
{
	Super::OnBindInput(Controller);

	if (UEnhancedInputComponent* InputComponent = Cast<UEnhancedInputComponent>(Controller.InputComponent); ensure(InputComponent))
	{
		if (InteractAction)
		{
			InteractActionBindingHandle = InputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &UArtifactIdentificationMode::Interact).GetHandle();
		}
	}
}

void UArtifactIdentificationMode::OnUnbindInput(APlayerController& Controller)
{
	Super::OnUnbindInput(Controller);

	if (UEnhancedInputComponent* InputComponent = Cast<UEnhancedInputComponent>(Controller.InputComponent); ensure(InputComponent))
	{
		InputComponent->RemoveBindingByHandle(InteractActionBindingHandle);
	}
}

void UArtifactIdentificationMode::TryChangePointedSphere(UArtifactIdentificationSphereComponent* NewSphere)
{
	UArtifactIdentificationSphereComponent* OldSphere = PointedIdentificationSphere;
	PointedIdentificationSphere = NewSphere;
	if (PointedIdentificationSphere != OldSphere)
	{
		OnPointedSphereChanged(OldSphere, PointedIdentificationSphere);
	}
}

void UArtifactIdentificationMode::OnPointedSphereChanged(UArtifactIdentificationSphereComponent* OldSphere, UArtifactIdentificationSphereComponent* NewSphere)
{
	AArtifact* Artifact = GetCurrentArtifact();
	if (!Artifact) return;
	if (!Artifact->ArtifactDynamicMaterial) return;

	Artifact->ArtifactDynamicMaterial->SetVectorParameterValueByInfo(SphereMaskPositionMPI, NewSphere ? NewSphere->GetRelativeLocation() : FVector::ZeroVector);
	Artifact->ArtifactDynamicMaterial->SetScalarParameterValueByInfo(SphereMaskRadiusMPI, NewSphere ? NewSphere->GetScaledSphereRadius() : 0.0f);
}

void UArtifactIdentificationMode::Interact()
{
	AArtifact* Artifact = GetCurrentArtifact();
	if (!PointedIdentificationSphere) return;

	switch (PointedIdentificationSphere->Type)
	{
		case EArtifactIdentificationSphereType::WhispersOfThePast:
		{
			if (!Artifact->WhispersOfThePastData.IsValidIndex(PointedIdentificationSphere->IndexInWOTPArray))
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Cos sie zrypalo."));
				return;
			}

			const FArtifactWhisperOfThePastData& Data = Artifact->WhispersOfThePastData[PointedIdentificationSphere->IndexInWOTPArray];
#if UE_ENABLE_DEBUG_DRAWING
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Interacted with WOTP symbol [%i]"), PointedIdentificationSphere->IndexInWOTPArray));
#endif
			// TODO: podswietlenie pokazanie dodanie do jurnala itp
			OnArtifactIdentified.Broadcast(Artifact);
			break;
		}
	}
}
