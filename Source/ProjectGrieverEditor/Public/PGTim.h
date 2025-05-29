#pragma once
#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPGTim, Log, All);
class FPGTim
{
public:
	FPGTim();
	FPGTim(TArray<uint8>& Buffer, const UINT Offset = 0);
	void GetTimIntoTexture(UTexture2D*& Texture, const int ClutIndex = 0);

	/** Returns array of colors obtained from given clut index */
	TArray<FColor> & GetClutColors(int ClutIndex = 0);

	UTexture2D* CreateClutPaletteTexture(UObject* InParent, const FString& InTextureName);
	UTexture2D* CreateRawTextureGrayscale(UObject* InParent, const FString& InTextureName);

private:
	int Mode = 0;
	USHORT ClutSectionSize = 0;
	USHORT ClutColorCount = 0;
	USHORT ClutCount = 0;

	USHORT Width,Height;

	TArray<TArray<FColor>> Cluts;
	
	TArray<FColor> ReadClut(const uint8*& Buffer);
	FColor ReadPsxColor16(const uint8* Buffer);
	FColor ReadPsxColor8(const uint8* Buffer);

	TArray<uint8> RawTextureData;
};
