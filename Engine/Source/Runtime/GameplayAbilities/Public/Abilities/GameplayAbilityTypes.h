// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayAbilityTypes.generated.h"

class UGameplayEffect;
class UAnimInstance;
class UAbilitySystemComponent;
class UGameplayAbility;
class AGameplayAbilityTargetActor;
class UAbilityTask;
class UAttributeSet;


UENUM(BlueprintType)
namespace EGameplayAbilityInstancingPolicy
{
	/**
	 *	How the ability is instanced when executed. This limits what an ability can do in its implementation. For example, a NonInstanced
	 *	Ability cannot have state. It is probably unsafe for an InstancedPerActor ability to have latent actions, etc.
	 */

	enum Type
	{
		NonInstanced,			// This ability is never instanced. Anything that executes the ability is operating on the CDO.
		InstancedPerActor,		// Each actor gets their own instance of this ability. State can be saved, replication is possible.
		InstancedPerExecution,	// We instance this ability each time it is executed. Replication possible but not recommended.
	};
}

UENUM(BlueprintType)
namespace EGameplayAbilityNetExecutionPolicy
{
	/**
	 *	How does an ability execute on the network. Does a client "ask and predict", "ask and wait", "don't ask"
	 */

	enum Type
	{
		Predictive		UMETA(DisplayName = "Predictive"),	// Part of this ability runs predictively on the client.
		Server			UMETA(DisplayName = "Server"),		// This ability must be OK'd by the server before doing anything on a client.
		Client			UMETA(DisplayName = "Client"),		// This ability runs as long the client says it does.
	};
}

UENUM(BlueprintType)
namespace EGameplayAbilityReplicationPolicy
{
	/**
	 *	How an ability replicates state/events to everyone on the network.
	 */

	enum Type
	{
		ReplicateNone		UMETA(DisplayName = "Replicate None"),	// We don't replicate the instance of the ability to anyone.
		ReplicateAll		UMETA(DisplayName = "Replicate All"),	// We replicate the instance of the ability to everyone (even simulating clients).
		ReplicateOwner		UMETA(DisplayName = "Replicate Owner"),	// Only replicate the instance of the ability to the owner.
	};
}


USTRUCT(BlueprintType)
struct FGameplayAbilityHandle
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityHandle()
	: Handle(INDEX_NONE)
	{

	}

	FGameplayAbilityHandle(int32 InHandle)
		: Handle(InHandle)
	{

	}

	bool IsValid() const
	{
		return Handle != INDEX_NONE;
	}

	FGameplayAbilityHandle GetNextHandle()
	{
		return FGameplayAbilityHandle(Handle + 1);
	}

	bool operator==(const FGameplayAbilityHandle& Other) const
	{
		return Handle == Other.Handle;
	}

	bool operator!=(const FGameplayAbilityHandle& Other) const
	{
		return Handle != Other.Handle;
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("%d"), Handle);
	}

private:

	UPROPERTY()
	int32 Handle;
};


USTRUCT()
struct FGameplayAbilityInputIDPair
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityInputIDPair()
	: Ability(nullptr), InputID(-1) { }

	FGameplayAbilityInputIDPair(UGameplayAbility* InAbility, int32 InInputID)
	: Ability(InAbility), InputID(InInputID) { }

	UPROPERTY()
	UGameplayAbility* Ability;

	UPROPERTY()
	int32	InputID;
};


UENUM(BlueprintType)
namespace EGameplayAbilityActivationMode
{
	enum Type
	{
		Authority,			// We are the authority activating this ability
		NonAuthority,		// We are not the authority but aren't predicting yet. This is a mostly invalid state to be in.

		Predicting,			// We are predicting the activation of this ability
		Confirmed,			// We are not the authority, but the authority has confirmed this activation
	};
}

/**
*	FGameplayAbilityActorInfo
*
*	Cached data associated with an Actor using an Ability.
*		-Initialized from an AActor* in InitFromActor
*		-Abilities use this to know what to actor upon. E.g., instead of being coupled to a specific actor class.
*		-These are generally passed around as pointers to support polymorphism.
*		-Projects can override UAbilitySystemGlobals::AllocAbilityActorInfo to override the default struct type that is created.
*
*/
USTRUCT(BlueprintType)
struct GAMEPLAYABILITIES_API FGameplayAbilityActorInfo
{
	GENERATED_USTRUCT_BODY()

	virtual ~FGameplayAbilityActorInfo() {}

	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	TWeakObjectPtr<AActor>	Actor;

	/** PlayerController associated with this actor. This will often be null! */
	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	TWeakObjectPtr<APlayerController>	PlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	TWeakObjectPtr<UAnimInstance>	AnimInstance;

	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	TWeakObjectPtr<UAbilitySystemComponent>	AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	TWeakObjectPtr<UMovementComponent>	MovementComponent;

	bool IsLocallyControlled() const;

	bool IsNetAuthority() const;

	virtual void InitFromActor(AActor *Actor, UAbilitySystemComponent* InAbilitySystemComponent);
	virtual void ClearActorInfo();
};

/**
*	FGameplayAbilityActorInfo
*
*	Contains data used to determine locations for targeting systems.
*
*/


/**
*	FGameplayAbilityActivationInfo
*
*	Data tied to a specific activation of an ability.
*		-Tell us whether we are the authority, if we are predicting, confirmed, etc.
*		-Holds current and previous PredictionKey
*		-Generally not meant to be subclassed in projects.
*		-Passed around by value since the struct is small.
*/

USTRUCT(BlueprintType)
struct GAMEPLAYABILITIES_API FGameplayAbilityActivationInfo
{
	GENERATED_USTRUCT_BODY()

	FGameplayAbilityActivationInfo()
	: ActivationMode(EGameplayAbilityActivationMode::Authority)
	, PrevPredictionKey(0)
	, CurrPredictionKey(0)
	{

	}

	FGameplayAbilityActivationInfo(AActor * InActor, uint32 InPrevPredictionKey, uint32 InCurrPredictionKey)
		: PrevPredictionKey(InPrevPredictionKey)
		, CurrPredictionKey(InCurrPredictionKey)
	{
		// On Init, we are either Authority or NonAuthority. We haven't been given a PredictionKey and we haven't been confirmed.
		// NonAuthority essentially means 'I'm not sure what how I'm going to do this yet'.
		ActivationMode = (InActor->Role == ROLE_Authority ? EGameplayAbilityActivationMode::Authority : EGameplayAbilityActivationMode::NonAuthority);
	}

	FGameplayAbilityActivationInfo(EGameplayAbilityActivationMode::Type InType, uint32 InPrevPredictionKey = 0, uint32 InCurrPredictionKey = 0)
		: ActivationMode(InType)
		, PrevPredictionKey(InPrevPredictionKey)
		, CurrPredictionKey(InCurrPredictionKey)
	{
	}


	void GeneratePredictionKey(UAbilitySystemComponent * Component) const;

	void SetActivationConfirmed();

	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	mutable TEnumAsByte<EGameplayAbilityActivationMode::Type>	ActivationMode;

	UPROPERTY()
	mutable uint32 PrevPredictionKey;

	UPROPERTY()
	mutable uint32 CurrPredictionKey;
};


// ----------------------------------------------------

USTRUCT(BlueprintType)
struct GAMEPLAYABILITIES_API FGameplayEventData
{
	GENERATED_USTRUCT_BODY()

	FGameplayEventData()
	: Instigator(NULL)
	, Target(NULL)
	, PrevPredictionKey(0)
	, CurrPredictionKey(0)
	{
		// do nothing
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayAbilityTriggerPayload)
	AActor* Instigator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayAbilityTriggerPayload)
	AActor* Target;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayAbilityTriggerPayload)
	float Var1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayAbilityTriggerPayload)
	float Var2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayAbilityTriggerPayload)
	float Var3;

	uint32 PrevPredictionKey;
	uint32 CurrPredictionKey;
};

/** 
 *	Structure that tells AbilitySystemComponent what to bind to an InputComponent (see BindAbilityActivationToInputComponent) 
 *	
 */
struct FGameplayAbiliyInputBinds
{
	FGameplayAbiliyInputBinds(FString InConfirmTargetCommand, FString InCancelTargetCommand, FString InEnumName)
	: ConfirmTargetCommand(InConfirmTargetCommand)
	, CancelTargetCommand(InCancelTargetCommand)
	, EnumName(InEnumName)
	{ }

	/** Defines command string that will be bound to Confirm Targeting */
	FString ConfirmTargetCommand;

	/** Defines command string that will be bound to Cancel Targeting */
	FString CancelTargetCommand;

	/** Returns enum to use for ability bings. E.g., "Ability1"-"Ability8" input commands will be bound to ability activations inside the AbiltiySystemComponent */
	FString	EnumName;

	UEnum* GetBindEnum() { return FindObject<UEnum>(ANY_PACKAGE, *EnumName); }
};

/** Used for cleaning up predicted data on network clients */
DECLARE_MULTICAST_DELEGATE(FAbilitySystemComponentPredictionKeyClear);

/** Generic delegate for ability 'events'/notifies */
DECLARE_MULTICAST_DELEGATE_OneParam(FGenericAbilityDelegate, UGameplayAbility*);
