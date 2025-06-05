// All rights reserved. Marcin 'Maki' Gomulak. marcingomulak@gmail.com

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PGAssetUtilsActions.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGRIEVER_API UPGAssetUtilsActions : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	UFUNCTION(CallInEditor)
	void AddAssetPrefixes();

private:
#define PREFIX_MAP(ClassName, Prefix) {ClassName::StaticClass(), TEXT(Prefix)},
	TMap<UClass*, FString> PrefixesMap {
		PREFIX_MAP(UStaticMesh, "SM")
	    PREFIX_MAP(UTexture2D, "T")
		PREFIX_MAP(USkeletalMesh, "SK")
		PREFIX_MAP(ULevel, "L")
		PREFIX_MAP(UPhysicsAsset, "PHYS")
		PREFIX_MAP(USkeleton, "SKEL")
		PREFIX_MAP(UMaterial, "M")
		PREFIX_MAP(UMaterialInstance, "MI")
        };

public:
	UFUNCTION(CallInEditor)
	static void UpdateTextureInformation();
};
