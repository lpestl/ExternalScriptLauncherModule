#pragma once

#include "SlateBasics.h"

class SArgEditor : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS( SArgEditor ){}
	SLATE_END_ARGS()

	void Construct( const FArguments& InArgs );

private:
	TArray< TSharedPtr< FString > > ComboTypeItems;
};
