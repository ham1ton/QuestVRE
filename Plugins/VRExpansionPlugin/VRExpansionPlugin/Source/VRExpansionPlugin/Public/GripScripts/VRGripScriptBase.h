// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "VRBPDatatypes.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "Tickable.h"
#include "Net/UnrealNetwork.h"

#include "VRGripScriptBase.generated.h"

class UGripMotionControllerComponent;

UENUM(Blueprintable)
enum class EGSTransformOverrideType : uint8
{
	/** Does not alter the world transform */
	None,

	/* Overrides the world transform */
	OverridesWorldTransform,

	/* Modifies the world transform*/
	ModifiesWorldTransform
};

UCLASS(NotBlueprintable, BlueprintType, EditInlineNew, DefaultToInstanced, Abstract, ClassGroup = (VRExpansionPlugin), HideCategories = DefaultSettings)
class VREXPANSIONPLUGIN_API UVRGripScriptBase : public UObject, public FTickableGameObject
{
	GENERATED_BODY()
public:

	UVRGripScriptBase(const FObjectInitializer& ObjectInitializer);

	bool IsSupportedForNetworking() const override
	{
		return true;
		//return bRequiresReplicationSupport || Super::IsSupportedForNetworking();
	}
	// I don't need to do this, there should be no dynamic script spawning and they are all name stable by default
	
	// Returns if the script is currently active and should be used
	bool IsScriptActive();

	// Is currently active helper variable, returned from IsScriptActive()
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "DefaultSettings")
	bool bIsActive;

	// Returns if the script is going to modify the world transform of the grip
	EGSTransformOverrideType GetWorldTransformOverrideType();

	// Whether this script overrides or modifies the world transform
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "DefaultSettings")
	EGSTransformOverrideType WorldTransformOverrideType;

	// Returns if the script wants auto drop to be ignored
	bool Wants_DenyAutoDrop();

	// Returns if we want to deny auto dropping
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "DefaultSettings")
		bool bDenyAutoDrop;

	// Returns if the script wants to force a drop
	bool Wants_ToForceDrop();

	// Returns if we want to force a drop
	UPROPERTY(BlueprintReadWrite, Category = "DefaultSettings")
		bool bForceDrop;

	// Flags the grip to be dropped as soon as possible
	UFUNCTION(BlueprintCallable, Category = "VRGripScript")
	void ForceGripToDrop()
	{
		bForceDrop = true;
	}

	// Returns if the script wants to deny late updates
	bool Wants_DenyLateUpdates();

	// Returns if we want to deny late updates
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "DefaultSettings")
		bool bDenyLateUpdates;

	// Returns if the script is currently active and should be used
	/*UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VRGripScript")
	bool Wants_DenyTeleport();
	virtual bool Wants_DenyTeleport_Implementation();*/

	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const override;
	
	// doesn't currently compile in editor builds, not sure why the linker is screwing up there but works elsewhere
	//virtual void PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker);
	virtual bool CallRemoteFunction(UFunction * Function, void * Parms, FOutParmRec * OutParms, FFrame * Stack) override;
	virtual int32 GetFunctionCallspace(UFunction * Function, FFrame * Stack) override;

	// FTickableGameObject functions
	
	
	// If true then this scrip can tick when bAllowticking is true
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Tick Settings")
		bool bCanEverTick;

	// If true and we bCanEverTick, then will fire off the tick function
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tick Settings")
		bool bAllowTicking;

	// Set whether the grip script can tick or not
	UFUNCTION(BlueprintCallable, Category = "Tick Settings")
		void SetTickEnabled(bool bTickEnabled);

	/**
	 * Function called every frame on this GripScript. Override this function to implement custom logic to be executed every frame.
	 * Only executes if bCanEverTick is true and bAllowTicking is true
	 *
	 * @param DeltaTime - The time since the last tick.
	 */
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;
	virtual bool IsTickableInEditor() const;
	virtual bool IsTickableWhenPaused() const override;
	virtual ETickableTickType GetTickableTickType() const;
	virtual TStatId GetStatId() const override;

	// End tickable object information


	// Returns the expected grip transform (relative * controller + addition)
	UFUNCTION(BlueprintPure, Category = "VRGripScript")
		FTransform GetGripTransform(const FBPActorGripInformation &Grip, const FTransform & ParentTransform);

	// Returns the current world transform of the owning object (or root comp of if it is an actor)
	UFUNCTION(BlueprintPure, Category = "VRGripScript")
		FTransform GetParentTransform(bool bGetWorldTransform = true);

	// Returns the parent component or actor to this
	UFUNCTION(BlueprintPure, Category = "VRGripScript")
		UObject * GetParent();

	// Returns the owning actor 
	UFUNCTION(BlueprintPure, Category = "VRGripScript")
		AActor * GetOwner();

	// If the owning actor has authority on this connection 
	UFUNCTION(BlueprintPure, Category = "VRGripScript")
		bool HasAuthority();

	// If the owning actor is on the server on this connection 
	UFUNCTION(BlueprintPure, Category = "VRGripScript")
		bool IsServer();

	void EndPlay(const EEndPlayReason::Type EndPlayReason);

	// Not all scripts will require this function, specific ones that use things like Lever logic however will. Best to call it.
	// Grippables will automatically call this, however if you manually spawn a grip script during play or you make your own
	// Interfaced grip object and give it grippables, YOU will be required to call this event on them.
	UFUNCTION(BlueprintNativeEvent, Category = "VRGripScript")
		void OnEndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void OnEndPlay_Implementation(const EEndPlayReason::Type EndPlayReason);

	void BeginPlay(UObject * CallingOwner);

	// Not all scripts will require this function, specific ones that use things like Lever logic however will. Best to call it.
	// Grippables will automatically call this, however if you manually spawn a grip script during play or you make your own
	// Interfaced grip object and give it grippables, YOU will be required to call this event on them.
	UFUNCTION(BlueprintNativeEvent, Category = "VRGripScript")
		void OnBeginPlay(UObject * CallingOwner);
		virtual void OnBeginPlay_Implementation(UObject * CallingOwner);

	// Overrides or Modifies the world transform with this grip script
	UFUNCTION(BlueprintNativeEvent, Category = "VRGripScript")
		bool GetWorldTransform(UGripMotionControllerComponent * GrippingController, float DeltaTime, UPARAM(ref) FTransform & WorldTransform, const FTransform &ParentTransform, UPARAM(ref) FBPActorGripInformation &Grip, AActor * actor, UPrimitiveComponent * root, bool bRootHasInterface, bool bActorHasInterface, bool bIsForTeleport);
		virtual bool GetWorldTransform_Implementation(UGripMotionControllerComponent * OwningController, float DeltaTime, FTransform & WorldTransform, const FTransform &ParentTransform, FBPActorGripInformation &Grip, AActor * actor, UPrimitiveComponent * root, bool bRootHasInterface, bool bActorHasInterface, bool bIsForTeleport);

	// Event triggered on the interfaced object when gripped
	UFUNCTION(BlueprintNativeEvent, Category = "VRGripScript")
		void OnGrip(UGripMotionControllerComponent * GrippingController, const FBPActorGripInformation & GripInformation);
		virtual void OnGrip_Implementation(UGripMotionControllerComponent * GrippingController, const FBPActorGripInformation & GripInformation);

	// Event triggered on the interfaced object when grip is released
	UFUNCTION(BlueprintNativeEvent, Category = "VRGripScript")
	void OnGripRelease(UGripMotionControllerComponent * ReleasingController, const FBPActorGripInformation & GripInformation, bool bWasSocketed = false);
	virtual void OnGripRelease_Implementation(UGripMotionControllerComponent * ReleasingController, const FBPActorGripInformation & GripInformation, bool bWasSocketed = false);

	// Event triggered on the interfaced object when secondary gripped
	UFUNCTION(BlueprintNativeEvent, Category = "VRGripInterface")
	void OnSecondaryGrip(UGripMotionControllerComponent * Controller, USceneComponent * SecondaryGripComponent, const FBPActorGripInformation & GripInformation);
	virtual void OnSecondaryGrip_Implementation(UGripMotionControllerComponent * Controller, USceneComponent * SecondaryGripComponent, const FBPActorGripInformation & GripInformation);

	// Event triggered on the interfaced object when secondary grip is released
	UFUNCTION(BlueprintNativeEvent, Category = "VRGripInterface")
	void OnSecondaryGripRelease(UGripMotionControllerComponent * Controller, USceneComponent * ReleasingSecondaryGripComponent, const FBPActorGripInformation & GripInformation);
	virtual void OnSecondaryGripRelease_Implementation(UGripMotionControllerComponent * Controller, USceneComponent * ReleasingSecondaryGripComponent, const FBPActorGripInformation & GripInformation);



	virtual bool CallCorrect_GetWorldTransform(UGripMotionControllerComponent * OwningController, float DeltaTime, FTransform & WorldTransform, const FTransform &ParentTransform, FBPActorGripInformation &Grip, AActor * actor, UPrimitiveComponent * root, bool bRootHasInterface, bool bActorHasInterface, bool bIsForTeleport)
	{
		return GetWorldTransform_Implementation(OwningController, DeltaTime, WorldTransform, ParentTransform, Grip, actor, root, bRootHasInterface, bActorHasInterface, bIsForTeleport);
	}
};


UCLASS(Blueprintable, Abstract, ClassGroup = (VRExpansionPlugin), ShowCategories = DefaultSettings)
class VREXPANSIONPLUGIN_API UVRGripScriptBaseBP : public UVRGripScriptBase
{
	GENERATED_BODY()
public:

	virtual bool CallCorrect_GetWorldTransform(UGripMotionControllerComponent * OwningController, float DeltaTime, FTransform & WorldTransform, const FTransform &ParentTransform, FBPActorGripInformation &Grip, AActor * actor, UPrimitiveComponent * root, bool bRootHasInterface, bool bActorHasInterface, bool bIsForTeleport) override
	{
		return GetWorldTransform(OwningController, DeltaTime, WorldTransform, ParentTransform, Grip, actor, root, bRootHasInterface, bActorHasInterface, bIsForTeleport);
	}

	virtual void Tick(float DeltaTime) override;

	/** Event called every frame if ticking is enabled */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Tick"))
		void ReceiveTick(float DeltaSeconds);
};