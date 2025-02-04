﻿// Copyright JAA Contributors 2024-2025

#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "../../Utilities/ObjectUtilities.h"
#include "../../Utilities/PropertyUtilities.h"
#include "Utilities/AppStyleCompatibility.h"
#include "Widgets/Notifications/SNotificationList.h"

extern TArray<FString> ImporterAcceptedTypes;

// Global handler for converting JSON to assets
class IImporter {
public:
    IImporter()
        : PropertySerializer(nullptr), GObjectSerializer(nullptr),
          Package(nullptr), OutermostPkg(nullptr) {}

    IImporter(const FString& FileName, const FString& FilePath, 
              const TSharedPtr<FJsonObject>& JsonObject, UPackage* Package, 
              UPackage* OutermostPkg, const TArray<TSharedPtr<FJsonValue>>& AllJsonObjects = {});

    virtual ~IImporter() {}

    // Import the data of the supported type, return if successful or not
    virtual bool ImportData() { return false; }

protected:
    UPropertySerializer* PropertySerializer;
    UObjectSerializer* GObjectSerializer;

public:
    static TArray<FString> GetAcceptedTypes() {
        return ImporterAcceptedTypes;
    }

    template <class T = UObject>
    void LoadObject(const TSharedPtr<FJsonObject>* PackageIndex, TObjectPtr<T>& Object);

    template <class T = UObject>
    TArray<TObjectPtr<T>> LoadObject(const TArray<TSharedPtr<FJsonValue>>& PackageArray, TArray<TObjectPtr<T>> Array);

    static bool CanImport(const FString& ImporterType) {
        return ImporterAcceptedTypes.Contains(ImporterType)
        || ImporterType.StartsWith("Sound") && ImporterType != "SoundWave" && !ImporterType.StartsWith("SoundNode"); 
    }

    static bool CanImportAny(TArray<FString>& Types) {
        for (FString& Type : Types) {
            if (CanImport(Type)) return true;
        }
        return false;
    }

    void ImportReference(const FString& File);
    bool ImportAssetReference(const FString& GamePath);
    bool ImportExports(TArray<TSharedPtr<FJsonValue>> Exports, FString File, bool bHideNotifications = false);

    TArray<TSharedPtr<FJsonValue>> GetObjectsWithTypeStartingWith(const FString& StartsWithStr) {
        TArray<TSharedPtr<FJsonValue>> FilteredObjects;

        for (const TSharedPtr<FJsonValue>& JsonObjectValue : AllJsonObjects) {
            if (JsonObjectValue->Type == EJson::Object) {
                TSharedPtr<FJsonObject> JsonObjectType = JsonObjectValue->AsObject();

                if (JsonObjectType.IsValid() && JsonObjectType->HasField("Type")) {
                    FString TypeValue = JsonObjectType->GetStringField("Type");

                    // Check if the "Type" field starts with the specified string
                    if (TypeValue.StartsWith(StartsWithStr)) {
                        FilteredObjects.Add(JsonObjectValue);
                    }
                }
            }
        }

        return FilteredObjects;
    }

    TSharedPtr<FJsonObject> GetExport(FJsonObject* PackageIndex);

    // Notification Functions
    virtual void AppendNotification(const FText& Text, const FText& SubText, float ExpireDuration, SNotificationItem::ECompletionState CompletionState, bool bUseSuccessFailIcons = false, float WidthOverride = 500);
    virtual void AppendNotification(const FText& Text, const FText& SubText, float ExpireDuration, const FSlateBrush* SlateBrush, SNotificationItem::ECompletionState CompletionState, bool bUseSuccessFailIcons = false, float WidthOverride = 500);

    TSharedPtr<FJsonObject> RemovePropertiesShared(TSharedPtr<FJsonObject> Input, TArray<FString> RemovedProperties) const;

protected:
    bool HandleAssetCreation(UObject* Asset) const;
    void SavePackage();

    // Handle edit changes, and add it to the content browser
    // Shortcut to calling SavePackage and HandleAssetCreation
    bool OnAssetCreation(UObject* Asset);

    template <class T = UObject>
    TObjectPtr<T> DownloadWrapper(TObjectPtr<T> InObject, FString Type, FString Name, FString Path);

    static FName GetExportNameOfSubobject(const FString& PackageIndex);
    TArray<TSharedPtr<FJsonValue>> FilterExportsByOuter(const FString& Outer);
    TSharedPtr<FJsonValue> GetExportByObjectPath(const TSharedPtr<FJsonObject>& Object);

    FORCEINLINE UObjectSerializer* GetObjectSerializer() const { return GObjectSerializer; }

    FString FileName;
    FString FilePath;
    TSharedPtr<FJsonObject> JsonObject;
    UPackage* Package;
    UPackage* OutermostPkg;

    TArray<TSharedPtr<FJsonValue>> AllJsonObjects;
};