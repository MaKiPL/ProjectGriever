// All rights reserved. Marcin 'Maki' Gomulak. marcingomulak@gmail.com

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "PGImporterTexl.generated.h"

//Declare log category
DECLARE_LOG_CATEGORY_EXTERN(LogPGImporterTexl, Log, All);
/**
 * 
 */
UCLASS()
class PROJECTGRIEVEREDITOR_API UPGImporterTexl : public UFactory
{
	GENERATED_BODY()

public:
	UPGImporterTexl();
	virtual bool FactoryCanImport(const FString& Filename) override;
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;

private:
	const int Tex_Count = 20;
	const int Tex_Size = 0x12800;
};
