
#include "awful_undoredo.h"
#include "awful_cursorandmouse.h"

UndoManaga::UndoManaga()
{
    first_action = last_action = NULL;
    current_action = NULL;
    current_group = 0;
    started = false;

    DoNewAction(Action_Empty, NULL, NULL, NULL);
    StartNewGroup();
}

void UndoManaga::StartNewGroup()
{
    started = false;
    current_group++;
}

Action* UndoManaga::FlushSkipped(Action* fnext, Element* eltodel)
{
    Action* fnxt;
    Action* fact = fnext;
    while(fact != NULL)
    {
        fnxt = fact->next;
        if(fact->atype == Action_Delete || fact->atype == Action_Add)
        {
            Element* el = (Element*)fact->adata1;
            if(CheckDeletionSkip(el, eltodel) == true)
            {
                RemoveAction(fact);
            }
            else
            {
                break;
            }
        }
        fact = fnxt;
    }

    return fact;
}

void UndoManaga::FlushFurtherActions()
{
    Action* fnext;
    Action* fact = current_action->next;
    while(fact != NULL)
    {
        fnext = fact->next;

        if(fact->atype == Action_Add)
        {
            Element* el = (Element*)fact->adata1;
            el->to_be_deleted = true;
        }
        RemoveAction(fact);

        fact = fnext;
    }
    AdjustClipboardOffsets();
	current_action->next = NULL;
}

bool UndoManaga::IsOK(ActionType atype, void* adata)
{
    switch(atype)
    {
        case Action_Delete:
        {
            Element* el = (Element*)adata;
            if(el->deleted == true)
            {
                return false;
            }
        }
    }

    return true;
}

void UndoManaga::DoNewAction(ActionType atype, void* adata1, void* adata2, void* adata3)
{
    if(IsOK(atype, adata1))
    {
        started = true;
        if(current_action != NULL && current_action->next != NULL)
        {
            current_group = current_action->next->groupID;
            FlushFurtherActions();
        }

        Action* act = new Action;
        act->adata1 = adata1;
        act->adata2 = adata2;
        act->adata3 = adata3;
        act->atype = atype;
        act->groupID = current_group;
        AddAction(act);

        current_action = act;

        Perform(current_action);

		if(atype != Action_Empty)
			ChangesIndicate();
    }
}

void UndoManaga::DoNewAction(ActionType atype, void* adata1, float f1, float f2, int i1, int i2)
{
    if(IsOK(atype, adata1))
    {
        started = true;
        if(current_action != NULL && current_action->next != NULL)
        {
            FlushFurtherActions();
        }

        Action* act = new Action;
        act->adata1 = adata1;
        act->f1 = f1;
        act->f2 = f2;
        act->i1 = i1;
        act->i2 = i2;
        act->atype = atype;
        act->groupID = current_group;
        AddAction(act);

        current_action = act;

        Perform(current_action);

        ChangesIndicate();
	}
}

void UndoManaga::RedoAction()
{
    if(current_action->next != NULL)
    {
        int id = current_action->next->groupID;
        while(current_action->next != NULL && current_action->next->groupID == id)
        {
            current_action = current_action->next;
            Perform(current_action);
        }

        ChangesIndicate();
    }
}

void UndoManaga::UndoAction()
{
    if(current_action != NULL)
    {
        int id = current_action->groupID;
        while(current_action->prev != NULL && current_action->groupID == id)
        {
            Unperform(current_action);
            current_action = current_action->prev;
        }

        ChangesIndicate();
    }
}

void UndoManaga::Unperform(Action* act)
{
    switch(act->atype)
    {
        case Action_Delete:
        {
            Element* el = (Element*)act->adata1;
            SoftUnDelete(el);

            if(el->patt == field)
            {
                R(Refresh_GridContent);
                if(gAux->isVolsPansMode())
                {
                    R(Refresh_Aux);
                }
            }
            else
            {
                R(Refresh_AuxContent);
                R(Refresh_SubAux);
            }
        }break;
        case Action_Add:
        {
            Element* el = (Element*)act->adata1;
            SoftDelete(el, true);

            if(el->patt == field)
            {
                R(Refresh_GridContent);
                if(gAux->isVolsPansMode())
                {
                    R(Refresh_Aux);
                }
            }
            else
            {
                R(Refresh_AuxContent);
                R(Refresh_SubAux);
            }
        }break;
        case Action_Bind:
        {
            Pattern* pt = (Pattern*)act->adata1;
            Instrument* i1 = (Instrument*)act->adata2;
            Instrument* i2 = (Instrument*)act->adata3;

            pt->ibound = i1;
            pt->OrigPt->ibound = i1;
            pt->OrigPt->UpdateScaledImage();

            R(Refresh_GridContent);
            if(gAux->auxmode == AuxMode_Pattern)
            {
                R(Refresh_Aux);
            }
        }break;
        case Action_Move:
        {
            Element* el = (Element*)act->adata1;
            float dtick = act->f1;
            int dline = act->i1;

            el->Move(-dtick, -dline);

            if(el->patt == field)
            {
                R(Refresh_GridContent);
                if(gAux->auxmode == AuxMode_Vols ||
                   gAux->auxmode == AuxMode_Pans)
                {
                    R(Refresh_Aux);
                }
            }
            else
            {
                R(Refresh_AuxContent);
                R(Refresh_SubAux);
            }
        }break;
        case Action_ParamChange:
        {
            Parameter* param = (Parameter*)act->adata1;
            float val1 = act->f1;

            param->SetNormalValue(val1);

            //R(Refresh_GridContent);
            if(gAux->auxmode == AuxMode_Vols ||
               gAux->auxmode == AuxMode_Pans)
            {
                R(Refresh_Aux);
            }
            else
            {
                R(Refresh_AuxContent);
                R(Refresh_SubAux);
            }
        }break;
        case Action_Resize:
        {
            Element* el = (Element*)act->adata1;
            if(el->type == El_Samplent)
            {
                Samplent* sn = (Samplent*)el;
                sn->touchresized = act->i1 == 1;
                sn->sample->touchresized = act->i1 == 1;
            }

            el->tick_length = act->f1;
            el->end_tick = el->start_tick + el->tick_length;

            el->Update();

            if(el->patt == field)
            {
                R(Refresh_GridContent);
                if(gAux->workPt == el)
                {
                    pbAux->RangesToPattern();
                }
                if(gAux->auxmode != AuxMode_Pattern || gAux->workPt == el)
                {
                    R(Refresh_Aux);
                }
            }
            else
            {
                R(Refresh_AuxContent);
                R(Refresh_SubAux);
            }
        }break;
    }
}

void UndoManaga::Perform(Action* act)
{
    switch(act->atype)
    {
        case Action_Delete:
        {
            Element* el = (Element*)act->adata1;
            jassert(el->deleted == false);
            SoftDelete(el, false);

            if(el->patt == field)
            {
                R(Refresh_GridContent);
                if(gAux->auxmode == AuxMode_Vols || gAux->auxmode == AuxMode_Pans)
                {
                    R(Refresh_Aux);
                }
            }
            else
            {
                R(Refresh_AuxContent);
                R(Refresh_SubAux);
            }
        }break;
        case Action_Add:
        {
            Element* el = (Element*)act->adata1;
            SoftUnDelete(el);

            if(el->patt == field)
            {
                R(Refresh_GridContent);
                if(gAux->auxmode == AuxMode_Vols ||
                   gAux->auxmode == AuxMode_Pans)
                {
                    R(Refresh_Aux);
                }
            }
            else
            {
                R(Refresh_AuxContent);
                R(Refresh_SubAux);
            }
        }break;
        case Action_Bind:
        {
            Pattern* pt = (Pattern*)act->adata1;
            jassert(pt->deleted == false);
            Instrument* i1 = (Instrument*)act->adata2;
            Instrument* i2 = (Instrument*)act->adata3;

            pt->ibound = i2;
            pt->OrigPt->ibound = i2;
            pt->OrigPt->UpdateScaledImage();

            R(Refresh_GridContent);
            if(gAux->auxmode == AuxMode_Pattern)
            {
                R(Refresh_Aux);
            }
        }break;
        case Action_Move:
        {
            Element* el = (Element*)act->adata1;
            float dtick = act->f1;
            int dline = act->i1;

            el->Move(dtick, dline);

            if(el->patt == field)
            {
                R(Refresh_GridContent);
                if(gAux->auxmode == AuxMode_Vols ||
                   gAux->auxmode == AuxMode_Pans)
                {
                    R(Refresh_Aux);
                }
            }
            else
            {
                R(Refresh_AuxContent);
                R(Refresh_SubAux);
            }
        }break;
        case Action_ParamChange:
        {
            Parameter* param = (Parameter*)act->adata1;
            float val2 = act->f2;

            param->SetNormalValue(val2);

            //R(Refresh_GridContent);
            if(gAux->isVolsPansMode())
            {
                R(Refresh_Aux);
            }
            else
            {
                R(Refresh_AuxContent);
                R(Refresh_SubAux);
            }
        }break;
        case Action_Resize:
        {
            Element* el = (Element*)act->adata1;
            if(el->type == El_Samplent)
            {
                Samplent* sn = (Samplent*)el;
                sn->touchresized = act->i2 == 1;
                sn->sample->touchresized = act->i2 == 1;
            }

			el->tick_length = act->f2;
    		el->end_tick = el->start_tick + el->tick_length;

            el->Update();

            if(el->patt == field)
            {
                R(Refresh_GridContent);
                if(gAux->workPt == el)
                {
					if(!(Loo_Active == true && M.looloc == Loc_SmallGrid))
						pbAux->RangesToPattern();
                    R(Refresh_AuxGrid);
				}
                if(gAux->isVolsPansMode() && el->type == El_SlideNote)
                {
                    R(Refresh_Aux);
                }
            }
            else
            {
                R(Refresh_AuxContent);
                R(Refresh_SubAux);
            }
        }break;
    }
}

// Deletes all history entries, regarding the passed element and queues its hard deletion
void UndoManaga::WipeElementFromHistory(Element* el)
{
    Action* actnext;
    Action* act = first_action;
    while(act != NULL)
    {
        actnext = act->next;
        switch(act->atype)
        {
            case Action_Delete:
            case Action_Add:
            case Action_Bind:
            case Action_Move:
            case Action_Resize:
            {
                Element* elact = (Element*)act->adata1;
                if(el == elact)
                {
                    RemoveAction(act);
                }
            }break;
        }
        act = actnext;
    }
}

void UndoManaga::WipeInstrumentFromHistory(Instrument* instr)
{
    Action* actnext;
    Action* act = first_action;
    while(act != NULL)
    {
        actnext = act->next;
        switch(act->atype)
        {
            case Action_Bind:
            {
                Instrument* i1 = (Instrument*)act->adata2;
                Instrument* i2 = (Instrument*)act->adata3;
                if(i1 == instr || i2 == instr)
                {
                    RemoveAction(act);
                }
            }break;
        }
        act = actnext;
    }
}

void UndoManaga::WipeEntireHistory()
{
    Action* actnext;
    Action* act = first_action;
    while(act != NULL)
    {
        actnext = act->next;
        RemoveAction(act);
        act = actnext;
    }

    // Reinitialize as in constructor
    first_action = last_action = NULL;
    current_action = NULL;
    current_group = 0;
    started = false;

    DoNewAction(Action_Empty, NULL, NULL, NULL);
    StartNewGroup();
}

void UndoManaga::WipeParameterFromHistory(Parameter* param)
{
    Action* actnext;
    Action* act = first_action;
    while(act != NULL)
    {
        actnext = act->next;
        switch(act->atype)
        {
            case Action_ParamChange:
            {
                Parameter* prmact = (Parameter*)act->adata1;
                if(prmact == param)
                {
                    RemoveAction(act);
                }
            }break;
        }
        act = actnext;
    }
}

void UndoManaga::WipeEffectFromHistory(Eff* eff)
{
}

void UndoManaga::AddAction(Action* a)
{
    if(first_action == NULL && last_action == NULL)
    {
        a->prev = NULL;
        a->next = NULL;
        first_action = a;
    }
    else
    {
        last_action->next = a;
        a->prev = last_action;
        a->next = NULL;
    }
    last_action = a;
}

void UndoManaga::RemoveAction(Action* a)
{
    if(a == current_action)
    {
        current_action = current_action->prev;
    }

	if((a == first_action)&&(a == last_action))
	{
		first_action = NULL;
		last_action = NULL;
	}
	else if(a == first_action)
	{
		first_action = a->next;
		first_action->prev = NULL;
	}
	else if(a == last_action)
	{
		last_action = a->prev;
		last_action->next = NULL;
	}
	else
	{
		if(a->prev != NULL)
		{
		    a->prev->next = a->next;
        }
		if(a->next != NULL)
		{
		    a->next->prev = a->prev;
        }
	}
    delete a;
}

