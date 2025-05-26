// All rights reserved. Marcin 'Maki' Gomulak. marcingomulak@gmail.com


#include "PGBattleXTestImporter.h"

UPGBattleXTestImporter::UPGBattleXTestImporter(const FObjectInitializer& ObjectInitializer)
{
	SupportedClass = UDataTable::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
	bEditAfterNew = true;

	Formats.Add(TEXT("x; BattleX Test Data"));
	bText = false;
}

FText UPGBattleXTestImporter::GetDisplayName() const
{
	return FText::FromString(TEXT("BattleX Test Data"));;
}

bool UPGBattleXTestImporter::FactoryCanImport(const FString& Filename)
{
	return FPaths::GetExtension(Filename).ToUpper() == TEXT("X");
}

UObject* UPGBattleXTestImporter::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	UDataTable* MyDataTable = NewObject<UDataTable>(InParent, InName, Flags);
	MyDataTable->RowStruct = FPGBattleXTestRow::StaticStruct();

	IPlatformFile* PlatformFile = &FPlatformFileManager::Get().GetPlatformFile();
	IFileHandle * FileHandle = PlatformFile->OpenRead(*Filename);
	if (!FileHandle)
		return nullptr;

    FileHandle->Seek(0x137C);
	uint32_t ObjectsCount;
	FileHandle->Read(reinterpret_cast<uint8*>(&ObjectsCount), sizeof(ObjectsCount));

	FPGBattleXTestRow Row;
	Row.ObjectsCount = ObjectsCount;
	
	MyDataTable->AddRow(FName(*FString::Printf(TEXT("Row%d"), 1)), Row);

	MyDataTable->MarkPackageDirty();
	
	return MyDataTable;
	
}
