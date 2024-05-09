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

	WhispersOfThePastData.Interactables.Reset();

	FString SocketName;
	for (UStaticMeshSocket* Socket : ArtifactStaticMesh->Sockets)
	{
		// TODO: Unhardcode socket prefix
		static const FString SocketPrefix = TEXT("ACS_");

		Socket->SocketName.ToString(SocketName);
		if (SocketName.StartsWith(SocketPrefix))
		{
			WhispersOfThePastData.Interactables.Add(FTransform(Socket->RelativeRotation, Socket->RelativeLocation, Socket->RelativeScale));
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

			// For now treat Red as probability
			const float RandVal = FMath::FRand();
			if (RandVal <= AvgColor.X)
			{
				FinalColor.X = RandVal <= DirtData.DustAmount ? 1.0f : 0.0f;
				FinalColor.Y = RandVal <= DirtData.MudAmount ? 1.0f : 0.0f;
				FinalColor.Z = RandVal <= DirtData.RustAmount ? 1.0f : 0.0f;
				FinalColor.W = RandVal <= DirtData.MoldAmount ? 1.0f : 0.0f;
			}
			else
			{
				FinalColor = FVector4f::Zero();
			}

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

void AArtifact::VertexPaint(const FVector& LocalPosition, const FColor& Color, const FVector4f& ChannelMask, float BrushRadiusWS, float BrushFalloffPow) const
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

	if (CleaningProgress >= 1.0f)
	{
		return;
	}

	const FHitResult HitResult = Utilities::GetPlayerSightTarget(300.0f, *this);
	if (HitResult.IsValidBlockingHit())
	{
		const FVector LocalPosition = Artifact->GetActorTransform().InverseTransformPosition(HitResult.Location);
		Artifact->VertexPaint(LocalPosition, FColor(0), CurrentPaintChannelMask);
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
		CleaningProgress = FMath::Clamp(1.0f - CurrentDirtValue / TotalDirtValue, 0.0f, 1.0f);
	}
	else
	{
		CleaningProgress = 1.0f;
	}

	if (CleaningProgress >= CleaningCompletedThreshold)
	{
		CleaningProgress = 1.0f;
		Artifact->CleanCompletely();
		OnArtifactCleaned.Broadcast(Artifact);
	}
}

void UArtifactIdentificationMode::OnBegin()
{
	check(GetCurrentArtifact());
	for (const FTransform& Transform : GetCurrentArtifact()->WhispersOfThePastData.Interactables)
	{
		USphereComponent* Sphere = ExactCast<USphereComponent>(GetCurrentArtifact()->AddComponentByClass(USphereComponent::StaticClass(), true, Transform, true));
		check(Sphere);

		Sphere->SetupAttachment(GetCurrentArtifact()->GetMeshComponent());
		// TODO: Unhardcode sphere radius
		Sphere->InitSphereRadius(3.0f);
		GetCurrentArtifact()->FinishAddComponent(Sphere, true, Transform);
		GetCurrentArtifact()->AddInstanceComponent(Sphere);

		Sphere->ComponentTags.Add(SphereComponentTag);
	}
}

void UArtifactIdentificationMode::OnEnd()
{
	check(GetCurrentArtifact());
	TArray<USphereComponent*, TInlineAllocator<8>> Spheres;
	GetCurrentArtifact()->GetComponents(Spheres);
	for (USphereComponent* Sphere : Spheres)
	{
		if (!Sphere->ComponentHasTag(SphereComponentTag))
		{
			continue;
		}

		GetCurrentArtifact()->RemoveInstanceComponent(Sphere);
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

void UArtifactIdentificationMode::Interact()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Interacted with artifact during identification!"));
}
