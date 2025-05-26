// All rights reserved. Marcin 'Maki' Gomulak. marcingomulak@gmail.com

#pragma once

#include "CoreMinimal.h"
#include "Factories/DataTableFactory.h"
#include "PGBattleXTestImporter.generated.h"


USTRUCT(BlueprintType)
struct PROJECTGRIEVEREDITOR_API FPGBattleXTestRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ObjectsCount;
};

UCLASS()
class PROJECTGRIEVEREDITOR_API UPGBattleXTestImporter : public UDataTableFactory
{
	GENERATED_BODY()

public:
	UPGBattleXTestImporter(const FObjectInitializer& ObjectInitializer);
	virtual FText GetDisplayName() const override;
	virtual bool FactoryCanImport(const FString& Filename) override;
	
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
};
