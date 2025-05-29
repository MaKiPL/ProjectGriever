#include "PGTim.h"
#include "PGBufferHelperMacros.h"

DEFINE_LOG_CATEGORY(LogPGTim);

// TODO: Implement more modes, rather than just 16BPP

FPGTim::FPGTim(): Width(4), Height(4)
{
	
}

FPGTim::FPGTim(TArray<uint8>& Buffer, const UINT Offset)
{
	const uint8* TimData = Buffer.GetData() + Offset*sizeof(uint8);
	READ_VAR_CONST(UINT, Magic, TimData);
	READ_VAR_DECLARED(UINT, Mode, TimData);
	UE_LOG(LogPGTim, Log, TEXT("Magic: %x, Mode: %d"), Magic, Mode);

	if (Mode != 9)
	{
		UE_LOG(LogPGTim, Error, TEXT("Unsupported TIM mode: %d. Due to laziness I support 16BPP only for now"), Mode);
        return;
	}

	READ_VAR_DECLARED(USHORT, ClutSectionSize, TimData);
	DATA_SEEK(TimData, 6);

	READ_VAR_DECLARED(USHORT, ClutColorCount, TimData);
	READ_VAR_DECLARED(USHORT, ClutCount, TimData);

	UE_LOG(LogPGTim, Log, TEXT("ClutCount: %d, ClutColorCount: %d"), ClutCount, ClutColorCount);
	
	Cluts.SetNum(ClutCount);

	for (int Index = 0; Index < ClutCount; Index++)
		Cluts[Index] = ReadClut(TimData);

	READ_VAR_CONST(USHORT, TextureSectionSize, TimData);
	DATA_SEEK(TimData, 6);

	READ_VAR_DECLARED(USHORT, Width, TimData);
	Width *= 2;
	READ_VAR_DECLARED(USHORT, Height, TimData);
	
	RawTextureData.SetNumUninitialized(Width*Height);
	uint8* RawData = RawTextureData.GetData();
	FMemory::Memcpy(RawData, TimData, Width*Height);
}

void FPGTim::GetTimIntoTexture(UTexture2D*& Texture, const int ClutIndex)
{
	
}

TArray<FColor> & FPGTim::GetClutColors(const int ClutIndex)
{
	if (ClutIndex >= Cluts.Num())
	{
		UE_LOG(LogPGTim, Error, TEXT("Invalid clut index: %d (max: %d). Returning first clut"), ClutIndex, ClutCount - 1);
		return Cluts[0];
	}

    return Cluts[ClutIndex];
}

UTexture2D* FPGTim::CreateClutPaletteTexture(UObject* InParent, const FString& InTextureName)
{
	// Get the package path from parent
	FString PackagePath = FPackageName::GetLongPackagePath(InParent->GetPathName());
	FString PackageName = PackagePath / InTextureName;
    
	// Create package for the palette texture
	UPackage* Package = CreatePackage(*PackageName);
	
	UTexture2D * Texture = NewObject<UTexture2D>(InParent, UTexture2D::StaticClass(),
		*InTextureName, RF_Public | RF_Standalone);

	
	TArray<uint8> ClutRGBAData = TArray<uint8>();
	ClutRGBAData.Reserve(ClutColorCount * 4*ClutCount);

	for (int ClutIndex = 0; ClutIndex < ClutCount; ClutIndex++)
	{
		TArray<FColor> ClutColors = GetClutColors(ClutIndex);
		UE_LOG(LogPGTim, Log, TEXT("ClutIndex: %d. ClutColors.Num(): %d"), ClutIndex, ClutColors.Num());
		for (int Index = 0; Index < ClutColors.Num(); Index++)
		{
			//UINT PackedColor = ClutColors[Index].ToPackedBGRA(); // You can change this to other color format if needed
			//ClutRGBAData.Add(ClutColors[Index].ToPackedBGRA());
			FColor RGBA = ClutColors[Index];
			ClutRGBAData.Add(RGBA.B);
			ClutRGBAData.Add(RGBA.G);
			ClutRGBAData.Add(RGBA.R);
			ClutRGBAData.Add(RGBA.A);
		}
	}

	Texture->Source.Init(ClutColorCount, ClutCount, 1, 1, TSF_BGRA8, ClutRGBAData.GetData());
	// FTexturePlatformData * PlatformData = new FTexturePlatformData();
	// PlatformData->SizeX = ClutColorCount;
	// PlatformData->SizeY = ClutCount;
	// PlatformData->PixelFormat = PF_B8G8R8A8;
	// Texture->SetPlatformData(PlatformData);

	Texture->Filter = TF_Nearest;

	Texture->UpdateResource();
	bool bIsDirty = InParent->MarkPackageDirty();

	return Texture;
}

UTexture2D* FPGTim::CreateRawTextureGrayscale(UObject* InParent, const FString& InTextureName)
{
	// Get the package path from parent
	FString PackagePath = FPackageName::GetLongPackagePath(InParent->GetPathName());
	FString PackageName = PackagePath / InTextureName;
    
	// Create package for the palette texture
	UPackage* Package = CreatePackage(*PackageName);
	
	UTexture2D * Texture = NewObject<UTexture2D>(InParent, UTexture2D::StaticClass(),
		*InTextureName, RF_Public | RF_Standalone);

	TArray<uint8> ClutRGBAData = TArray<uint8>();
	ClutRGBAData.Reserve(Width*Height*4);

	for (int PixelIndex = 0; PixelIndex < Width*Height; PixelIndex++)
	{
		ClutRGBAData.Add(RawTextureData[PixelIndex]);
		ClutRGBAData.Add(RawTextureData[PixelIndex]);
		ClutRGBAData.Add(RawTextureData[PixelIndex]);
		ClutRGBAData.Add(255);
	}
	

	Texture->Source.Init(Width, Height, 1, 1, TSF_BGRA8, ClutRGBAData.GetData());
	// FTexturePlatformData * PlatformData = new FTexturePlatformData();
	// PlatformData->SizeX = ClutColorCount;
	// PlatformData->SizeY = ClutCount;
	// PlatformData->PixelFormat = PF_B8G8R8A8;
	// Texture->SetPlatformData(PlatformData);

	Texture->Filter = TF_Nearest;

	Texture->UpdateResource();
	bool bIsDirty = InParent->MarkPackageDirty();

	return Texture;
}

TArray<FColor> FPGTim::ReadClut(const uint8*& Buffer)
{
	TArray<FColor> ClutColors;
    ClutColors.Reserve(ClutColorCount);

	//This looks like shit with that switch-case, ehh
	switch (Mode)
	{
	default:
	case 9:
    for (int Index = 0; Index < ClutColorCount; Index++)
		{
    		ClutColors.Add(ReadPsxColor16(Buffer));
            Buffer += sizeof(USHORT);
		}
		break;
		
	}

	return ClutColors;
}

FColor FPGTim::ReadPsxColor16(const uint8* Buffer)
{
	const USHORT PixelColor =  *reinterpret_cast<const USHORT*>(Buffer);

	BYTE R = static_cast<BYTE>((PixelColor & 0x1F) << 3);
	BYTE G = static_cast<BYTE>((PixelColor >> 5 & 0x1F) << 3);
	BYTE B = static_cast<BYTE>((PixelColor >> 10 & 0x1F) << 3);
	
	R |= R >> 5;
	G |= G >> 5;
	B |= B >> 5;

	const BYTE A = static_cast<BYTE>((R == G == B == 0) ? 255 : 0);
	
	return FColor(R, G, B, A);
}

FColor FPGTim::ReadPsxColor8(const uint8* Buffer)
{
	//Not implemented yet
	return FColor::Black;
}
