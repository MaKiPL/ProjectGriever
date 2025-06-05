// All rights reserved. Marcin 'Maki' Gomulak. marcingomulak@gmail.com


#include "PGImporterTexl.h"
#include "PGTim.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"

DEFINE_LOG_CATEGORY(LogPGImporterTexl);

UPGImporterTexl::UPGImporterTexl()
{
	SupportedClass = UTexture2D::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
	Formats.Add(TEXT("obj; FF8 WM/TEXL"));
	bText = false;
}

bool UPGImporterTexl::FactoryCanImport(const FString& Filename)
{
	const bool bPathCorrect = FPaths::GetExtension(Filename).ToLower() == TEXT("texl");
	return bPathCorrect;
}

UObject* UPGImporterTexl::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	TArray<uint8> Buffer;
	if (!FFileHelper::LoadFileToArray(Buffer, *Filename))
	{
		UE_LOG(LogPGImporterTexl, Error, TEXT("Failed to load file %s"), *Filename);
        return nullptr;
	}

	if (Buffer.NumBytes() != (Tex_Count*Tex_Size))
	{
		UE_LOG(LogPGImporterTexl, Error, TEXT("Invalid file size for %s. Got: %llu"), *Filename, Buffer.NumBytes());
		return nullptr;
	}

	FString PackagePath = FPackageName::GetLongPackagePath(InParent->GetPathName());
	FString BaseName = InName.ToString();

	UTexture2D* MainTexture = nullptr;

	for (int Index = 0; Index < Tex_Count; ++Index)
	{
		// those package naming and creation is pain in ass in UE
		FString BaseTextureName = FString::Printf(TEXT("%s_Texture%02d"), *BaseName, Index);
		FString PaletteTextureName = FString::Printf(TEXT("%s_Texture%02d_palette"), *BaseName, Index);
		FString BasePackageName = PackagePath / BaseTextureName;
		FString PalettePackageName = PackagePath / PaletteTextureName;
        
		const int TexturePointer = Index*Tex_Size;
		FPGTim Tim(Buffer, TexturePointer);
		
		// Create package
		UPackage* BasePackage = CreatePackage(*BasePackageName);
		BasePackage->FullyLoad();
		UTexture2D* BaseTexture = Tim.CreateRawTextureGrayscale(BasePackage, BaseTextureName);
		FString BasePackageFilename = FPackageName::LongPackageNameToFilename(BasePackageName, FPackageName::GetAssetPackageExtension());
		bool bDirty = BasePackage->MarkPackageDirty();
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		UPackage::SavePackage(BasePackage, BaseTexture, *BasePackageName, SaveArgs);
		FAssetRegistryModule::AssetCreated(BaseTexture);

		// 
		UPackage* PalettePackage = CreatePackage(*PalettePackageName);
		PalettePackage->FullyLoad();
		UTexture2D * PaletteTexture = Tim.CreateClutPaletteTexture(PalettePackage, PaletteTextureName);
		FString PalettePackageFilename = FPackageName::LongPackageNameToFilename(PalettePackageName, FPackageName::GetAssetPackageExtension());
		bDirty = PalettePackage->MarkPackageDirty();
		UPackage::SavePackage(PalettePackage, PaletteTexture, *PalettePackageFilename, SaveArgs);
		FAssetRegistryModule::AssetCreated(PaletteTexture);
		
		//Base texture needs to be non sRGB :shrug:
		BaseTexture->SRGB = false;
		
		FPGTim::UpdateTextureParams(BaseTexture);
		FPGTim::UpdateTextureParams(PaletteTexture);
		

		if (!MainTexture)
			MainTexture = BaseTexture;
	}
	
	return MainTexture;
	
}


