#include "DrJonesEditor.h"

DEFINE_LOG_CATEGORY(LogDrJonesEditor);

#define LOCTEXT_NAMESPACE "FDrJonesEditorModule"

void FDrJonesEditorModule::StartupModule()
{
    UE_LOG(LogDrJonesEditor, Display, TEXT("DrJonesEditor Module startup"));
}

void FDrJonesEditorModule::ShutdownModule()
{
    UE_LOG(LogDrJonesEditor, Display, TEXT("DrJonesEditor Module shutdown"));
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FDrJonesEditorModule, DrJonesEditor)
