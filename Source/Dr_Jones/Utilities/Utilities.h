#pragma once

#include "NightShiftMacros.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "Player/DrJonesCharacter.h"
#include "UI/DrJonesWidgetBase.h"

#include "Utilities.generated.h"

class ADrJonesCharacter;

namespace Utilities
{
	ADrJonesCharacter& GetPlayerCharacter(const UObject& WorldContextObject);
	float CalculateAngleBetweenTwoVectors(const FVector& Direction, const FVector& Second);
	float GetChordLength(const float SphereRadius, const FVector& Direction, const FVector& Second);
	bool IsPointInSphere(const FVector& Point, const FVector& SphereOrigin, const float Radius);
	FVector FindGround(const UObject& WorldContextObject, const FVector& StartLocation, const TArray<const AActor*>& ActorsToIgnore = {});
	uint32 WrapIndexToSize(int64 Index, uint32 Size);
	double GetMeshZOffset(const AActor& Actor);
	UDrJonesWidgetBase* GetWidget(const UObject& WorldContextObject, const TSubclassOf<UDrJonesWidgetBase> WidgetClass);
	UWidgetManager& GetWidgetManager(const UObject& WorldContextObject);
	FHitResult GetPlayerSightTarget(const float Reach,  const UObject& WorldContextObject, ECollisionChannel CollisionChannel = ECC_Visibility, bool bTraceComplex = false);
	void DrawInteractionDebugInfo(const FVector& WorldLocation, const FVector& LineEnd, const FHitResult& Hit);
	bool IsValueBetween(double Value, float A, float B);
	
	template <class TWidgetClass>
	TWidgetClass* GetWidget(const UObject& WorldContextObject, const TSubclassOf<UDrJonesWidgetBase> WidgetClass)
	{
		UDrJonesWidgetBase* Widget = GetWidget(WorldContextObject, WidgetClass);
		return Widget ? Cast<TWidgetClass>(Widget) : nullptr;
	}

	template <typename T>
	uint32 WrapIndexToArray(int64 Index, TArray<T> Array)
	{
		return WrapIndexToSize(Index, Array.Num());
	}

	template <typename T>
	uint32 WrapIndexToArray(int64 Index, TSet<T> Array)
	{
		return WrapIndexToSize(Index, Array.Num());
	}

	template<typename T>
	typename std::enable_if<std::is_base_of<UActorComponent, T>::value, T*>::type
	FindComponentFromCDO(UClass* InActorClass)
	{
		NS_EARLY_R(!IsValid(InActorClass), nullptr);

		const AActor* ActorCDO = InActorClass->GetDefaultObject<AActor>();
		if (const UActorComponent* FoundComponent = ActorCDO->FindComponentByClass<T>(); FoundComponent != nullptr)
		{
			return Cast<T>(FoundComponent);
		}

		UBlueprintGeneratedClass* RootBlueprintGeneratedClass = Cast<UBlueprintGeneratedClass>(InActorClass);
		UClass* ActorClass = InActorClass;
		
		do
		{
			const UBlueprintGeneratedClass* ActorBlueprintGeneratedClass = Cast<UBlueprintGeneratedClass>(ActorClass);
			NS_EARLY_R(!ActorBlueprintGeneratedClass, nullptr);

			const TArray<USCS_Node*>& ActorBlueprintNodes = ActorBlueprintGeneratedClass->SimpleConstructionScript->GetAllNodes();
			for (const USCS_Node* Node : ActorBlueprintNodes)
			{
				if (Node->ComponentClass->IsChildOf<T>())
				{
					return Cast<T>(Node->GetActualComponentTemplate(RootBlueprintGeneratedClass));
				}
			}
			ActorClass = Cast<UClass>(ActorClass->GetSuperStruct());

		} while (ActorClass != AActor::StaticClass());

		return nullptr;
	}; 
	
	template <typename T, typename TEnableIf<TIsDerivedFrom<T, AActor>::Value>::Type* = nullptr>
	void FindObjectsOfClass(const UObject* WorldContextObject, TArray<T*>& OutObjects)
	{
		OutObjects.Reset();

		check(GEngine);
		const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		NS_EARLY(!World);
		
		for (TObjectIterator<T> It(World, T::StaticClass()); It; ++It)
		{
			T* Object = *It;
			OutObjects.Add(Object);
		}
	}
}	

UCLASS()
class UDrJonesUtilitiesFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "DrJones|Utilities", meta = (WorldContext = "WorldContextObject"))
	static ADrJonesCharacter* GetDrJonesCharacter(const UObject* WorldContextObject)
	{ return WorldContextObject ? &Utilities::GetPlayerCharacter(*WorldContextObject) : nullptr; }

	UFUNCTION(BlueprintPure, Category = "DrJones|Utilities", meta = (WorldContext = "WorldContextObject"))
	static FVector FindGround(const UObject* WorldContextObject, const FVector& StartLocation)
	{ return WorldContextObject ? Utilities::FindGround(*WorldContextObject, StartLocation) : FVector::ZeroVector; }

	UFUNCTION(BlueprintPure, Category = "DrJones|Utilities", meta = (WorldContext = "WorldContextObject"))
	static int64 WrapIndexToSize(int64 Index, int32 Size)
	{ return Utilities::WrapIndexToSize(Index, Size); }

	UFUNCTION(BlueprintPure, Category = "DrJones|Utilities", meta = (WorldContext = "WorldContextObject"))
	static FHitResult GetHitResultOfPlayerSight(const UObject* WorldContextObject, const float Reach = 400.f)
	{ return WorldContextObject ? Utilities::GetPlayerSightTarget(Reach, *WorldContextObject) : FHitResult(); }

	UFUNCTION(BlueprintPure, Category = "DrJones|Utilities", meta = (WorldContext = "WorldContextObject"))
	static UWidgetManager* GetWidgetManager(const UObject* WorldContextObject)
	{ return WorldContextObject ? &Utilities::GetWidgetManager(*WorldContextObject) : nullptr; }

	UFUNCTION(BlueprintPure, Category = "DrJones|Utilities")
	static bool IsValueBetween(double Value, float Less, float More)
	{ return Utilities::IsValueBetween(Value, Less, More);}
};