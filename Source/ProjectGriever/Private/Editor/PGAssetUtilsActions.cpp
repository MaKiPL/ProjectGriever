// All rights reserved. Marcin 'Maki' Gomulak. marcingomulak@gmail.com


#include "Editor/PGAssetUtilsActions.h"

#include "AssetSelection.h"
#include "EditorUtilityLibrary.h"

void UPGAssetUtilsActions::AddAssetPrefixes()
{
	TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();

	for (UObject* Asset : SelectedAssets)
	{
		FString ObjectName = Asset->GetName();
		const UClass* ObjectClass = Asset->GetClass();
		UE_LOG(LogTemp, Log, TEXT("Asset: %s, Class: %s"), *ObjectName, *ObjectClass->GetName());

		const FString * FoundPrefix = PrefixesMap.Find(ObjectClass);
		if (!FoundPrefix)
		{
			UE_LOG(LogTemp, Log, TEXT("No prefix found for class: %s"), *ObjectClass->GetName());
			continue;
		}
		if(!ObjectName.StartsWith(*FoundPrefix))
		{
			FString NewObjectName = FString::Printf(TEXT("%s_%s"), **FoundPrefix, *ObjectName);
			UEditorUtilityLibrary::RenameAsset(Asset, NewObjectName);
		}
	}
}

void UPGAssetUtilsActions::UpdateTextureInformation()
{
	TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssetsOfClass(UTexture2D::StaticClass());
	for (UObject* Asset : SelectedAssets)
	{
		UTexture2D* Texture = Cast<UTexture2D>(Asset);
        if (!Texture)
	        continue;
		Texture->MipGenSettings = TMGS_FromTextureGroup;
		Texture->Filter = TF_Nearest;
		Texture->UpdateResource();
		Texture->PostEditChange();
	}
}
