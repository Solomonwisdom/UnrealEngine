// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

class FPakPlatformFile;

/** A content source which represents a content upack. */
class FFeaturePackContentSource : public IContentSource
{
public:
	FFeaturePackContentSource(FString InFeaturePackPath);

	virtual TArray<FLocalizedText> GetLocalizedNames() override;
	virtual TArray<FLocalizedText> GetLocalizedDescriptions() override;
	
	virtual EContentSourceCategory GetCategory() override;

	virtual TSharedPtr<FImageData> GetIconData() override;
	virtual TArray<TSharedPtr<FImageData>> GetScreenshotData() override;

	virtual void InstallToProject(FString InstallPath) override;

private:
	void LoadPakFileToBuffer(FPakPlatformFile& PakPlatformFile, FString Path, TArray<uint8>& Buffer);

private:
	FString FeaturePackPath;
	TArray<FLocalizedText> LocalizedNames;
	TArray<FLocalizedText> LocalizedDescriptions;
	EContentSourceCategory Category;
	TSharedPtr<FImageData> IconData;
	TArray<TSharedPtr<FImageData>> ScreenshotData;
};