// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class AActor;
class UPrimitiveComponent;
struct FHitResult;
#ifdef VREXPANSIONPLUGIN_VRButtonComponent_generated_h
#error "VRButtonComponent.generated.h already included, missing '#pragma once' in VRButtonComponent.h"
#endif
#define VREXPANSIONPLUGIN_VRButtonComponent_generated_h

#define vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_44_DELEGATE \
struct _Script_VRExpansionPlugin_eventVRButtonStartedInteractionSignature_Parms \
{ \
	AActor* InteractingActor; \
	UPrimitiveComponent* InteractingComponent; \
}; \
static inline void FVRButtonStartedInteractionSignature_DelegateWrapper(const FMulticastScriptDelegate& VRButtonStartedInteractionSignature, AActor* InteractingActor, UPrimitiveComponent* InteractingComponent) \
{ \
	_Script_VRExpansionPlugin_eventVRButtonStartedInteractionSignature_Parms Parms; \
	Parms.InteractingActor=InteractingActor; \
	Parms.InteractingComponent=InteractingComponent; \
	VRButtonStartedInteractionSignature.ProcessMulticastDelegate<UObject>(&Parms); \
}


#define vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_41_DELEGATE \
struct _Script_VRExpansionPlugin_eventVRButtonStateChangedSignature_Parms \
{ \
	bool ButtonState; \
	AActor* InteractingActor; \
	UPrimitiveComponent* InteractingComponent; \
}; \
static inline void FVRButtonStateChangedSignature_DelegateWrapper(const FMulticastScriptDelegate& VRButtonStateChangedSignature, bool ButtonState, AActor* InteractingActor, UPrimitiveComponent* InteractingComponent) \
{ \
	_Script_VRExpansionPlugin_eventVRButtonStateChangedSignature_Parms Parms; \
	Parms.ButtonState=ButtonState ? true : false; \
	Parms.InteractingActor=InteractingActor; \
	Parms.InteractingComponent=InteractingComponent; \
	VRButtonStateChangedSignature.ProcessMulticastDelegate<UObject>(&Parms); \
}


#define vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_RPC_WRAPPERS \
	virtual bool IsValidOverlap_Implementation(UPrimitiveComponent* OverlapComponent); \
 \
	DECLARE_FUNCTION(execOnRep_InitialRelativeTransform) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnRep_InitialRelativeTransform(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execIsValidOverlap) \
	{ \
		P_GET_OBJECT(UPrimitiveComponent,Z_Param_OverlapComponent); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->IsValidOverlap_Implementation(Z_Param_OverlapComponent); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetButtonToRestingPosition) \
	{ \
		P_GET_UBOOL(Z_Param_bLerpToPosition); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->SetButtonToRestingPosition(Z_Param_bLerpToPosition); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetButtonState) \
	{ \
		P_GET_UBOOL(Z_Param_bNewButtonState); \
		P_GET_UBOOL(Z_Param_bCallButtonChangedEvent); \
		P_GET_UBOOL(Z_Param_bSnapIntoPosition); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->SetButtonState(Z_Param_bNewButtonState,Z_Param_bCallButtonChangedEvent,Z_Param_bSnapIntoPosition); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execResetInitialButtonLocation) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ResetInitialButtonLocation(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execIsButtonInUse) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->IsButtonInUse(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnOverlapEnd) \
	{ \
		P_GET_OBJECT(UPrimitiveComponent,Z_Param_OverlappedComp); \
		P_GET_OBJECT(AActor,Z_Param_OtherActor); \
		P_GET_OBJECT(UPrimitiveComponent,Z_Param_OtherComp); \
		P_GET_PROPERTY(UIntProperty,Z_Param_OtherBodyIndex); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnOverlapEnd(Z_Param_OverlappedComp,Z_Param_OtherActor,Z_Param_OtherComp,Z_Param_OtherBodyIndex); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnOverlapBegin) \
	{ \
		P_GET_OBJECT(UPrimitiveComponent,Z_Param_OverlappedComp); \
		P_GET_OBJECT(AActor,Z_Param_OtherActor); \
		P_GET_OBJECT(UPrimitiveComponent,Z_Param_OtherComp); \
		P_GET_PROPERTY(UIntProperty,Z_Param_OtherBodyIndex); \
		P_GET_UBOOL(Z_Param_bFromSweep); \
		P_GET_STRUCT_REF(FHitResult,Z_Param_Out_SweepResult); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnOverlapBegin(Z_Param_OverlappedComp,Z_Param_OtherActor,Z_Param_OtherComp,Z_Param_OtherBodyIndex,Z_Param_bFromSweep,Z_Param_Out_SweepResult); \
		P_NATIVE_END; \
	}


#define vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_RPC_WRAPPERS_NO_PURE_DECLS \
	virtual bool IsValidOverlap_Implementation(UPrimitiveComponent* OverlapComponent); \
 \
	DECLARE_FUNCTION(execOnRep_InitialRelativeTransform) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnRep_InitialRelativeTransform(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execIsValidOverlap) \
	{ \
		P_GET_OBJECT(UPrimitiveComponent,Z_Param_OverlapComponent); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->IsValidOverlap_Implementation(Z_Param_OverlapComponent); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetButtonToRestingPosition) \
	{ \
		P_GET_UBOOL(Z_Param_bLerpToPosition); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->SetButtonToRestingPosition(Z_Param_bLerpToPosition); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execSetButtonState) \
	{ \
		P_GET_UBOOL(Z_Param_bNewButtonState); \
		P_GET_UBOOL(Z_Param_bCallButtonChangedEvent); \
		P_GET_UBOOL(Z_Param_bSnapIntoPosition); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->SetButtonState(Z_Param_bNewButtonState,Z_Param_bCallButtonChangedEvent,Z_Param_bSnapIntoPosition); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execResetInitialButtonLocation) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->ResetInitialButtonLocation(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execIsButtonInUse) \
	{ \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(bool*)Z_Param__Result=P_THIS->IsButtonInUse(); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnOverlapEnd) \
	{ \
		P_GET_OBJECT(UPrimitiveComponent,Z_Param_OverlappedComp); \
		P_GET_OBJECT(AActor,Z_Param_OtherActor); \
		P_GET_OBJECT(UPrimitiveComponent,Z_Param_OtherComp); \
		P_GET_PROPERTY(UIntProperty,Z_Param_OtherBodyIndex); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnOverlapEnd(Z_Param_OverlappedComp,Z_Param_OtherActor,Z_Param_OtherComp,Z_Param_OtherBodyIndex); \
		P_NATIVE_END; \
	} \
 \
	DECLARE_FUNCTION(execOnOverlapBegin) \
	{ \
		P_GET_OBJECT(UPrimitiveComponent,Z_Param_OverlappedComp); \
		P_GET_OBJECT(AActor,Z_Param_OtherActor); \
		P_GET_OBJECT(UPrimitiveComponent,Z_Param_OtherComp); \
		P_GET_PROPERTY(UIntProperty,Z_Param_OtherBodyIndex); \
		P_GET_UBOOL(Z_Param_bFromSweep); \
		P_GET_STRUCT_REF(FHitResult,Z_Param_Out_SweepResult); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		P_THIS->OnOverlapBegin(Z_Param_OverlappedComp,Z_Param_OtherActor,Z_Param_OtherComp,Z_Param_OtherBodyIndex,Z_Param_bFromSweep,Z_Param_Out_SweepResult); \
		P_NATIVE_END; \
	}


#define vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_EVENT_PARMS \
	struct VRButtonComponent_eventIsValidOverlap_Parms \
	{ \
		UPrimitiveComponent* OverlapComponent; \
		bool ReturnValue; \
 \
		/** Constructor, initializes return property only **/ \
		VRButtonComponent_eventIsValidOverlap_Parms() \
			: ReturnValue(false) \
		{ \
		} \
	}; \
	struct VRButtonComponent_eventReceiveButtonBeginInteraction_Parms \
	{ \
		AActor* InteractingActor; \
		UPrimitiveComponent* InteractingComponent; \
	}; \
	struct VRButtonComponent_eventReceiveButtonEndInteraction_Parms \
	{ \
		AActor* LastInteractingActor; \
		UPrimitiveComponent* LastInteractingComponent; \
	}; \
	struct VRButtonComponent_eventReceiveButtonStateChanged_Parms \
	{ \
		bool bCurButtonState; \
		AActor* LastInteractingActor; \
		UPrimitiveComponent* InteractingComponent; \
	};


#define vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_CALLBACK_WRAPPERS
#define vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUVRButtonComponent(); \
	friend struct Z_Construct_UClass_UVRButtonComponent_Statics; \
public: \
	DECLARE_CLASS(UVRButtonComponent, UStaticMeshComponent, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/VRExpansionPlugin"), NO_API) \
	DECLARE_SERIALIZER(UVRButtonComponent) \
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#define vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_INCLASS \
private: \
	static void StaticRegisterNativesUVRButtonComponent(); \
	friend struct Z_Construct_UClass_UVRButtonComponent_Statics; \
public: \
	DECLARE_CLASS(UVRButtonComponent, UStaticMeshComponent, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/VRExpansionPlugin"), NO_API) \
	DECLARE_SERIALIZER(UVRButtonComponent) \
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#define vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UVRButtonComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UVRButtonComponent) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UVRButtonComponent); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UVRButtonComponent); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UVRButtonComponent(UVRButtonComponent&&); \
	NO_API UVRButtonComponent(const UVRButtonComponent&); \
public:


#define vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UVRButtonComponent(UVRButtonComponent&&); \
	NO_API UVRButtonComponent(const UVRButtonComponent&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UVRButtonComponent); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UVRButtonComponent); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UVRButtonComponent)


#define vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_PRIVATE_PROPERTY_OFFSET
#define vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_46_PROLOG \
	vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_EVENT_PARMS


#define vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_PRIVATE_PROPERTY_OFFSET \
	vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_RPC_WRAPPERS \
	vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_CALLBACK_WRAPPERS \
	vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_INCLASS \
	vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_PRIVATE_PROPERTY_OFFSET \
	vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_RPC_WRAPPERS_NO_PURE_DECLS \
	vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_CALLBACK_WRAPPERS \
	vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_INCLASS_NO_PURE_DECLS \
	vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h_49_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> VREXPANSIONPLUGIN_API UClass* StaticClass<class UVRButtonComponent>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID vrexppluginexample_Plugins_VRExpansionPlugin_VRExpansionPlugin_Source_VRExpansionPlugin_Public_Interactibles_VRButtonComponent_h


#define FOREACH_ENUM_EVRSTATECHANGEAUTHORITYTYPE(op) \
	op(EVRStateChangeAuthorityType::CanChangeState_All) \
	op(EVRStateChangeAuthorityType::CanChangeState_Server) \
	op(EVRStateChangeAuthorityType::CanChangeState_Owner) 

enum class EVRStateChangeAuthorityType : uint8;
template<> VREXPANSIONPLUGIN_API UEnum* StaticEnum<EVRStateChangeAuthorityType>();

#define FOREACH_ENUM_EVRBUTTONTYPE(op) \
	op(EVRButtonType::Btn_Press) \
	op(EVRButtonType::Btn_Toggle_Return) \
	op(EVRButtonType::Btn_Toggle_Stay) 

enum class EVRButtonType : uint8;
template<> VREXPANSIONPLUGIN_API UEnum* StaticEnum<EVRButtonType>();

PRAGMA_ENABLE_DEPRECATION_WARNINGS
