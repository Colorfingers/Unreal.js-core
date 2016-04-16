#include "JavascriptEditor.h"
#include "JavascriptEditorTabManager.h"

#define LOCTEXT_NAMESPACE "JavascriptTabManager"

#if WITH_EDITOR
class JAVASCRIPTEDITOR_API SPrimaryDockingArea : public SBox
{
public:
	SLATE_BEGIN_ARGS(SPrimaryDockingArea)
	{}
	SLATE_END_ARGS()

	UJavascriptEditorTabManager* Owner;

	void Construct(const FArguments& InArgs)
	{
		SBox::Construct(SBox::FArguments());
	}

	virtual ~SPrimaryDockingArea()
	{
		Owner->Check(this);
	}
	
	TSharedPtr<FTabManager> TabManager;
	TWeakPtr<SDockTab> DockTab;
};
#endif

UJavascriptEditorTabManager::UJavascriptEditorTabManager(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{	
}

#if WITH_EDITOR	
void UJavascriptEditorTabManager::Setup(TSharedRef<SBox> Box)
{	
	auto DockArea = StaticCastSharedRef<SPrimaryDockingArea>(Box);

	TSharedRef<SDockTab> ConstructUnderMajorTab = DockArea->DockTab.Pin().ToSharedRef();
	TSharedPtr<SWindow> ConstructUnderWindow;

	auto TabManager = FGlobalTabmanager::Get()->NewTabManager(ConstructUnderMajorTab);
	auto AppMenuGroup = TabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("DeviceManagerMenuGroupName", "Device Manager"));

	TabManager->UnregisterAllTabSpawners();

	for (auto Tab : Tabs)
	{
		Tab->Register(TabManager, nullptr, AppMenuGroup);
	}

	auto CachedLayout = FTabManager::FLayout::NewFromString(Layout);		

	if (CachedLayout.IsValid())
	{
		Box->SetContent(
			TabManager->RestoreFrom(CachedLayout.ToSharedRef(), ConstructUnderWindow).ToSharedRef()
			);
	}	

	DockArea->TabManager = TabManager;
}

TSharedRef<SWidget> UJavascriptEditorTabManager::RebuildWidget()
{	
	// Tab manager requires a parent (SDocktab)
	auto DockTab = UJavascriptEditorTab::FindDocktab(this);
	if (DockTab.IsValid())
	{
		auto PrimaryArea = SNew(SPrimaryDockingArea);
		PrimaryArea->DockTab = DockTab;
		PrimaryArea->Owner = this;

		Setup(PrimaryArea);
		SpawnedAreas.Add(PrimaryArea);

		return PrimaryArea;
	}
	else
	{
		return SNew(SButton);
	}	
}

void UJavascriptEditorTabManager::Check(SBox* LastOne)
{
	for (int32 Index = SpawnedAreas.Num() - 1; Index >= 0; --Index)
	{
		if (!SpawnedAreas[Index].IsValid())
		{
			SpawnedAreas.RemoveAt(Index);
		}
	}
}
#endif
