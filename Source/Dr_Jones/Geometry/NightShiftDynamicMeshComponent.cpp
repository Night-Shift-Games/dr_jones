// Property of Night Shift Games, all rights reserved.

#include "NightShiftDynamicMeshComponent.h"

#include "DynamicMeshBuilder.h"
#include "MaterialDomain.h"
#include "Materials/MaterialRenderProxy.h"

class FNightShiftDynamicMeshSceneProxy final : public FPrimitiveSceneProxy
{
public:
	SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

	FNightShiftDynamicMeshSceneProxy(UNightShiftDynamicMeshComponent* Component)
		: FPrimitiveSceneProxy(Component)
		, VertexFactory(GetScene().GetFeatureLevel(), "FNightShiftDynamicMeshSceneProxy")
		, MaterialRelevance(Component->GetMaterialRelevance(GetScene().GetFeatureLevel()))
	{
		check(Component);

		TArray<FDynamicMeshVertex> DynamicVertices;
		DynamicVertices.AddUninitialized(Component->Vertices.Num());
		IndexBuffer.Indices.AddUninitialized(Component->Triangles.Num() * 3);

		struct FVertexTangents
		{
			FVector TangentX;
			FVector TangentZ;
			FVector TangentY;
		};
		TArray<FVertexTangents> Tangents;
		Tangents.AddUninitialized(Component->Vertices.Num());
		for (auto It = Component->Triangles.CreateConstIterator(); It; ++It)
		{
			const UE::Geometry::FIndex3i& Triangle = *It;
			const int32 TriIndex = It.GetIndex();

			check(Triangle.A < Component->Vertices.Num());
			check(Triangle.B < Component->Vertices.Num());
			check(Triangle.C < Component->Vertices.Num());

			IndexBuffer.Indices[TriIndex * 3 + 0] = Triangle.A;
			IndexBuffer.Indices[TriIndex * 3 + 1] = Triangle.B;
			IndexBuffer.Indices[TriIndex * 3 + 2] = Triangle.C;
		}

		check(Component->Vertices.Num() == Component->Colors.Num());
		for (auto It = Component->Vertices.CreateConstIterator(); It; ++It)
		{
			const int32 VertIndex = It.GetIndex();

			FDynamicMeshVertex Vert;
			Vert.Color = Component->Colors[VertIndex];
			Vert.Position = FVector3f(*It);
			Vert.TangentZ = FVector(0, 0, 1);

			DynamicVertices[VertIndex] = Vert;
		}

		VertexBuffers.InitFromDynamicVertex(&VertexFactory, DynamicVertices);

		BeginInitResource(&VertexBuffers.PositionVertexBuffer);
		BeginInitResource(&VertexBuffers.StaticMeshVertexBuffer);
		BeginInitResource(&VertexBuffers.ColorVertexBuffer);
		BeginInitResource(&IndexBuffer);
		BeginInitResource(&VertexFactory);

		Material = Component->GetMaterial(0);
		if (!Material)
		{
			Material = UMaterial::GetDefaultMaterial(MD_Surface);
		}
	}

	virtual ~FNightShiftDynamicMeshSceneProxy() override
	{
		VertexBuffers.PositionVertexBuffer.ReleaseResource();
		VertexBuffers.StaticMeshVertexBuffer.ReleaseResource();
		VertexBuffers.ColorVertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
		QUICK_SCOPE_CYCLE_COUNTER( STAT_CustomMeshSceneProxy_GetDynamicMeshElements );

		const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;
		FMaterialRenderProxy* MaterialProxy;
		if (bWireframe)
		{
			MaterialProxy = new FColoredMaterialRenderProxy(
				GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy() : nullptr,
				FLinearColor(0.98f, 0.41f, 0.02f)
			);
			Collector.RegisterOneFrameMaterialProxy(MaterialProxy);
		}
		else
		{
			MaterialProxy = Material->GetRenderProxy();
		}

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				FMeshBatch& Mesh = Collector.AllocateMesh();
				FMeshBatchElement& BatchElement = Mesh.Elements[0];
				BatchElement.IndexBuffer = &IndexBuffer;
				Mesh.bWireframe = bWireframe;
				Mesh.VertexFactory = &VertexFactory;
				Mesh.MaterialRenderProxy = MaterialProxy;

				bool bHasPrecomputedVolumetricLightmap;
				FMatrix PreviousLocalToWorld;
				int32 SingleCaptureIndex;
				bool bOutputVelocity;
				GetScene().GetPrimitiveUniformShaderParameters_RenderThread(GetPrimitiveSceneInfo(), bHasPrecomputedVolumetricLightmap, PreviousLocalToWorld, SingleCaptureIndex, bOutputVelocity);
				bOutputVelocity |= AlwaysHasVelocity();

				FDynamicPrimitiveUniformBuffer& DynamicPrimitiveUniformBuffer = Collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();
				DynamicPrimitiveUniformBuffer.Set(GetLocalToWorld(), PreviousLocalToWorld, GetBounds(), GetLocalBounds(), true, bHasPrecomputedVolumetricLightmap, bOutputVelocity);
				BatchElement.PrimitiveUniformBufferResource = &DynamicPrimitiveUniformBuffer.UniformBuffer;

				BatchElement.FirstIndex = 0;
				BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 3;
				BatchElement.MinVertexIndex = 0;
				BatchElement.MaxVertexIndex = VertexBuffers.PositionVertexBuffer.GetNumVertices() - 1;
				Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
				Mesh.Type = PT_TriangleList;
				Mesh.DepthPriorityGroup = SDPG_World;
				Mesh.bCanApplyViewModeOverrides = false;
				Collector.AddMesh(ViewIndex, Mesh);
			}
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = IsShown(View);
		Result.bShadowRelevance = IsShadowCast(View);
		Result.bDynamicRelevance = true;
		Result.bRenderInMainPass = ShouldRenderInMainPass();
		Result.bUsesLightingChannels = GetLightingChannelMask() != GetDefaultLightingChannelMask();
		Result.bRenderCustomDepth = ShouldRenderCustomDepth();
		Result.bTranslucentSelfShadow = bCastVolumetricTranslucentShadow;
		MaterialRelevance.SetPrimitiveViewRelevance(Result);
		Result.bVelocityRelevance = DrawsVelocity() && Result.bOpaque && Result.bRenderInMainPass;
		return Result;
	}

	virtual bool CanBeOccluded() const override
	{
		return !MaterialRelevance.bDisableDepthTest;
	}

	virtual uint32 GetMemoryFootprint() const override { return sizeof(*this) + GetAllocatedSize(); }
	SIZE_T GetAllocatedSize() const { return FPrimitiveSceneProxy::GetAllocatedSize(); }

private:

	UMaterialInterface* Material;
	FStaticMeshVertexBuffers VertexBuffers;
	FDynamicMeshIndexBuffer32 IndexBuffer;
	FLocalVertexFactory VertexFactory;

	FMaterialRelevance MaterialRelevance;
};

UNightShiftDynamicMeshComponent::UNightShiftDynamicMeshComponent()
{
}

int32 UNightShiftDynamicMeshComponent::GetNumMaterials() const
{
	return 1;
}

FPrimitiveSceneProxy* UNightShiftDynamicMeshComponent::CreateSceneProxy()
{
	return !Triangles.IsEmpty() ? new FNightShiftDynamicMeshSceneProxy(this) : nullptr;
}

FBoxSphereBounds UNightShiftDynamicMeshComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	FBox BoundingBox(ForceInit);
	for (const FVector& Vertex : Vertices)
	{
		BoundingBox += LocalToWorld.TransformPosition(Vertex);
	}
	return FBoxSphereBounds(BoundingBox);
}
