// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AnimGraphNode_SkeletalControlBase.h"
#include "Animation/BoneControllers/AnimNode_ModifyBone.h"
#include "EdGraph/EdGraphNodeUtils.h" // for FNodeTitleTextTable
#include "AnimGraphNode_ModifyBone.generated.h"

UCLASS(MinimalAPI)
class UAnimGraphNode_ModifyBone : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category=Settings)
	FAnimNode_ModifyBone Node;

public:
	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FString GetKeywords() const override;
	// End of UEdGraphNode interface

protected:
	// UAnimGraphNode_SkeletalControlBase interface
	virtual FText GetControllerDescription() const override;

	ANIMGRAPH_API virtual FVector GetWidgetLocation(const USkeletalMeshComponent* SkelComp, struct FAnimNode_SkeletalControlBase* AnimNode) override;
	ANIMGRAPH_API virtual FWidget::EWidgetMode GetWidgetMode(const USkeletalMeshComponent* SkelComp) override;
	ANIMGRAPH_API virtual FWidget::EWidgetMode ChangeWidgetMode(const USkeletalMeshComponent* SkelComp, FWidget::EWidgetMode InCurWidgetMode) override;

	ANIMGRAPH_API virtual FName FindSelectedBone() override;

	ANIMGRAPH_API virtual void UpdateDefaultValues(const FAnimNode_Base* AnimNode) override;
	ANIMGRAPH_API virtual void UpdateAllDefaultValues(const FAnimNode_Base* AnimNode) override;

	ANIMGRAPH_API virtual void DoTranslation(const USkeletalMeshComponent* SkelComp, FVector& Drag, FAnimNode_Base* InOutAnimNode) override;
	ANIMGRAPH_API virtual void DoRotation(const USkeletalMeshComponent* SkelComp, FRotator& Rotation, FAnimNode_Base* InOutAnimNode) override;
	ANIMGRAPH_API virtual void DoScale(const USkeletalMeshComponent* SkelComp, FVector& Drag, FAnimNode_Base* InOutAnimNode) override;
	ANIMGRAPH_API virtual void	CopyNodeDataTo(FAnimNode_Base* OutAnimNode) override;
	ANIMGRAPH_API virtual void	CopyNodeDataFrom(const FAnimNode_Base* InNewAnimNode) override;

	// End of UAnimGraphNode_SkeletalControlBase interface

	// methods to find a valid widget mode for gizmo because doesn't need to show gizmo when the mode is "Ignore"
	FWidget::EWidgetMode FindValidWidgetMode(FWidget::EWidgetMode InMode);
	EBoneModificationMode GetBoneModificationMode(FWidget::EWidgetMode InMode);
	FWidget::EWidgetMode GetNextWidgetMode(FWidget::EWidgetMode InMode);

private:
	/** Constructing FText strings can be costly, so we cache the node's title */
	FNodeTitleTextTable CachedNodeTitles;

	FWidget::EWidgetMode CurWidgetMode;
};

