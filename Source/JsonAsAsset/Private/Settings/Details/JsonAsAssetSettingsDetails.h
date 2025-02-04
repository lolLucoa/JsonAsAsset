// Copyright JAA Contributors 2024-2025

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class UJsonAsAssetSettings;

// Buttons in plugin settings
class FJsonAsAssetSettingsDetails : public IDetailCustomization
{
public:

	static TSharedRef<IDetailCustomization> MakeInstance();
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	static void SaveConfig(UJsonAsAssetSettings* Settings);
};