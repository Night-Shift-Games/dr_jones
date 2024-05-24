// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Clock.h"
#include "ArchaeologicalSite/ArchaeologicalSite.h"
#include "Quest/QuestSystem.h"
#include "UObject/Object.h"
#include "UObject/WeakObjectPtr.h"

#include "Illuminati.generated.h"

class IQuestMessageInterface;
class USoundCue;
class UQuestSystemComponent;

inline TAutoConsoleVariable CVarIlluminatiDebug(
	TEXT("NS.Illuminati.Debug"),
	false,
	TEXT("Enable Illuminati debug logging."),
	ECVF_Cheat
);

USTRUCT(BlueprintType)
struct FWorldEventHandle
{
	GENERATED_BODY()

	uint32 Key;
	inline static uint32 KeyCounter = 0;

	FWorldEventHandle() : Key(KeyCounter++) {}

	bool operator==(const FWorldEventHandle& Other) const { return Key == Other.Key; }
};

inline int32 GetTypeHash(const FWorldEventHandle& WorldEventHandle)
{
	return GetTypeHash(WorldEventHandle.Key);
}

USTRUCT(BlueprintType, Category = "Illuminati")
struct FIlluminatiGlobalEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global Event")
	TObjectPtr<USoundBase> RadioSound;
};

DECLARE_DYNAMIC_DELEGATE(FWorldEventDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGlobalEventReceivedDelegate, const FIlluminatiGlobalEvent&, GlobalEvent);

class FIlluminatiDelegates
{
public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnGlobalEventReceivedDelegate, const FIlluminatiGlobalEvent&);
	static inline FOnGlobalEventReceivedDelegate OnGlobalEventReceived;
};

UCLASS(Blueprintable, ClassGroup = "DrJones", HideCategories = (Replication, Actor, Physics, Cooking))
class DR_JONES_API AIlluminati : public AInfo
{
	GENERATED_BODY()

public:
	AIlluminati();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "World Event", meta = (AutoCreateRefTerm = "GlobalEvent"))
	void PostGlobalEvent(const FIlluminatiGlobalEvent& GlobalEvent) const;

	UFUNCTION(BlueprintPure, Category = "Illuminati", meta = (WorldContext = "WorldContextObject"))
	static AIlluminati* GetIlluminatiInstance(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Illuminati", meta = (WorldContext = "WorldContextObject"))
	static UQuestSystemComponent* GetQuestSystemInstance(const UObject* WorldContextObject);

	template <typename T, typename FFunc>
	static void SendQuestMessage(const UObject* WorldContextObject, FFunc Initializer)
	{
		UQuestSystemComponent* QuestSystem = GetQuestSystemInstance(WorldContextObject);
		check(QuestSystem);
		T* ArtifactCleanedMessage = NewObject<T>();
		Initializer(ArtifactCleanedMessage);
		QuestSystem->SendQuestMessage(ArtifactCleanedMessage);
	}

	void FillArchaeologicalSites();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, SaveGame, Category = "DrJones|Clock")
	TObjectPtr<UClock> Clock; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DrJones|Clock")
	FDateTime InitialTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DrJones|Locations")
	TArray<FArchaeologicalSiteFactoryData> DefaultSites;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DrJones|Locations")
	TMap<FName, TObjectPtr<UArchaeologicalSite>> ArchaeologicalSites;
	
	////////////////////

	UPROPERTY(BlueprintAssignable, Category = "Global Event")
	FOnGlobalEventReceivedDelegate OnGlobalEventReceived;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	TObjectPtr<UQuestSystemComponent> QuestSystemComponent;;
};
