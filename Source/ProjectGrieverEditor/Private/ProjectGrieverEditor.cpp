#include "ProjectGrieverEditor.h"

#include "ContentBrowserModule.h"
#include "DesktopPlatformModule.h"
#include "PGBattleXTestImporter.h"
#include "AssetRegistry/AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "FProjectGrieverEditorModule"

void FProjectGrieverEditorModule::StartupModule()
{
	// Register content browser extension
	FContentBrowserModule& ContentBrowserModule = 
		FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
    
	TArray<FContentBrowserMenuExtender_SelectedAssets>& MenuExtenders =
		ContentBrowserModule.GetAllAssetViewContextMenuExtenders();

	// Add our menu extender
	MenuExtenders.Add(FContentBrowserMenuExtender_SelectedAssets::CreateRaw(this, &FProjectGrieverEditorModule::ExtendContentBrowserMenu));
    
	// Store handle for cleanup
	DelegateHandles.Add(MenuExtenders.Last().GetHandle());
}

void FProjectGrieverEditorModule::ShutdownModule()
{
    
}

TSharedRef<FExtender> FProjectGrieverEditorModule::ExtendContentBrowserMenu(const TArray<FAssetData>& SelectedPaths)
{
	TSharedRef<FExtender> Extender(new FExtender());
    
	Extender->AddMenuExtension(
		"AssetActions",
		EExtensionHook::After,
		nullptr,
		FMenuExtensionDelegate::CreateRaw(this, &FProjectGrieverEditorModule::AddImportMultipleFilesMenu)
	);
    
	return Extender;
}

void FProjectGrieverEditorModule::AddImportMultipleFilesMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
	LOCTEXT("ImportMultipleBattleXFiles", "Import Multiple BattleX Files"),
	LOCTEXT("ImportMultipleBattleXFilesToolTip", "Import multiple .x files into a single DataTable"),
	FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.AssetActions.Import"),
	FUIAction(FExecuteAction::CreateRaw(this, &FProjectGrieverEditorModule::ImportMultipleFilesToDataTable))
);
}

void FProjectGrieverEditorModule::ImportMultipleFilesToDataTable()
{
	// Open file dialog to select multiple files
	TArray<FString> OpenFilenames;
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    
	bool bOpened = false;
    
	// Get active window for dialog parenting
	void* ParentWindowHandle = FSlateApplication::Get().GetActiveTopLevelWindow()->GetNativeWindow()->GetOSWindowHandle();
    
	if (DesktopPlatform)
	{
		bOpened = DesktopPlatform->OpenFileDialog(
			ParentWindowHandle,
			LOCTEXT("ImportBattleXFiles", "Select BattleX Files to Import").ToString(),
			FPaths::ProjectContentDir(),
			TEXT(""),
			TEXT("BattleX Files (*.x)|*.x"),
			EFileDialogFlags::Multiple,
			OpenFilenames
		);
	}
	// Ask for asset name and location
	FString AssetName = TEXT("BattleXData");
	FString PathName = TEXT("/Game/Data");
    
	// Create the DataTable asset
	FString PackageName = PathName + TEXT("/") + AssetName;
	UPackage* Package = CreatePackage(*PackageName);
    
	UDataTable* DataTable = NewObject<UDataTable>(Package, FName(*AssetName), RF_Public | RF_Standalone);
	DataTable->RowStruct = FPGBattleXTestRow::StaticStruct();
    
	// Process each file
	int32 RowIndex = 0;
	int32 SuccessCount = 0;
    
	for (const FString& FilePath : OpenFilenames)
	{
		if (ProcessBattleXFile(FilePath, DataTable, RowIndex))
		{
			SuccessCount++;
		}
	}
    
	// Save the created asset
	if (SuccessCount > 0)
	{
		FAssetRegistryModule::AssetCreated(DataTable);
		DataTable->MarkPackageDirty();
        
		// Save package
		FString PackageFileName = FPackageName::LongPackageNameToFilename(
			PackageName, FPackageName::GetAssetPackageExtension());
            
		UPackage::SavePackage(
			Package, 
			DataTable, 
			RF_Public | RF_Standalone, 
			*PackageFileName, 
			GError, 
			nullptr, 
			false, 
			true, 
			SAVE_NoError
		);
        
		// Show success message
		FText SuccessMessage = FText::Format(
			LOCTEXT("ImportSuccessMessage", "Successfully imported {0} files into {1}"),
			FText::AsNumber(SuccessCount),
			FText::FromString(AssetName)
		);
        
		FMessageDialog::Open(EAppMsgType::Ok, SuccessMessage);
	}
	else
	{
		// Show error message
		FMessageDialog::Open(
			EAppMsgType::Ok,
			LOCTEXT("ImportFailedMessage", "Failed to import any files.")
		);
	}
}

bool FProjectGrieverEditorModule::ProcessBattleXFile(const FString& FilePath, UDataTable* DataTable, int32& RowIndex)
{
	FPGBattleXTestRow Row;
	Row.ObjectsCount = RowIndex;
	const FName RowName = FName(*FString::Printf(TEXT("%s_Row%d"), *FilePath, RowIndex++));
	DataTable->AddRow(RowName,Row);
	return true;
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FProjectGrieverEditorModule, ProjectGrieverEditor)