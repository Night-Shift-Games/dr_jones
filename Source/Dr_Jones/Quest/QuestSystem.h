// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "QuestSystem.generated.h"

class UQuest;

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

UCLASS(Blueprintable)
class DR_JONES_API UQuest : public UObject
{
	GENERATED_BODY()

public:
	// Handle can be invalid if the quest has not been registered yet.
	TOptional<FQuestHandle> GetHandle() const;

	UFUNCTION(BlueprintPure, Category = "Quest")
	bool IsRegistered() const;

private:
	TOptional<FQuestHandle> HandleInRegistry;

	UPROPERTY(BlueprintReadOnly, Category = "Quest", meta = (AllowPrivateAccess = true))
	uint8 bCompleted : 1;

	friend class UQuestSystemComponent;
};

UCLASS(ClassGroup = (DrJones), meta = (BlueprintSpawnableComponent))
class DR_JONES_API UQuestSystemComponent : public UActorComponent
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Quest")
	UPARAM(DisplayName = "Quest Object") UQuest* AddQuest(const FQuestDescription& QuestDescription);

public:
	FQuestHandle RegisterQuest(UQuest& Quest);

private:
	UPROPERTY()
	TMap<FQuestHandle, UQuest*> QuestRegistry;
};
