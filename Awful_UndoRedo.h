#ifndef UNDOREDO_H
#define UNDOREDO_H

#include "awful.h"
#include "awful_elements.h"
#include "awful_panels.h"

typedef enum ActionType
{
    Action_Empty,
    Action_Add,
    Action_Delete,
    Action_Move,
    Action_Bind,
    Action_ParamChange,
    Action_Resize
}ActionType;

typedef struct Action
{
    ActionType  atype;
    void*       adata1;
    void*       adata2;
    void*       adata3;

    float       f1;
    float       f2;

    int         i1;
    int         i2;

    int         groupID;

    Action*     prev;
    Action*     next;
}Action;

class UndoManaga
{
public:
    Action* first_action;
    Action* last_action;
    Action* current_action;
    int     current_group;

    bool    started;

    UndoManaga();

    void StartNewGroup();
    void Perform(Action* act);
    void Unperform(Action* current_action);
    Action* FlushSkipped(Action* fnext, Element* eltodel);
    void FlushFurtherActions();
    bool IsOK(ActionType atype, void* adata);
    void DoNewAction(ActionType atype, void* adata1, void* adata2, void* adata3);
    void DoNewAction(ActionType atype, void* adata1, float f1, float f2, int i1, int i2);
    void RedoAction();
    void UndoAction();
    void AddAction(Action* a);
    void RemoveAction(Action* a);
    void WipeElementFromHistory(Element* el);
    void WipeInstrumentFromHistory(Instrument* i);
    void WipeParameterFromHistory(Parameter* param);
    void WipeEffectFromHistory(Eff* eff);
    void WipeEntireHistory();
};

#endif //UNDOREDO_H
