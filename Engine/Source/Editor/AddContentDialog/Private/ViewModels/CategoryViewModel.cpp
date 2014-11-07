// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AddContentDialogPCH.h"

#define LOCTEXT_NAMESPACE "ContentSourceViewModel"

FCategoryViewModel::FCategoryViewModel()
{
	Category = EContentSourceCategory::Unknown;
	Initialize();
}

FCategoryViewModel::FCategoryViewModel(EContentSourceCategory InCategory)
{
	Category = InCategory;
	Initialize();
}

FText FCategoryViewModel::GetText() const
{
	return Text;
}

const FSlateBrush* FCategoryViewModel::GetIconBrush() const
{
	return IconBrush;
}

inline bool FCategoryViewModel::operator==(const FCategoryViewModel& Other) const
{
	return Other.Text.EqualTo(Text) && (Other.IconBrush == IconBrush);
}

uint32 FCategoryViewModel::GetTypeHash() const
{
	return (uint32)Category;
}

void FCategoryViewModel::Initialize()
{
	switch (Category)
	{
	case EContentSourceCategory::BlueprintFeature:
		Text = LOCTEXT("BlueprintFeature", "Blueprint Feature");
		IconBrush = FAddContentDialogStyle::Get().GetBrush("AddContentDialog.BlueprintFeatureCategory");
		break;
	case EContentSourceCategory::CodeFeature:
		Text = LOCTEXT("CodeFeature", "C++ Feature");
		IconBrush = FAddContentDialogStyle::Get().GetBrush("AddContentDialog.CodeFeatureCategory");
		break;
	default:
		Text = LOCTEXT("Miscellaneous", "Miscellaneous");
		IconBrush = FAddContentDialogStyle::Get().GetBrush("AddContentDialog.DefaultCategory");
		break;
	}
}