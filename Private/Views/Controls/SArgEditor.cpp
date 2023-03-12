#include "SArgEditor.h"

#define LOCTEXT_NAMESPACE "ArgEditor"

void SArgEditor::Construct(const FArguments& InArgs)
{
	TSharedPtr<SGridPanel> ArgGrid = SNew(SGridPanel);

	int WidgetNum = 0, RowCount = 3, Cols = 3;
	auto NextSlot = [WidgetNum, RowCount, Cols](TSharedPtr<SGridPanel> Grid, const FText& InLabel, bool bIsDefaultEnable = false) mutable -> SHorizontalBox::FSlot&
	{
		SHorizontalBox::FSlot* ContentsSlot;    
		TSharedRef<SHorizontalBox> HBox = SNew(SHorizontalBox)
											+SHorizontalBox::Slot()
											.Expose(ContentsSlot)
											.VAlign(VAlign_Center)
											.HAlign(HAlign_Fill)
											.AutoWidth();

		if (!bIsDefaultEnable)
			HBox->SetEnabled(bIsDefaultEnable);

		// Checkbox to enable/disable arg part
		Grid->AddSlot((WidgetNum / RowCount)*Cols, WidgetNum % RowCount)
		.Padding(2.f)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		[
			SNew(SCheckBox)
			.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SimplifiedCheckbox"))
			.IsChecked_Lambda( [HBox] { return HBox->IsEnabled() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; } )
			.OnCheckStateChanged_Lambda( [HBox] (ECheckBoxState NewState) { HBox->SetEnabled( NewState == ECheckBoxState::Checked); })
		];

		// Add the Label for arg part
		Grid->AddSlot((WidgetNum / RowCount)*Cols + 1, WidgetNum % RowCount )
		.Padding(12.f, 2.f, 12.f, 2.f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Right)
		[
			SNew(STextBlock)
			.Font(FAppStyle::Get().GetFontStyle("NormalFont"))
			.ColorAndOpacity(FAppStyle::Get().GetSlateColor("Colors.White50"))
			.Text(InLabel)
		];

		//auto& Ret = Grid->AddSlot((WidgetNum / RowCount)*2 + 1, WidgetNum % RowCount)
		Grid->AddSlot((WidgetNum / RowCount)*Cols + 2, WidgetNum % RowCount)
		.Padding(12.f, 2.f, 12.f, 2.f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Fill)
		[
			HBox
		];

		++WidgetNum;
		return *ContentsSlot;
	};

	auto LeftRightLabel = [](const FName& InIconName = FName(), const FText& InLabel = FText::GetEmpty(), const FName& InTextStyle = TEXT("ButtonText")) -> TSharedRef<SWidget>
	{
		TSharedPtr<SHorizontalBox> HBox = SNew(SHorizontalBox);
		float Space = InIconName.IsNone() ? 0.0f : 8.f;

		if (!InIconName.IsNone())
		{
			HBox->AddSlot()	
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SImage)
				.ColorAndOpacity(FSlateColor::UseForeground())
				.Image(FAppStyle::Get().GetBrush(InIconName))
			];
		}

		if (!InLabel.IsEmpty())
		{
			HBox->AddSlot()	
			.VAlign(VAlign_Center)
			.Padding(Space, 0.5f, 0.f, 0.f)  // Compensate down for the baseline since we're using all caps
			.AutoWidth()
			[
				SNew(STextBlock)
				.TextStyle( &FAppStyle::Get().GetWidgetStyle< FTextBlockStyle >( InTextStyle ))
				.Justification(ETextJustify::Center)
				.Text(InLabel)
			];
		}

		return SNew(SBox)/*.HeightOverride(16.f)*/[ HBox.ToSharedRef() ];
	};

	// Prefix editor
	NextSlot(ArgGrid, LOCTEXT("PrefixLable", "Prefix"))
	.FillWidth(1.0)
	[
		SNew(SEditableTextBox)
		// .SetColorAndOpacity(FLinearColor::White)
		// .ForegroundColor(FLinearColor::White)
		// .BackgroundColor(FLinearColor::White)
		// .SetTextBoxForegroundColor(FLinearColor::White)
		// .SetTextBoxBackgroundColor(FLinearColor::White)
		// .(FLinearColor::White)
		.HintText(LOCTEXT("PrefixHint", "Enter the Prefix"))
	];

	// Argument name editor
	NextSlot(ArgGrid, LOCTEXT("ArgNameLable", "Argument name"), true)
	.FillWidth(1.0)
	[
		SNew(SEditableTextBox)
		.HintText(LOCTEXT("ArgNameHint", "Enter the Name of the argument"))
	];

	// Value type editor
	ComboTypeItems.Add(MakeShareable(new FString(TEXT("String"))));
	ComboTypeItems.Add(MakeShareable(new FString(TEXT("Int"))));
	ComboTypeItems.Add(MakeShareable(new FString(TEXT("Float"))));
	ComboTypeItems.Add(MakeShareable(new FString(TEXT("Bool"))));
	ComboTypeItems.Add(MakeShareable(new FString(TEXT("Item from List"))));
	ComboTypeItems.Add(MakeShareable(new FString(TEXT("File path"))));
	ComboTypeItems.Add(MakeShareable(new FString(TEXT("Directory path"))));
	
	TSharedPtr<STextBlock> ComboBoxTitleBlock;
	
	NextSlot(ArgGrid, LOCTEXT("ValueTypeLable", "Value type"))
	.FillWidth(1.0)
	[
		SNew(SGridPanel)
		+SGridPanel::Slot(0, 0)
		.Padding(2.f)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Font(FAppStyle::Get().GetFontStyle("NormalFont"))
			.ColorAndOpacity(FAppStyle::Get().GetSlateColor("Colors.White50"))
			.Text(LOCTEXT("SeparatingCharLable", "Separating char"))
		]
		+SGridPanel::Slot(1, 0)
		.Padding(12.f, 2.f, 12.f, 2.f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Fill)
		[
			SNew(SEditableTextBox)
			.HintText(LOCTEXT("SeparatingCharHint", "="))
		]
		+SGridPanel::Slot(0, 1)
		.ColumnSpan(2)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Fill)
		[
			SNew( SComboBox<TSharedPtr<FString> >)
			.OptionsSource(&ComboTypeItems)
			.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item) 
			{ 
				return SNew(STextBlock).Text( FText::FromString(*Item));
			} )
			.OnSelectionChanged_Lambda([ComboBoxTitleBlock] (TSharedPtr<FString> InSelection, ESelectInfo::Type InSelectInfo) 
			{
				if (InSelection.IsValid())
				{
					ComboBoxTitleBlock->SetText( FText::FromString(*InSelection));
				}
			} )
			[
				SAssignNew(ComboBoxTitleBlock, STextBlock).Text( FText::FromString(*ComboTypeItems[0]))  
			]
		]
	];
	
	ChildSlot
	[
		SNew(SBorder)
		.BorderImage( FAppStyle::Get().GetBrush("ToolPanel.GroupBorder") )        
		.Padding(3.f)
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.Padding(5.f)
			.AutoHeight()
			[
				SNew(SEditableText)
				.HintText(LOCTEXT("PreviewHint", "< Argument preview >"))
			]
			+SVerticalBox::Slot()
			[
				// SNew(SCheckBox)
				ArgGrid.ToSharedRef()
			]
		]
	];
}

#undef LOCTEXT_NAMESPACE