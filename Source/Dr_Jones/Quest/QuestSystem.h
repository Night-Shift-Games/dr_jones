// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Dr_Jones.h"
#include "Components/ActorComponent.h"

#include "QuestSystem.generated.h"

class UQuest;

inline TAutoConsoleVariable CVarQuestSystemDebug(
	TEXT("NS.QuestSystem.Debug"),
	false,
	TEXT("Enable Quest System debug logging."),
	ECVF_Cheat
);

USTRUCT(BlueprintType)
struct FQuestHandle
{
	GENERATED_BODY()

	using KeyType = uint32;
	static constexpr uint32 InvalidKey = TNumericLimits<uint32>::Max();

	static FQuestHandle New()
	{
		// TODO: *Handle* this case in a better way
		checkf(KeyCounter < InvalidKey, TEXT("Cannot make more quest handles."));
		FQuestHandle Handle;
		Handle.Key = KeyCounter++;
		return Handle;
	}

	static FQuestHandle NewInvalid()
	{
		FQuestHandle Handle;
		Handle.Key = InvalidKey;
		return Handle;
	}

	bool IsValid() const
	{
		return Key != InvalidKey;
	}

	KeyType GetKey() const
	{
		return Key;
	}

	FQuestHandle() = default;
	FQuestHandle(const FQuestHandle& Other) = default;
	FQuestHandle(FQuestHandle&& Other) = default;

	bool operator==(const FQuestHandle& Other) const
	{
		return Key == Other.Key;
	}

	friend uint32 GetTypeHash(const FQuestHandle& Handle)
	{
		return GetTypeHash(Handle.Key);
	}

private:
	KeyType Key;
	inline static KeyType KeyCounter = 0;

	friend class UQuestSystemComponent;
};

USTRUCT(BlueprintType)
struct FQuestDescription
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UQuest> QuestClass;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuestCompletedDelegate);

UCLASS(Blueprintable, Abstract)
class DR_JONES_API UQuest : public UObject
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE(FNotifyCompletedDelegate);

	virtual UWorld* GetWorld() const override;

	void InitializePending();
	void SendQuestMessage(const TScriptInterface<IQuestMessageInterface>& QuestMessage);

	// Handle can be invalid if the quest has not been registered yet.
	TOptional<FQuestHandle> GetHandle() const;

	UFUNCTION(BlueprintPure, Category = "Quest")
	const FQuestDescription& GetDescription() const;

	UFUNCTION(BlueprintPure, Category = "Quest")
	bool IsRegistered() const;

	UFUNCTION(BlueprintPure, Category = "Quest")
	bool IsCompleted() const;

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void MarkAsCompleted();

protected:
	// Called when a pending task should initialize its properties/events.
	// When a new quest is added or, for an existing quest, when the level is started.
	UFUNCTION(BlueprintImplementableEvent, Category = "Quest")
	void OnInitializePending();

	UFUNCTION(BlueprintImplementableEvent, Category = "Quest")
	void OnQuestMessageReceived(const TScriptInterface<IQuestMessageInterface>& QuestMessage);

public:
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestCompletedDelegate OnQuestCompleted;

private:
	TOptional<FQuestHandle> HandleInRegistry;
	FNotifyCompletedDelegate NotifyCompletedDelegate;

	UPROPERTY(BlueprintReadOnly, Category = "Quest", meta = (AllowPrivateAccess = true))
	FQuestDescription QuestDescription;

	UPROPERTY(BlueprintReadOnly, Category = "Quest", meta = (AllowPrivateAccess = true))
	uint8 bCompleted : 1;

	friend class UQuestSystemComponent;
};

UCLASS(Blueprintable)
class UQuestChain : public UObject
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE(FNotifyCompletedDelegate);

	bool HasAnyQuest() const { return !Quests.IsEmpty(); }

	void InitializeChain(UQuestSystemComponent* QuestSystemComponent);

	UFUNCTION()
	void OnQuestCompleted();

protected:
	const FQuestDescription* GetNextQuest() const;
	bool HasNextQuest() const;
	void SetupNextQuest();

private:
	FNotifyCompletedDelegate NotifyCompletedDelegate;

	UPROPERTY(EditAnywhere, Category = "Quest")
	TArray<FQuestDescription> Quests;

	UPROPERTY()
	TObjectPtr<UQuestSystemComponent> OwningQuestSystem;

	mutable int32 InternalQuestIndex = TNumericLimits<int32>::Max();

	friend class UQuestSystemComponent;
};

UINTERFACE()
class UQuestMessageInterface : public UInterface
{
	GENERATED_BODY()
};

class IQuestMessageInterface
{
	GENERATED_BODY()

public:
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestChainCompletedDelegate, UQuestChain*, QuestChain);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGlobalQuestCompletedDelegate, UQuest*, Quest);

UCLASS(ClassGroup = (DrJones), meta = (BlueprintSpawnableComponent))
class DR_JONES_API UQuestSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void AddQuestChainByObject(UQuestChain* QuestChain);

	UFUNCTION(BlueprintCallable, Category = "Quest", meta = (AutoCreateRefTerm = "QuestChainClass"))
	void AddQuestChain(const TSubclassOf<UQuestChain>& QuestChainClass);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	UPARAM(DisplayName = "Quest Object") UQuest* AddQuest(const FQuestDescription& QuestDescription);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void SendQuestMessage(const TScriptInterface<IQuestMessageInterface>& QuestMessage);

	UFUNCTION(BlueprintPure, Category = "Quest")
	UQuest* FindQuest(const FQuestHandle& Handle) const;

protected:
	FQuestHandle RegisterQuest(UQuest& Quest);
	void InitializePendingQuest(UQuest& Quest);

public:
	UPROPERTY(BlueprintAssignable)
	FOnQuestChainCompletedDelegate OnQuestChainCompleted;

	UPROPERTY(BlueprintAssignable)
	FOnGlobalQuestCompletedDelegate OnGlobalQuestCompleted;

private:
	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Quest", meta = (AllowPrivateAccess = true))
	TArray<FQuestHandle> PendingQuests;

	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Quest", meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UQuestChain>> PendingQuestChains;

	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Quest", meta = (AllowPrivateAccess = true))
	TMap<FQuestHandle, UQuest*> QuestRegistry;
};

namespace QuestSystemUtils
{
#if !NO_LOGGING
	static bool IsDebugEnabled() { return CVarQuestSystemDebug.GetValueOnGameThread(); }

	template<int32 Length, typename... Types>
	static void LogDebug(const TCHAR(&Format)[Length], Types&&... Args)
	{
		check(GEngine);
		const FString Message = FString::Printf(Format, Forward<Types>(Args)...);
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Turquoise, Message);
		// Yep. TEXT("%s"). I don't care...
		UE_LOG(LogDrJones, Log, TEXT("%s"), *Message);
	}

	template<int32 Length, typename... Types>
	static void LogDebugIfEnabled(const TCHAR(&Format)[Length], Types&&... Args)
	{
		if (IsDebugEnabled())
		{
			LogDebug(Format, Forward<Types>(Args)...);
		}
	}
#else
	static constexpr bool IsDebugEnabled() { return false; }
	template<int32 Length, typename... Types> static void LogDebug(const TCHAR(&Format)[Length], Types&&... Args) { }
	template<int32 Length, typename... Types> static void LogDebugIfEnabled(const TCHAR(&Format)[Length], Types&&... Args) { }
#endif
}
