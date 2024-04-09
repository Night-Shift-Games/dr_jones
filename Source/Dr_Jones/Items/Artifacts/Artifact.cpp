// Property of Night Shift Games, all rights reserved.

#include "Artifact.h"

#include "ArtifactFactory.h"
#include "EnhancedInputSubsystems.h"
#include "StaticMeshLODResourcesToDynamicMesh.h"
#include "Components/DynamicMeshComponent.h"
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
	DirtData.RustAmount = FMath::FRand();
	DirtData.MudAmount = FMath::FRand();
	DirtData.DustAmount = FMath::FRand();

	SetupDynamicArtifact();
}

void AArtifact::SetupDynamicArtifact()
{
	using namespace UE::Geometry;

	// TODO: Generate these from the above values?
	DirtData.RustAmount = FMath::FRand();
	DirtData.MudAmount = FMath::FRand();
	DirtData.DustAmount = FMath::FRand();

	if (!ArtifactStaticMesh)
	{
		return;
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
		if (!Mesh.HasAttributes())
		{
			Mesh.EnableAttributes();
		}
		FDynamicMeshAttributeSet* Attributes = Mesh.Attributes();

		if (!Attributes->HasPrimaryColors())
		{
			Attributes->EnablePrimaryColors();
		}
		else
		{
			Attributes->PrimaryColors()->ClearElements();
		}
		FDynamicMeshColorOverlay* ColorOverlay = Attributes->PrimaryColors();

		for (int32 TriID = 0; TriID < Mesh.TriangleCount(); ++TriID)
		{
			// TODO: Take the probability mask into consideration

			FVector4f Color;
			Color.X = DirtData.RustAmount;
			Color.Y = DirtData.MudAmount;
			Color.Z = DirtData.DustAmount;
			Color.W = 0.0f;

			const int32 A = ColorOverlay->AppendElement(FVector4f(Color));
			const int32 B = ColorOverlay->AppendElement(FVector4f(Color));
			const int32 C = ColorOverlay->AppendElement(FVector4f(Color));
			ColorOverlay->SetTriangle(TriID, FIndex3i(A, B, C));
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

UWorld* UArtifactCleaningMode::GetWorld() const
{
	if (CurrentArtifact)
	{
		return CurrentArtifact->GetWorld();
	}
	return UObject::GetWorld();
}

void UArtifactCleaningMode::Begin(const ADrJonesCharacter& Character, AArtifact& Artifact)
{
	if (!ensureMsgf(!CurrentArtifact, TEXT("The artifact cleaning mode is already active.")))
	{
		return;
	}

	CurrentArtifact = &Artifact;

	// TODO: Switch on different cleaning tools
	CurrentPaintChannelMask = FVector4f(1);

	if (!InputMappingContext)
	{
		return;
	}

	if (const APlayerController* Controller = Character.GetController<APlayerController>())
	{
		if (const ULocalPlayer* LocalPlayer = Controller->GetLocalPlayer())
		{
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->AddMappingContext(InputMappingContext, 690000);
		}
	}
}

void UArtifactCleaningMode::End(const ADrJonesCharacter& Character)
{
	if (!ensureMsgf(CurrentArtifact, TEXT("The artifact cleaning mode is not active.")))
	{
		return;
	}

	CurrentArtifact = nullptr;

	if (!InputMappingContext)
	{
		return;
	}

	if (const APlayerController* Controller = Character.GetController<APlayerController>())
	{
		if (const ULocalPlayer* LocalPlayer = Controller->GetLocalPlayer())
		{
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->RemoveMappingContext(InputMappingContext);
		}
	}
}

void UArtifactCleaningMode::TickBrushStroke()
{
	if (!ensure(CurrentArtifact))
	{
		return;
	}

	if (!CurrentArtifact->IsDynamic())
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
		const FVector LocalPosition = CurrentArtifact->GetActorTransform().InverseTransformPosition(HitResult.Location);
		CurrentArtifact->VertexPaint(LocalPosition, FColor(0), CurrentPaintChannelMask);
	}

	float TotalDirtValue = 0.0f;
	float CurrentDirtValue = 0.0f;
	check(CurrentArtifact->GetDynamicMeshComponent());
	CurrentArtifact->GetDynamicMeshComponent()->ProcessMesh([&](const FDynamicMesh3& Mesh)
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
		CurrentArtifact->CleanCompletely();
		OnArtifactCleaned.Broadcast(CurrentArtifact);
	}
}
