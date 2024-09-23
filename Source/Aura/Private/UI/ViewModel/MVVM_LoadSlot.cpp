// P.S. project


#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadSlot::InitializeSlot()
{
	//TODO: check slot status based on loaded data
	SetWidgetSwitcherIndexDelegate.Broadcast(2);
}
