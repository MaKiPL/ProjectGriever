#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FProjectGrieverEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    TArray<UObject*> RegisteredImporterFactories;
    // Menu extension methods
    TSharedRef<FExtender> ExtendContentBrowserMenu(const TArray<FAssetData>& SelectedPaths);
    void AddImportMultipleFilesMenu(FMenuBuilder& MenuBuilder);
    
    // Import action
    void ImportMultipleFilesToDataTable();
    
    // Helper methods
    bool ProcessBattleXFile(const FString& FilePath, UDataTable* DataTable, int32& RowIndex);
    
    // Keep track of delegates for cleanup
    TArray<FDelegateHandle> DelegateHandles;
};
