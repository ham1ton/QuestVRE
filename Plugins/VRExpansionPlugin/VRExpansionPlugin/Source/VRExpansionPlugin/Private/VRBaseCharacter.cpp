// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "VRBaseCharacter.h"
#include "NavigationSystem.h"
#include "VRPathFollowingComponent.h"
//#include "Runtime/Engine/Private/EnginePrivate.h"

DEFINE_LOG_CATEGORY(LogBaseVRCharacter);

FName AVRBaseCharacter::LeftMotionControllerComponentName(TEXT("Left Grip Motion Controller"));
FName AVRBaseCharacter::RightMotionControllerComponentName(TEXT("Right Grip Motion Controller"));
FName AVRBaseCharacter::ReplicatedCameraComponentName(TEXT("VR Replicated Camera"));
FName AVRBaseCharacter::ParentRelativeAttachmentComponentName(TEXT("Parent Relative Attachment"));
FName AVRBaseCharacter::SmoothingSceneParentComponentName(TEXT("NetSmoother"));

AVRBaseCharacter::AVRBaseCharacter(const FObjectInitializer& ObjectInitializer)
 : Super(ObjectInitializer.DoNotCreateDefaultSubobject(ACharacter::MeshComponentName).SetDefaultSubobjectClass<UVRBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))

{

	// Remove the movement jitter with slow speeds
	this->ReplicatedMovement.LocationQuantizationLevel = EVectorQuantization::RoundTwoDecimals;

	if (UCapsuleComponent * cap = GetCapsuleComponent())
	{
		cap->SetCapsuleSize(16.0f, 96.0f);
		cap->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		cap->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	}

	NetSmoother = CreateDefaultSubobject<USceneComponent>(AVRBaseCharacter::SmoothingSceneParentComponentName);
	if (NetSmoother)
	{
		NetSmoother->SetupAttachment(RootComponent);
	}

	VRReplicatedCamera = CreateDefaultSubobject<UReplicatedVRCameraComponent>(AVRBaseCharacter::ReplicatedCameraComponentName);
	if (VRReplicatedCamera)
	{
		VRReplicatedCamera->bOffsetByHMD = false;
		VRReplicatedCamera->SetupAttachment(NetSmoother);
		VRReplicatedCamera->OverrideSendTransform = &AVRBaseCharacter::Server_SendTransformCamera;
	}

	VRMovementReference = NULL;
	if (GetMovementComponent())
	{
		VRMovementReference = Cast<UVRBaseCharacterMovementComponent>(GetMovementComponent());
		//AddTickPrerequisiteComponent(this->GetCharacterMovement());
	}

	ParentRelativeAttachment = CreateDefaultSubobject<UParentRelativeAttachmentComponent>(AVRBaseCharacter::ParentRelativeAttachmentComponentName);
	if (ParentRelativeAttachment && VRReplicatedCamera)
	{
		// Moved this to be root relative as the camera late updates were killing how it worked
		ParentRelativeAttachment->SetupAttachment(NetSmoother);
		ParentRelativeAttachment->bOffsetByHMD = false;
	}

	LeftMotionController = CreateDefaultSubobject<UGripMotionControllerComponent>(AVRBaseCharacter::LeftMotionControllerComponentName);
	if (LeftMotionController)
	{
		LeftMotionController->SetupAttachment(NetSmoother);
		//LeftMotionController->MotionSource = FXRMotionControllerBase::LeftHandSourceId;
		LeftMotionController->SetTrackingMotionSource(FXRMotionControllerBase::LeftHandSourceId);
		//LeftMotionController->Hand = EControllerHand::Left;
		LeftMotionController->bOffsetByHMD = false;
		// Keep the controllers ticking after movement
		LeftMotionController->AddTickPrerequisiteComponent(GetCharacterMovement());
		LeftMotionController->OverrideSendTransform = &AVRBaseCharacter::Server_SendTransformLeftController;
	}

	RightMotionController = CreateDefaultSubobject<UGripMotionControllerComponent>(AVRBaseCharacter::RightMotionControllerComponentName);
	if (RightMotionController)
	{
		RightMotionController->SetupAttachment(NetSmoother);
		//RightMotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
		RightMotionController->SetTrackingMotionSource(FXRMotionControllerBase::RightHandSourceId);
		//RightMotionController->Hand = EControllerHand::Right;
		RightMotionController->bOffsetByHMD = false;
		// Keep the controllers ticking after movement
		RightMotionController->AddTickPrerequisiteComponent(GetCharacterMovement());
		RightMotionController->OverrideSendTransform = &AVRBaseCharacter::Server_SendTransformRightController;
	}

	OffsetComponentToWorld = FTransform(FQuat(0.0f, 0.0f, 0.0f, 1.0f), FVector::ZeroVector, FVector(1.0f));


	// Setting a minimum of every frame for replication consideration (UT uses this value for characters and projectiles).
	// Otherwise we will get some massive slow downs if the replication is allowed to hit the 2 per second minimum default
	MinNetUpdateFrequency = 100.0f;

	// This is for smooth turning, we have more of a use for this than FPS characters do
	// Due to roll/pitch almost never being off 0 for VR the cost is just one byte so i'm fine defaulting it here
	// End users can reset to byte components if they ever want too.
	ReplicatedMovement.RotationQuantizationLevel = ERotatorQuantization::ShortComponents;

	VRReplicateCapsuleHeight = false;

	bUseExperimentalUnseatModeFix = true;
}

void AVRBaseCharacter::OnRep_PlayerState()
{
	OnPlayerStateReplicated_Bind.Broadcast(GetPlayerState());
	Super::OnRep_PlayerState();
}

void AVRBaseCharacter::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AVRBaseCharacter, SeatInformation, COND_None);
	DOREPLIFETIME_CONDITION(AVRBaseCharacter, VRReplicateCapsuleHeight, COND_None);
	DOREPLIFETIME_CONDITION(AVRBaseCharacter, ReplicatedCapsuleHeight, COND_SimulatedOnly);
}

void AVRBaseCharacter::PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	DOREPLIFETIME_ACTIVE_OVERRIDE(AVRBaseCharacter, ReplicatedCapsuleHeight, VRReplicateCapsuleHeight);
}

USkeletalMeshComponent* AVRBaseCharacter::GetIKMesh_Implementation() const
{
	return nullptr;
}

bool AVRBaseCharacter::Server_SetSeatedMode_Validate(USceneComponent * SeatParent, bool bSetSeatedMode, FTransform_NetQuantize TargetTransform, FTransform_NetQuantize InitialRelCameraTransform, float AllowedRadius, float AllowedRadiusThreshold, bool bZeroToHead, EVRConjoinedMovementModes PostSeatedMovementMode)
{
	return true;
}

void AVRBaseCharacter::Server_SetSeatedMode_Implementation(USceneComponent * SeatParent, bool bSetSeatedMode, FTransform_NetQuantize TargetTransform, FTransform_NetQuantize InitialRelCameraTransform, float AllowedRadius, float AllowedRadiusThreshold, bool bZeroToHead, EVRConjoinedMovementModes PostSeatedMovementMode)
{
	SetSeatedMode(SeatParent, bSetSeatedMode, TargetTransform, InitialRelCameraTransform, AllowedRadius, AllowedRadiusThreshold, bZeroToHead, PostSeatedMovementMode);
}

void AVRBaseCharacter::Server_ReZeroSeating_Implementation(FTransform_NetQuantize NewTargetTransform, FTransform_NetQuantize NewInitialRelCameraTransform, bool bZeroToHead)
{
	SeatInformation.StoredTargetTransform = NewTargetTransform;
	SeatInformation.InitialRelCameraTransform = NewInitialRelCameraTransform;

	// Purify the yaw of the initial rotation
	SeatInformation.InitialRelCameraTransform.SetRotation(UVRExpansionFunctionLibrary::GetHMDPureYaw_I(NewInitialRelCameraTransform.Rotator()).Quaternion());

	// #TODO: Need to handle non 1 scaled values here eventually
	if (bZeroToHead)
	{
		FVector newLocation = SeatInformation.InitialRelCameraTransform.GetTranslation();
		SeatInformation.StoredTargetTransform.AddToTranslation(FVector(0, 0, -newLocation.Z));
	}

	OnRep_SeatedCharInfo();
}

bool AVRBaseCharacter::Server_ReZeroSeating_Validate(FTransform_NetQuantize NewTargetTransform, FTransform_NetQuantize NewInitialRelCameraTransform, bool bZeroToHead)
{
	return true;
}

void AVRBaseCharacter::OnCustomMoveActionPerformed_Implementation(EVRMoveAction MoveActionType, FVector MoveActionVector, FRotator MoveActionRotator)
{

}

void AVRBaseCharacter::OnBeginWallPushback_Implementation(FHitResult HitResultOfImpact, bool bHadLocomotionInput, FVector HmdInput)
{

}

void AVRBaseCharacter::OnEndWallPushback_Implementation()
{

}

void AVRBaseCharacter::OnClimbingSteppedUp_Implementation()
{

}

void AVRBaseCharacter::Server_SendTransformCamera_Implementation(FBPVRComponentPosRep NewTransform)
{
	if(VRReplicatedCamera)
		VRReplicatedCamera->Server_SendCameraTransform_Implementation(NewTransform);
}

bool AVRBaseCharacter::Server_SendTransformCamera_Validate(FBPVRComponentPosRep NewTransform)
{
	return true;
	// Optionally check to make sure that player is inside of their bounds and deny it if they aren't?
}

void AVRBaseCharacter::Server_SendTransformLeftController_Implementation(FBPVRComponentPosRep NewTransform)
{
	if (LeftMotionController)
		LeftMotionController->Server_SendControllerTransform_Implementation(NewTransform);
}

bool AVRBaseCharacter::Server_SendTransformLeftController_Validate(FBPVRComponentPosRep NewTransform)
{
	return true;
	// Optionally check to make sure that player is inside of their bounds and deny it if they aren't?
}

void AVRBaseCharacter::Server_SendTransformRightController_Implementation(FBPVRComponentPosRep NewTransform)
{
	if(RightMotionController)
		RightMotionController->Server_SendControllerTransform_Implementation(NewTransform);
}

bool AVRBaseCharacter::Server_SendTransformRightController_Validate(FBPVRComponentPosRep NewTransform)
{
	return true;
	// Optionally check to make sure that player is inside of their bounds and deny it if they aren't?
}
FVector AVRBaseCharacter::GetTeleportLocation(FVector OriginalLocation)
{	
	return OriginalLocation;
}


void AVRBaseCharacter::NotifyOfTeleport_Implementation()
{
	if (!IsLocallyControlled())
	{
		if (LeftMotionController)
			LeftMotionController->bIsPostTeleport = true;

		if (RightMotionController)
			RightMotionController->bIsPostTeleport = true;
	}
}

void AVRBaseCharacter::OnRep_SeatedCharInfo()
{
	// Handle setting up the player here

	if (UPrimitiveComponent * root = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		if (SeatInformation.bSitting /*&& !SeatInformation.bWasSeated*/) // Removing WasSeated check because we may be switching seats
		{
			if (SeatInformation.bWasSeated)
			{
				if (SeatInformation.SeatParent != this->GetRootComponent()->GetAttachParent())
				{
					InitSeatedModeTransition();
				}
				else // Is just a reposition
				{
					//if (this->Role != ROLE_SimulatedProxy)
					ZeroToSeatInformation();
				}

			}
			else
			{
				if (this->Role == ROLE_SimulatedProxy)
				{
					/*if (UVRBaseCharacterMovementComponent * charMovement = Cast<UVRBaseCharacterMovementComponent>(GetMovementComponent()))
					{
						charMovement->SetMovementMode(MOVE_Custom, (uint8)EVRCustomMovementMode::VRMOVE_Seated);
					}*/
				}
				else
				{
					if (UVRBaseCharacterMovementComponent * charMovement = Cast<UVRBaseCharacterMovementComponent>(GetMovementComponent()))
					{
						charMovement->SetMovementMode(MOVE_Custom, (uint8)EVRCustomMovementMode::VRMOVE_Seated);
					}
				}
			}
		}
		else if (!SeatInformation.bSitting && SeatInformation.bWasSeated)
		{
			if (this->Role == ROLE_SimulatedProxy)
			{

				/*if (UVRBaseCharacterMovementComponent * charMovement = Cast<UVRBaseCharacterMovementComponent>(GetMovementComponent()))
				{
					charMovement->ApplyReplicatedMovementMode(SeatInformation.PostSeatedMovementMode);
					//charMovement->SetComponentTickEnabled(true);
				}*/

			}
			else
			{
				if (UVRBaseCharacterMovementComponent * charMovement = Cast<UVRBaseCharacterMovementComponent>(GetMovementComponent()))
				{
					charMovement->ApplyReplicatedMovementMode(SeatInformation.PostSeatedMovementMode);
				}
			}
		}
	}
}

void AVRBaseCharacter::InitSeatedModeTransition()
{
	if (UPrimitiveComponent * root = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		if (SeatInformation.bSitting /*&& !SeatInformation.bWasSeated*/) // Removing WasSeated check because we may be switching seats
		{

			if (SeatInformation.SeatParent /*&& !root->IsAttachedTo(SeatInformation.SeatParent)*/)
			{
				FAttachmentTransformRules TransformRule = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
				TransformRule.bWeldSimulatedBodies = true;
				AttachToComponent(SeatInformation.SeatParent, TransformRule);
			}

			if (this->Role == ROLE_SimulatedProxy)
			{
				if (UVRBaseCharacterMovementComponent * charMovement = Cast<UVRBaseCharacterMovementComponent>(GetMovementComponent()))
				{
					//charMovement->DisableMovement();
					//charMovement->SetComponentTickEnabled(false);
					//charMovement->SetMovementMode(MOVE_Custom, (uint8)EVRCustomMovementMode::VRMOVE_Seated);
				}

				root->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				// Set it before it is set below
				if (!SeatInformation.bWasSeated)
					SeatInformation.bOriginalControlRotation = bUseControllerRotationYaw;

				SeatInformation.bWasSeated = true;
				bUseControllerRotationYaw = false; // This forces rotation in world space, something that we don't want
				ZeroToSeatInformation();
				OnSeatedModeChanged(SeatInformation.bSitting, SeatInformation.bWasSeated);
			}
			else
			{
				if (UVRBaseCharacterMovementComponent * charMovement = Cast<UVRBaseCharacterMovementComponent>(GetMovementComponent()))
				{
					//charMovement->DisableMovement();
					//charMovement->SetComponentTickEnabled(false);
					//charMovement->SetMovementMode(MOVE_Custom, (uint8)EVRCustomMovementMode::VRMOVE_Seated);
					//charMovement->bIgnoreClientMovementErrorChecksAndCorrection = true;

					if (this->Role == ROLE_AutonomousProxy)
					{
						FNetworkPredictionData_Client_Character* ClientData = charMovement->GetPredictionData_Client_Character();
						check(ClientData);

						if (ClientData->SavedMoves.Num())
						{
							// Ack our most recent move, we don't want to start sending old moves after un seating.
							ClientData->AckMove(ClientData->SavedMoves.Num() - 1, *charMovement);
						}
					}

				}

				root->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				// Set it before it is set below
				if (!SeatInformation.bWasSeated)
					SeatInformation.bOriginalControlRotation = bUseControllerRotationYaw;

				SeatInformation.bWasSeated = true;
				bUseControllerRotationYaw = false; // This forces rotation in world space, something that we don't want

				ZeroToSeatInformation();
				OnSeatedModeChanged(SeatInformation.bSitting, SeatInformation.bWasSeated);
			}
		}
		else if (!SeatInformation.bSitting && SeatInformation.bWasSeated)
		{
			DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

			if (this->Role == ROLE_SimulatedProxy)
			{
				root->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);


				bUseControllerRotationYaw = SeatInformation.bOriginalControlRotation;

				SetActorLocationAndRotationVR(SeatInformation.StoredTargetTransform.GetTranslation(), SeatInformation.StoredTargetTransform.Rotator(), true, true, true);
				LeftMotionController->PostTeleportMoveGrippedObjects();
				RightMotionController->PostTeleportMoveGrippedObjects();

				/*if (UVRBaseCharacterMovementComponent * charMovement = Cast<UVRBaseCharacterMovementComponent>(GetMovementComponent()))
				{
					charMovement->ApplyReplicatedMovementMode(SeatInformation.PostSeatedMovementMode);
					//charMovement->SetComponentTickEnabled(true);
				}*/

				OnSeatedModeChanged(SeatInformation.bSitting, SeatInformation.bWasSeated);
			}
			else
			{
				if (UVRBaseCharacterMovementComponent * charMovement = Cast<UVRBaseCharacterMovementComponent>(GetMovementComponent()))
				{
					//charMovement->ApplyReplicatedMovementMode(SeatInformation.PostSeatedMovementMode);
					//charMovement->bIgnoreClientMovementErrorChecksAndCorrection = false;
					//charMovement->SetComponentTickEnabled(true);

					if (this->Role == ROLE_Authority)
					{
						if (bUseExperimentalUnseatModeFix)
						{
							FNetworkPredictionData_Server_Character * ServerData = charMovement->GetPredictionData_Server_Character();
							check(ServerData);
							ServerData->CurrentClientTimeStamp = 0.f;
							ServerData->ServerAccumulatedClientTimeStamp = 0.0f;
							ServerData->LastUpdateTime = 0.f;
							ServerData->ServerTimeStampLastServerMove = 0.f;
							ServerData->bForceClientUpdate = false;
							ServerData->TimeDiscrepancy = 0.f;
							ServerData->bResolvingTimeDiscrepancy = false;
							ServerData->TimeDiscrepancyResolutionMoveDeltaOverride = 0.f;
							ServerData->TimeDiscrepancyAccumulatedClientDeltasSinceLastServerTick = 0.f;
						}
						//charMovement->ForceReplicationUpdate();
						//FNetworkPredictionData_Server_Character * ServerData = charMovement->GetPredictionData_Server_Character();
						//check(ServerData);

						// Reset client timestamp check so that there isn't a delay on ending seated mode before we accept movement packets
						//ServerData->CurrentClientTimeStamp = 1.f;
					}
				}

				bUseControllerRotationYaw = SeatInformation.bOriginalControlRotation;

				// Re-purposing them for the new location and rotations
				SetActorLocationAndRotationVR(SeatInformation.StoredTargetTransform.GetTranslation(), SeatInformation.StoredTargetTransform.Rotator(), true, true, true);
				LeftMotionController->PostTeleportMoveGrippedObjects();
				RightMotionController->PostTeleportMoveGrippedObjects();

				// Enable collision now
				root->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

				OnSeatedModeChanged(SeatInformation.bSitting, SeatInformation.bWasSeated);
				SeatInformation.ClearTempVals();
			}
		}
	}
}

void AVRBaseCharacter::TickSeatInformation(float DeltaTime)
{
	float LastThresholdScaler = SeatInformation.CurrentThresholdScaler;
	bool bLastOverThreshold = SeatInformation.bIsOverThreshold;

	FVector NewLoc = VRReplicatedCamera->RelativeLocation;
	FVector OrigLocation = SeatInformation.InitialRelCameraTransform.GetTranslation();

	if (!SeatInformation.bZeroToHead)
	{
		NewLoc.Z = 0.0f;
		OrigLocation.Z = 0.0f;
	}

	float AbsDistance = FMath::Abs(FVector::Dist(OrigLocation, NewLoc));

	//FTransform newTrans = SeatInformation.StoredTargetTransform * SeatInformation.SeatParent->GetComponentTransform();

	// If over the allowed distance
	if (AbsDistance > SeatInformation.AllowedRadius)
	{
		// Force them back into range
		FVector diff = NewLoc - OrigLocation;
		diff.Normalize();
		diff = (-diff * (AbsDistance - SeatInformation.AllowedRadius));

		SetSeatRelativeLocationAndRotationVR(diff);
		SeatInformation.bWasOverLimit = true;
	}
	else if (SeatInformation.bWasOverLimit) // Make sure we are in the zero point otherwise
	{
		SetSeatRelativeLocationAndRotationVR(FVector::ZeroVector);
		SeatInformation.bWasOverLimit = false;
	}

	if (AbsDistance > SeatInformation.AllowedRadius - SeatInformation.AllowedRadiusThreshold)
		SeatInformation.bIsOverThreshold = true;
	else
		SeatInformation.bIsOverThreshold = false;

	SeatInformation.CurrentThresholdScaler = FMath::Clamp((AbsDistance - (SeatInformation.AllowedRadius - SeatInformation.AllowedRadiusThreshold)) / SeatInformation.AllowedRadiusThreshold, 0.0f, 1.0f);

	if (bLastOverThreshold != SeatInformation.bIsOverThreshold || !FMath::IsNearlyEqual(LastThresholdScaler, SeatInformation.CurrentThresholdScaler))
	{
		OnSeatThreshholdChanged(!SeatInformation.bIsOverThreshold, SeatInformation.CurrentThresholdScaler);
		OnSeatThreshholdChanged_Bind.Broadcast(!SeatInformation.bIsOverThreshold, SeatInformation.CurrentThresholdScaler);
	}
}

bool AVRBaseCharacter::SetSeatedMode(USceneComponent * SeatParent, bool bSetSeatedMode, FTransform TargetTransform, FTransform InitialRelCameraTransform, float AllowedRadius, float AllowedRadiusThreshold, bool bZeroToHead, EVRConjoinedMovementModes PostSeatedMovementMode)
{
	if (!this->HasAuthority() || !SeatParent)
		return false;

	if (bSetSeatedMode)
	{
		SeatInformation.SeatParent = SeatParent;
		SeatInformation.bSitting = true;
		SeatInformation.bZeroToHead = bZeroToHead;
		SeatInformation.StoredTargetTransform = TargetTransform;
		SeatInformation.InitialRelCameraTransform = InitialRelCameraTransform;

		// Purify the yaw of the initial rotation
		SeatInformation.InitialRelCameraTransform.SetRotation(UVRExpansionFunctionLibrary::GetHMDPureYaw_I(InitialRelCameraTransform.Rotator()).Quaternion());
		SeatInformation.AllowedRadius = AllowedRadius;
		SeatInformation.AllowedRadiusThreshold = AllowedRadiusThreshold;

		// #TODO: Need to handle non 1 scaled values here eventually
		if (bZeroToHead)
		{
			FVector newLocation = SeatInformation.InitialRelCameraTransform.GetTranslation();
			SeatInformation.StoredTargetTransform.AddToTranslation(FVector(0, 0, -newLocation.Z));
		}

		//SetReplicateMovement(false);/ / No longer doing this, allowing it to replicate down to simulated clients now instead
	}
	else
	{
		SeatInformation.SeatParent = nullptr;
		SeatInformation.StoredTargetTransform = TargetTransform;
		SeatInformation.PostSeatedMovementMode = PostSeatedMovementMode;
		//SetReplicateMovement(true); // No longer doing this, allowing it to replicate down to simulated clients now instead
		SeatInformation.bSitting = false;
	}

	OnRep_SeatedCharInfo(); // Call this on server side because it won't call itself
	NotifyOfTeleport(); // Teleport the controllers

	return true;
}

void AVRBaseCharacter::SetSeatRelativeLocationAndRotationVR(FVector DeltaLoc)
{
	/*if (bUseYawOnly)
	{
		NewRot.Pitch = 0.0f;
		NewRot.Roll = 0.0f;
	}

	NewLoc = NewLoc + Pivot;
	NewLoc -= NewRot.RotateVector(Pivot);

	SetActorRelativeTransform(FTransform(NewRot, NewLoc, GetCapsuleComponent()->RelativeScale3D));*/

	FTransform NewTrans = SeatInformation.StoredTargetTransform;// *SeatInformation.SeatParent->GetComponentTransform();

	FVector NewLocation;
	FRotator NewRotation;
	FVector PivotPoint = SeatInformation.InitialRelCameraTransform.GetTranslation();
	PivotPoint.Z = 0.0f;

	NewRotation = SeatInformation.InitialRelCameraTransform.Rotator();
	NewRotation = (NewRotation.Quaternion().Inverse() * NewTrans.GetRotation()).Rotator();
	NewLocation = NewTrans.GetTranslation();
	NewLocation -= NewRotation.RotateVector(PivotPoint + (-DeltaLoc));	

	// Also setting actor rot because the control rot transfers to it anyway eventually
	SetActorRelativeTransform(FTransform(NewRotation, NewLocation, GetCapsuleComponent()->RelativeScale3D));
}


FVector AVRBaseCharacter::AddActorWorldRotationVR(FRotator DeltaRot, bool bUseYawOnly)
{
	AController* OwningController = GetController();

	FVector NewLocation;
	FRotator NewRotation;
	FVector OrigLocation = GetActorLocation();
	FVector PivotPoint = GetActorTransform().InverseTransformPosition(GetVRLocation_Inline());
	PivotPoint.Z = 0.0f;

	NewRotation = bUseControllerRotationYaw && OwningController ? OwningController->GetControlRotation() : GetActorRotation();

	if (bUseYawOnly)
	{
		NewRotation.Pitch = 0.0f;
		NewRotation.Roll = 0.0f;
	}

	NewLocation = OrigLocation + NewRotation.RotateVector(PivotPoint);
	NewRotation = (NewRotation.Quaternion() * DeltaRot.Quaternion()).Rotator();
	NewLocation -= NewRotation.RotateVector(PivotPoint);

	if (bUseControllerRotationYaw && OwningController /*&& IsLocallyControlled()*/)
		OwningController->SetControlRotation(NewRotation);

	// Also setting actor rot because the control rot transfers to it anyway eventually
	SetActorLocationAndRotation(NewLocation, NewRotation);
	return NewLocation - OrigLocation;
}

FVector AVRBaseCharacter::SetActorRotationVR(FRotator NewRot, bool bUseYawOnly, bool bAccountForHMDRotation)
{
	AController* OwningController = GetController();

	FVector NewLocation;
	FRotator NewRotation;
	FVector OrigLocation = GetActorLocation();
	FVector PivotPoint = GetActorTransform().InverseTransformPosition(GetVRLocation_Inline());
	PivotPoint.Z = 0.0f;

	FRotator OrigRotation = bUseControllerRotationYaw && OwningController ? OwningController->GetControlRotation() : GetActorRotation();

	if (bUseYawOnly)
	{
		NewRot.Pitch = 0.0f;
		NewRot.Roll = 0.0f;
	}

	if (bAccountForHMDRotation)
	{
		NewRotation = UVRExpansionFunctionLibrary::GetHMDPureYaw_I(VRReplicatedCamera->RelativeRotation);
		NewRotation = (NewRot.Quaternion() * NewRotation.Quaternion().Inverse()).Rotator();
	}
	else
		NewRotation = NewRot;

	NewLocation = OrigLocation + OrigRotation.RotateVector(PivotPoint);
	//NewRotation = NewRot;
	NewLocation -= NewRotation.RotateVector(PivotPoint);

	if (bUseControllerRotationYaw && OwningController /*&& IsLocallyControlled()*/)
		OwningController->SetControlRotation(NewRotation);

	// Also setting actor rot because the control rot transfers to it anyway eventually
	SetActorLocationAndRotation(NewLocation, NewRotation);
	return NewLocation - OrigLocation;
}

FVector AVRBaseCharacter::SetActorLocationAndRotationVR(FVector NewLoc, FRotator NewRot, bool bUseYawOnly, bool bAccountForHMDRotation, bool bTeleport)
{
	AController* OwningController = GetController();

	FVector NewLocation;
	FRotator NewRotation;
	FVector PivotPoint = GetActorTransform().InverseTransformPosition(GetVRLocation_Inline());
	PivotPoint.Z = 0.0f;

	if (bUseYawOnly)
	{
		NewRot.Pitch = 0.0f;
		NewRot.Roll = 0.0f;
	}

	if (bAccountForHMDRotation)
	{
		NewRotation = UVRExpansionFunctionLibrary::GetHMDPureYaw_I(VRReplicatedCamera->RelativeRotation);//bUseControllerRotationYaw && OwningController ? OwningController->GetControlRotation() : GetActorRotation();
		NewRotation = (NewRotation.Quaternion().Inverse() * NewRot.Quaternion()).Rotator();
	}
	else
		NewRotation = NewRot;

	NewLocation = NewLoc;// +PivotPoint;// NewRotation.RotateVector(PivotPoint);
						 //NewRotation = NewRot;
	NewLocation -= NewRotation.RotateVector(PivotPoint);

	if (bUseControllerRotationYaw && OwningController /*&& IsLocallyControlled()*/)
		OwningController->SetControlRotation(NewRotation);

	// Also setting actor rot because the control rot transfers to it anyway eventually
	SetActorLocationAndRotation(NewLocation, NewRotation, false, nullptr, bTeleport ? ETeleportType::TeleportPhysics : ETeleportType::None);
	return NewLocation - NewLoc;
}

void AVRBaseCharacter::SetCharacterSizeVR(float NewRadius, float NewHalfHeight, bool bUpdateOverlaps)
{
	if (UCapsuleComponent * Capsule = Cast<UCapsuleComponent>(this->RootComponent))
	{
		if (!FMath::IsNearlyEqual(NewRadius, Capsule->GetUnscaledCapsuleRadius()) || !FMath::IsNearlyEqual(NewHalfHeight, Capsule->GetUnscaledCapsuleHalfHeight()))
			Capsule->SetCapsuleSize(NewRadius, NewHalfHeight, bUpdateOverlaps);

		if (GetNetMode() < ENetMode::NM_Client && VRReplicateCapsuleHeight)
			ReplicatedCapsuleHeight.CapsuleHeight = Capsule->GetUnscaledCapsuleHalfHeight();
	}
}

void AVRBaseCharacter::SetCharacterHalfHeightVR(float HalfHeight, bool bUpdateOverlaps)
{
	if (UCapsuleComponent * Capsule = Cast<UCapsuleComponent>(this->RootComponent))
	{
		if (!FMath::IsNearlyEqual(HalfHeight, Capsule->GetUnscaledCapsuleHalfHeight()))
			Capsule->SetCapsuleHalfHeight(HalfHeight, bUpdateOverlaps);

		if (GetNetMode() < ENetMode::NM_Client && VRReplicateCapsuleHeight)
			ReplicatedCapsuleHeight.CapsuleHeight = Capsule->GetUnscaledCapsuleHalfHeight();
	}
}

void AVRBaseCharacter::ExtendedSimpleMoveToLocation(const FVector& GoalLocation, float AcceptanceRadius, bool bStopOnOverlap, bool bUsePathfinding, bool bProjectDestinationToNavigation, bool bCanStrafe, TSubclassOf<UNavigationQueryFilter> FilterClass, bool bAllowPartialPaths)
{
	UNavigationSystemV1* NavSys = Controller ? FNavigationSystem::GetCurrent<UNavigationSystemV1>(Controller->GetWorld()) : nullptr;
	if (NavSys == nullptr || Controller == nullptr )
	{
		UE_LOG(LogBaseVRCharacter, Warning, TEXT("UVRSimpleCharacter::ExtendedSimpleMoveToLocation called for NavSys:%s Controller:%s (if any of these is None then there's your problem"),
			*GetNameSafe(NavSys), *GetNameSafe(Controller));
		return;
	}

	UPathFollowingComponent* PFollowComp = nullptr;
	//Controller->InitNavigationControl(PFollowComp);
	if (Controller)
	{
		// New for 4.20, spawning the missing path following component here if there isn't already one
		PFollowComp = Controller->FindComponentByClass<UPathFollowingComponent>();
		if (PFollowComp == nullptr)
		{
			PFollowComp = NewObject<UVRPathFollowingComponent>(Controller);
			PFollowComp->RegisterComponentWithWorld(Controller->GetWorld());
			PFollowComp->Initialize();
		}
	}

	if (PFollowComp == nullptr)
	{
		UE_LOG(LogBaseVRCharacter, Warning, TEXT("ExtendedSimpleMoveToLocation - No PathFollowingComponent Found"));
		return;
	}

	if (!PFollowComp->IsPathFollowingAllowed())
	{
		UE_LOG(LogBaseVRCharacter, Warning, TEXT("ExtendedSimpleMoveToLocation - Path Following Movement Is Not Set To Allowed"));
		return;
	}

	EPathFollowingReachMode ReachMode;
	if (bStopOnOverlap)
		ReachMode = EPathFollowingReachMode::OverlapAgent;
	else
		ReachMode = EPathFollowingReachMode::ExactLocation;

	bool bAlreadyAtGoal = false;

	if(UVRPathFollowingComponent * pathcomp = Cast<UVRPathFollowingComponent>(PFollowComp))
		bAlreadyAtGoal = pathcomp->HasReached(GoalLocation, /*EPathFollowingReachMode::OverlapAgent*/ReachMode);
	else
		bAlreadyAtGoal = PFollowComp->HasReached(GoalLocation, /*EPathFollowingReachMode::OverlapAgent*/ReachMode);

	// script source, keep only one move request at time
	if (PFollowComp->GetStatus() != EPathFollowingStatus::Idle)
	{
		if (GetNetMode() == ENetMode::NM_Client)
		{
			// Stop the movement here, not keeping the velocity because it bugs out for clients, might be able to fix.
			PFollowComp->AbortMove(*NavSys, FPathFollowingResultFlags::ForcedScript | FPathFollowingResultFlags::NewRequest
				, FAIRequestID::AnyRequest, /*bAlreadyAtGoal ? */EPathFollowingVelocityMode::Reset /*: EPathFollowingVelocityMode::Keep*/);
		}
		else
		{
			PFollowComp->AbortMove(*NavSys, FPathFollowingResultFlags::ForcedScript | FPathFollowingResultFlags::NewRequest
				, FAIRequestID::AnyRequest, bAlreadyAtGoal ? EPathFollowingVelocityMode::Reset : EPathFollowingVelocityMode::Keep);
		}
	}

	if (bAlreadyAtGoal)
	{
		PFollowComp->RequestMoveWithImmediateFinish(EPathFollowingResult::Success);
	}
	else
	{
		const ANavigationData* NavData = NavSys->GetNavDataForProps(Controller->GetNavAgentPropertiesRef());
		if (NavData)
		{
			FPathFindingQuery Query(Controller, *NavData, Controller->GetNavAgentLocation(), GoalLocation);
			FPathFindingResult Result = NavSys->FindPathSync(Query);
			if (Result.IsSuccessful())
			{
				FAIMoveRequest MoveReq(GoalLocation);
				MoveReq.SetUsePathfinding(bUsePathfinding);
				MoveReq.SetAllowPartialPath(bAllowPartialPaths);
				MoveReq.SetProjectGoalLocation(bProjectDestinationToNavigation);
				MoveReq.SetNavigationFilter(*FilterClass ? FilterClass : DefaultNavigationFilterClass);
				MoveReq.SetAcceptanceRadius(AcceptanceRadius);
				MoveReq.SetReachTestIncludesAgentRadius(bStopOnOverlap);
				MoveReq.SetCanStrafe(bCanStrafe);
				MoveReq.SetReachTestIncludesGoalRadius(true);

				PFollowComp->RequestMove(/*FAIMoveRequest(GoalLocation)*/MoveReq, Result.Path);
			}
			else if (PFollowComp->GetStatus() != EPathFollowingStatus::Idle)
			{
				PFollowComp->RequestMoveWithImmediateFinish(EPathFollowingResult::Invalid);
			}
		}
	}
}