#include "SLauncherWidget.h"

#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#include "DesktopPlatform/Public/IDesktopPlatform.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"
#include "ArgsPasserModule/Private/Core/LaunchTask.h"
#include "Controls/SArgEditor.h"

#define LOCTEXT_NAMESPACE "LauncherWidget"

void SLauncherWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew( SVerticalBox )
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding( 5.f )
		[
			SNew( SHorizontalBox )
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign( VAlign_Center )
			.Padding(FMargin(0.f, 0.f, 3.f, 0.f) )
			[
				SNew( STextBlock )
				.Text( NSLOCTEXT( "Launcher", "ExecutableText", "Executable:" ) )
			]
			+SHorizontalBox::Slot()
			.FillWidth( 1.f )
			[
				SNew( SEditableTextBox )
				.HintText( NSLOCTEXT( "Launcher", "PleaseEnterPath", "Please enter the path to the file") )
				.Text(this, &SLauncherWidget::GetExecutablePath)
				.OnTextCommitted(this, &SLauncherWidget::OnExecutablePathCommited)
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding( FMargin( 0.f, -1.f ) )
			[
				SNew( SButton )
				.Text( NSLOCTEXT( "Launcher", "Browse", "Browse...") )
				.OnClicked(this, &SLauncherWidget::OnBrowseExecutableClicked)
				.ContentPadding(FCoreStyle::Get().GetMargin("StandardDialog.ContentPadding"))
			]
		]		
		+SVerticalBox::Slot()
		.FillHeight( 1.f )
		.Padding( 5.f )
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5.f)
		[
			SNew(SArgEditor)
		]
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding( 5.f )
		.HAlign( HAlign_Right )
		[
			SNew( SHorizontalBox )
			+SHorizontalBox::Slot()
			.Padding( FMargin( 3.f, 0.f, 0.f, 0.f ) )
			[
				SNew( SBox )
				.WidthOverride( 100.f )
				[
					SNew( SButton )
					.HAlign(HAlign_Center)
					.Text( NSLOCTEXT( "Launcher", "ExecuteText", "Execute" ) )
					.IsEnabled(this, &SLauncherWidget::IsExecutablePathExist)
					.OnClicked(this, &SLauncherWidget::OnLaunchClicked)
				]
			]
		]
	];
}

bool SLauncherWidget::ShowOpenFileDialog(
	const FString& DefaultFolder,
	const FString& FileDescription,
	const FString& FileExtension,
	TArray<FString>& OutFileNames)
{
	// Prompt the user for the filenames
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	const FString FileTypes = FString::Printf( TEXT("%s (%s)|%s"), *FileDescription, *FileExtension, *FileExtension );
	bool bOpened = false;
	if ( DesktopPlatform )
	{
		void* ParentWindowWindowHandle = nullptr;

		const TSharedPtr<SWindow>& MainFrameParentWindow = FSlateApplicationBase::Get().GetActiveTopLevelWindow();
		if ( MainFrameParentWindow.IsValid() && MainFrameParentWindow->GetNativeWindow().IsValid() )
		{
		 	ParentWindowWindowHandle = MainFrameParentWindow->GetNativeWindow()->GetOSWindowHandle();
		}

		bOpened = DesktopPlatform->OpenFileDialog(
			ParentWindowWindowHandle,
			LOCTEXT("OpenProjectBrowseTitle", "Open Project").ToString(),
			DefaultFolder,
			TEXT(""),
			FileTypes,
			EFileDialogFlags::None,
			OutFileNames
		);
	}

	return bOpened;
}

FReply SLauncherWidget::OnBrowseExecutableClicked()
{
	const FString FileDescription = LOCTEXT( "FileTypeDescription", "All files" ).ToString();
	const FString FileExtension = FString::Printf(TEXT("*.*"));
	const FString DefaultFolder = FString();
	
	TArray< FString > FileNames;
	if (ShowOpenFileDialog(DefaultFolder, FileDescription, FileExtension, FileNames))
	{
		if (FileNames.Num() > 0)
		{
			ExecutablePath = FPaths::ConvertRelativePathToFull(FileNames[0]);
			bIsExecutablePathExist = CheckPathExist(ExecutablePath);
		}		
	}

	return FReply::Handled();
}

FReply SLauncherWidget::OnLaunchClicked()
{
	ensure(LaunchTaskPtr == nullptr);

	LaunchTaskPtr = new FAsyncTask<FLaunchTask>( ExecutablePath, TArray< FString >() );

	LaunchTaskPtr->StartBackgroundTask();

	return FReply::Handled();
}

FText SLauncherWidget::GetExecutablePath() const
{
	return FText::FromString( ExecutablePath );
}

void SLauncherWidget::OnExecutablePathCommited(const FText& InText, ETextCommit::Type Type)
{
	ExecutablePath = InText.ToString();
	bIsExecutablePathExist = CheckPathExist(ExecutablePath);
}

bool SLauncherWidget::IsExecutablePathExist() const
{
	return bIsExecutablePathExist;
}

bool SLauncherWidget::CheckPathExist(const FString& InPath)
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	return FileManager.FileExists(*InPath);
}

#undef LOCTEXT_NAMESPACE
