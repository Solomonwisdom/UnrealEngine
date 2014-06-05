// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ProjectTargetPlatformEditorPrivatePCH.h"
#include "Settings.h"

#define LOCTEXT_NAMESPACE "FProjectTargetPlatformEditorModule"

/**
 * Implements the platform target platform editor module
 */
class FProjectTargetPlatformEditorModule : public IProjectTargetPlatformEditorModule
{
public:

	// Begin IProjectTargetPlatformEditorModule interface

	virtual TWeakPtr<SWidget> CreateProjectTargetPlatformEditorPanel() OVERRIDE
	{
		TSharedPtr<SWidget> Panel = SNew(SProjectTargetPlatformSettings);
		EditorPanels.Add(Panel);

		return Panel;
	}

	virtual void DestroyProjectTargetPlatformEditorPanel(const TWeakPtr<SWidget>& Panel) OVERRIDE
	{
		EditorPanels.Remove(Panel.Pin());
	}

	virtual void AddOpenProjectTargetPlatformEditorMenuItem(FMenuBuilder& MenuBuilder) const OVERRIDE
	{
		struct Local
		{
			static void OpenSettings( FName ContainerName, FName CategoryName, FName SectionName )
			{
				FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer(ContainerName, CategoryName, SectionName);
			}
		};

		MenuBuilder.AddMenuEntry(
			LOCTEXT("SupportedPlatformsMenuLabel", "Supported Platforms..."),
			LOCTEXT("SupportedPlatformsMenuToolTip", "Change which platforms this project supports"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateStatic(&Local::OpenSettings, FName("Project"), FName("Game"), FName("SupportedPlatforms")))
			);
	}

	virtual TSharedRef<SWidget> MakePlatformMenuItemWidget(const PlatformInfo::FPlatformInfo& PlatformInfo, const bool bForCheckBox = false, const FText& DisplayNameOverride = FText()) const OVERRIDE
	{
		struct Local
		{
			static EVisibility IsUnsupportedPlatformWarningVisible(const FName PlatformName)
			{
				FProjectStatus ProjectStatus;
				return (!IProjectManager::Get().QueryStatusForCurrentProject(ProjectStatus) || ProjectStatus.IsTargetPlatformSupported(PlatformName)) ? EVisibility::Hidden : EVisibility::Visible;
			}
		};

		return 
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(FMargin((bForCheckBox) ? 2 : 13, 0, 2, 0))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SOverlay)
				+SOverlay::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(SBox)
					.WidthOverride(MultiBoxConstants::MenuIconSize)
					.HeightOverride(MultiBoxConstants::MenuIconSize)
					[
						SNew(SImage)
						.Image(FEditorStyle::GetBrush(PlatformInfo.GetIconStyleName(PlatformInfo::EPlatformIconSize::Normal)))
					]
				]
				+SOverlay::Slot()
				.Padding(FMargin(MultiBoxConstants::MenuIconSize * 0.5f, 0, 0, 0))
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Bottom)
				[
					SNew(SBox)
					.WidthOverride(MultiBoxConstants::MenuIconSize * 0.9f)
					.HeightOverride(MultiBoxConstants::MenuIconSize * 0.9f)
					[
						SNew(SImage)
						.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateStatic(&Local::IsUnsupportedPlatformWarningVisible, PlatformInfo.VanillaPlatformName)))
						.Image(FEditorStyle::GetBrush("Launcher.Platform.Warning"))
					]
				]
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(FMargin((bForCheckBox) ? 2 : 7, 0, 6, 0))
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.TextStyle(FEditorStyle::Get(), "Menu.Label")
				.Text((DisplayNameOverride.IsEmpty()) ? PlatformInfo.DisplayName : DisplayNameOverride)
			];
	}

	virtual bool ShowUnsupportedTargetWarning(const FName PlatformName) const OVERRIDE
	{
		const PlatformInfo::FPlatformInfo* const PlatformInfo = PlatformInfo::FindPlatformInfo(PlatformName);
		check(PlatformInfo);

		// Don't show the warning during automation testing; the dlg is modal and blocks
		FProjectStatus ProjectStatus;
		if(!GIsAutomationTesting && IProjectManager::Get().QueryStatusForCurrentProject(ProjectStatus) && !ProjectStatus.IsTargetPlatformSupported(PlatformInfo->VanillaPlatformName))
		{
			FFormatNamedArguments Args;
			Args.Add(TEXT("DisplayName"), PlatformInfo->DisplayName);
			FText WarningText = FText::Format(LOCTEXT("ShowUnsupportedPlatformWarning_Message", "{DisplayName} is not listed as a supported platform for this project, so may not run as expected.\n\nDo you wish to continue?"), Args);

			FSuppressableWarningDialog::FSetupInfo Info(
				WarningText, 
				LOCTEXT("ShowUnsupportedPlatformWarning_Title", "Unsupported Platform"), 
				TEXT("SuppressUnsupportedPlatformWarningDialog")
				);
			Info.ConfirmText = LOCTEXT("ShowUnsupportedPlatformWarning_Confirm", "Continue");
			Info.CancelText = LOCTEXT("ShowUnsupportedPlatformWarning_Cancel", "Cancel");
			FSuppressableWarningDialog UnsupportedPlatformWarningDialog(Info);

			return UnsupportedPlatformWarningDialog.ShowModal() != FSuppressableWarningDialog::EResult::Cancel;
		}

		return true;
	}

	// End IProjectTargetPlatformEditorModule interface

private:

	// Holds the collection of created editor panels.
	TArray<TSharedPtr<SWidget> > EditorPanels;
};

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FProjectTargetPlatformEditorModule, ProjectTargetPlatformEditor);
