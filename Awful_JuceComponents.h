#ifndef __JUCER_HEADER_MAINCOMPONENT_MAINCOMPONENT_D0F6CD31__
#define __JUCER_HEADER_MAINCOMPONENT_MAINCOMPONENT_D0F6CD31__

#include "juce_amalgamated.h"
#include "awful_graphjuce.h"
#include "awful_juceext.h"
#include "rosic/basics/GlobalDefinitions.h"

class SliderBase;
class MainComponent;
class AGroupComponent;
class ASlider;
class ASliderListener;
class ALabel;
class ALabelListener;
class ALabelKeyboardFocusTraverser;
class AComboBox;
class AComboBoxListener;
class Animator;
class Posiator;
class DrawArea;
class AToggleButton;
class AButton;
class ATextButton;
class VSTParamComponent;
class CAudioDeviceManager;
class AMidiInputSelectorComponentListBox;

extern void drawGlassLozenge (Graphics& g,
                            const float x, const float y,
                            const float width, const float height,
                            const Colour& colour,
                            const float outlineThickness,
                            const float cornerSize,
                            const bool flatOnLeft,
                            const bool flatOnRight,
                            const bool flatOnTop,
                            const bool flatOnBottom);

/**
    A class for receiving events from an AComboBox.

    You can register a ComboBoxListener with a ComboBox using the ComboBox::addListener()
    method, and it will be called when the selected item in the box changes.

    @see ComboBox::addListener, ComboBox::removeListener
*/
class AComboBoxListener
{
public:
    /** Destructor. */
    virtual ~AComboBoxListener() {}

    /** Called when a ComboBox has its selected item changed.
    */
    virtual void comboBoxChanged (AComboBox* comboBoxThatHasChanged) = 0;
};

class ListeningComponent : public Component,
                                   private AComboBoxListener
{
public:
    AComboBox*  comboCC;

    bool    pos1visible;
    bool    pos2visible;
    bool    old1visible;
    bool    old2visible;

    int     posX;
    int     posW;
    int     pX;
    int     pW;
    int     oldX;
    int     oldW;
    bool    posChanged;

    int     pos2Offs;
    int     pos2X;
    int     pos2W;
    int     old2X;
    bool    pos2Changed;

    bool    mhredraw;

    int     cx, cy, cw, ch;
    int     mx, my, mw, mh;

    int     numclicks;
	int		rc;
	int		rd;

    bool    mouse_stuff_redraw;
    bool    auxhighlights_redraw;
    bool    auxhighlights_redraw_wait;

    DrawArea*   first_temp_drawarea;
    DrawArea*   last_temp_drawarea;

    MainComponent*  maincomp;

    ListeningComponent(MainComponent* mc);
    void paint(Graphics& g);
    //bool keyPressed(const KeyPress &key);
    void mouseUp(const MouseEvent& e);
    void mouseDown(const MouseEvent& e);
    void mouseMove(const MouseEvent& e);
    void mouseDrag(const MouseEvent& e);
    void mouseExit(const MouseEvent& e);
    void mouseEnter(const MouseEvent& e);
	void mouseWheelMove(const MouseEvent& e, float wheelIncrementX, float wheelIncrementY);
    void CheckAndRedraw();
    void Redraw(int flags);
    void UpdateRects();
    void CommonInputActions();
    void AddTempDrawArea(void* dobj, DrawObjType t, int x, int y, int w, int h);
    void RemoveTempDrawArea(DrawArea* da);
    void RefreshPos();
    void RefreshMenus();
    void comboBoxChanged (AComboBox* comboBoxThatHasChanged);
    INLINE void UpdatePosData()
    {
        int oldPos = posX;
        int old2Pos = pos2X;
        
        posX = currPlayX - RoundFloat(OffsTick*tickWidth);
        int w = GridX2 - GridX1;
        if(posX - 3 > w || posX + 3 < 0)
        {
            pos1visible = false;
        }
        else
        {
            pos1visible = true;
        }

        if(AuxCheckPosData(&pos2X))
        {
            if(pos2X < 0 || pos2X > GridXS2 - GridXS1)
            {
                pos2visible = false;
            }
            else
            {
                pos2visible = true;
            }
        }
        else
        {
            pos2visible = false;
        }

        if(oldPos != posX || pos1visible != old1visible)
        {
            posChanged = true;
        }
        if(old2Pos != pos2X || pos2visible != old2visible)
        {
            pos2Changed = true;
        }
    }
};

class DrawArea
{
public:
    int     ax;
    int     ay;
    int     aw;
    int     ah;

    DrawObjType type;
    void*       drawobject;

    bool    changed;
    bool    redraw;
    bool    set;

    DrawArea* prev;
    DrawArea* next;

    DrawArea(void* dobj, DrawObjType t);
    void SetBounds(int x, int y, int w, int h);
    void SetBounds(int x, int y, int w, int h, int xr, int yr, int wr, int hr);
    void ResetBounds();
    void Enable();
    void EnableWithBounds(int x, int y, int w, int h);
    void EnableWithBounds(int x, int y, int w, int h, int xr, int yr, int wr, int hr);
    void Disable();
    void DisableRedraw(Component* c);
    void Change();
    void UnChange();
    bool isEnabled();
    bool isChanged();

private:
    bool    enabled;
};

class HintComponent : public Component,
                            public Timer
{
public:
    String  text;
    int     width;
    MainComponent* maincomp;

    HintComponent(MainComponent* mc);
    ~HintComponent();
    void    paint(Graphics& g);
    void    SetText(String& text);
    void    Disable();
    void    timerCallback();
};

class MainComponent : public Component,
                            public ButtonListener
{
public:
    int                 refresh;
    int                 block;

    Image*              maingrid;
    Image*              auxgrid;
    Image*              mixer;

    DrawArea*           first_drawarea;
    DrawArea*           last_drawarea;

    Animator*           anim;
    Posiator*           poso;
    ListeningComponent* listen;
    HintComponent*      hint;

    MainComponent ();
    ~MainComponent();
    void    paint(Graphics& g);
    void    resized();
    void    buttonClicked(Button* buttonThatWasClicked);
    void    AddDrawArea(DrawArea* s);
    void    RemoveDrawArea(DrawArea* s);
    void    MakeSnapshot(Image** img, int x, int y, int w, int h, Image* otherimg = NULL);
    void    RefreshSeparates();
    void    EnableHint(String& text, int x, int y);
    void    DisableHint();
    void    ForceRefresh();

    //==============================================================================
    //[juce_UseDebuggingNewOperator

private:
    //TextButton* quitButton;

    MainComponent (const MainComponent&);
    const MainComponent& operator= (const MainComponent&);

    TextButton* animateButton;
    ComponentAnimator animator;
};

class Animator : public Timer
{
public:
    MainComponent* maincomp;

    Animator(MainComponent* maincomp);
    void timerCallback();
};

class Posiator : public Timer
{
public:
    MainComponent* maincomp;
    long mainPos;
    long auxPos;
    int add;
    bool scheduled_pattern_refresh;
    uint32 lasttime;

    Posiator(MainComponent* maincomp);
    void timerCallback();
    void initTimer(int interval = 30);
    void ScheduleRefresh();
};

class AButton   : public Button
{
public:

    AButton (const String& name);
    ~AButton();

    void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown);

    juce_UseDebuggingNewOperator
};

class ATextButton   : public AButton
{
public:

    ATextButton(const String& name);
    ATextButton(const String& name, const String& toolTip);
    ~ATextButton();

    void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown);

    juce_UseDebuggingNewOperator

private:

    void drawText (Graphics& g, ATextButton& button, bool isButtonDown);
};

/**
    A component that draws an outline around itself and has an optional title at
    the top, for drawing an outline around a group of controls.

*/
class AGroupComponent    : public Component
{
public:

    /** Creates a GroupComponent.

        @param componentName    the name to give the component
        @param labelText        the text to show at the top of the outline
    */
    AGroupComponent(const String& componentName,
                    const String& labelText);

    /** Destructor. */
    ~AGroupComponent();

    /** Changes the text that's shown at the top of the component. */
    void setText (const String& newText) throw();

    /** Returns the currently displayed text label. */
    const String getText() const throw();

    /** Sets the positioning of the text label.
        (The default is Justification::left)
        @see getTextLabelPosition
    */
    void setTextLabelPosition (const Justification& justification);

    /** Returns the current text label position.
        @see setTextLabelPosition
    */
    const Justification getTextLabelPosition() const throw()            { return justification; }

    /** A set of colour IDs to use to change the colour of various aspects of the component.
        These constants can be used either via the Component::setColour(), or LookAndFeel::setColour()
        methods.
        @see Component::setColour, Component::findColour, LookAndFeel::setColour, LookAndFeel::findColour
    */
    enum ColourIds
    {
        outlineColourId     = 0x1005400,    /**< The colour to use for drawing the line around the edge. */
        textColourId        = 0x1005410     /**< The colour to use to draw the text label. */
    };

    /** @internal */
    void paint (Graphics& g);
    /** @internal */
    void enablementChanged();
    /** @internal */
    void colourChanged();

private:
    String text;
    Justification justification;

    AGroupComponent (const AGroupComponent&);
    const AGroupComponent& operator= (const AGroupComponent&);
};

/**
    A component that displays a text string, and can optionally become a text
    editor when clicked.
*/


/**
    A component that displays a text string, and can optionally become a text
    editor when clicked.
*/
class ALabel  : public Component,
                         public SettableTooltipClient,
                         protected TextEditorListener,
                         private ComponentListener,
                         private AsyncUpdater
{
public:

    /** Creates a Label.

        @param componentName    the name to give the component
        @param labelText        the text to show in the label
    */
    ALabel (const String& componentName,
           const String& labelText);

    /** Destructor. */
    ~ALabel();

    /** Changes the label text.

        If broadcastChangeMessage is true and the new text is different to the current
        text, then the class will broadcast a change message to any LabelListeners that
        are registered.
    */
    void setText (const String& newText,
                  const bool broadcastChangeMessage);

    /** Returns the label's current text.

        @param returnActiveEditorContents   if this is true and the label is currently
                                            being edited, then this method will return the
                                            text as it's being shown in the editor. If false,
                                            then the value returned here won't be updated until
                                            the user has finished typing and pressed the return
                                            key.
    */
    const String getText (const bool returnActiveEditorContents = false) const throw();

    /** Changes the font to use to draw the text.

        @see getFont
    */
    void setFont (const Font& newFont) throw();

    /** Returns the font currently being used.

        @see setFont
    */
    const Font& getFont() const throw();

    /** A set of colour IDs to use to change the colour of various aspects of the label.

        These constants can be used either via the Component::setColour(), or LookAndFeel::setColour()
        methods.

        Note that you can also use the constants from TextEditor::ColourIds to change the
        colour of the text editor that is opened when a label is editable.

        @see Component::setColour, Component::findColour, LookAndFeel::setColour, LookAndFeel::findColour
    */
    enum ColourIds
    {
        backgroundColourId     = 0x1000280, /**< The background colour to fill the label with. */
        textColourId           = 0x1000281, /**< The colour for the text. */
        outlineColourId        = 0x1000282  /**< An optional colour to use to draw a border around the label.
                                                 Leave this transparent to not have an outline. */
    };

    /** Sets the style of justification to be used for positioning the text.

        (The default is Justification::centredLeft)
    */
    void setJustificationType (const Justification& justification) throw();

    /** Returns the type of justification, as set in setJustificationType(). */
    const Justification getJustificationType() const throw()                    { return justification; }

    /** Makes this label "stick to" another component.

        This will cause the label to follow another component around, staying
        either to its left or above it.

        @param owner    the component to follow
        @param onLeft   if true, the label will stay on the left of its component; if
                        false, it will stay above it.
    */
    void attachToComponent (Component* owner,
                            const bool onLeft);

    /** If this label has been attached to another component using attachToComponent, this
        returns the other component.

        Returns 0 if the label is not attached.
    */
    Component* getAttachedComponent() const throw()                             { return ownerComponent; }

    /** If the label is attached to the left of another component, this returns true.

        Returns false if the label is above the other component. This is only relevent if
        attachToComponent() has been called.
    */
    bool isAttachedOnLeft() const throw()                                       { return leftOfOwnerComp; }

    /** Registers a listener that will be called when the label's text changes. */
    void addListener (ALabelListener* const listener) throw();

    /** Deregisters a previously-registered listener. */
    void removeListener (ALabelListener* const listener) throw();

    /** Makes the label turn into a TextEditor when clicked.

        By default this is turned off.

        If turned on, then single- or double-clicking will turn the label into
        an editor. If the user then changes the text, then the ChangeBroadcaster
        base class will be used to send change messages to any listeners that
        have registered.

        If the user changes the text, the textWasEdited() method will be called
        afterwards, and subclasses can override this if they need to do anything
        special.

        @param editOnSingleClick            if true, just clicking once on the label will start editing the text
        @param editOnDoubleClick            if true, a double-click is needed to start editing
        @param lossOfFocusDiscardsChanges   if true, clicking somewhere else while the text is being
                                            edited will discard any changes; if false, then this will
                                            commit the changes.
        @see showEditor, setEditorColours, TextEditor
    */
    void setEditable (const bool editOnSingleClick,
                      const bool editOnDoubleClick = false,
                      const bool lossOfFocusDiscardsChanges = false) throw();

    /** Returns true if this option was set using setEditable(). */
    bool isEditableOnSingleClick() const throw()                        { return editSingleClick; }

    /** Returns true if this option was set using setEditable(). */
    bool isEditableOnDoubleClick() const throw()                        { return editDoubleClick; }

    /** Returns true if this option has been set in a call to setEditable(). */
    bool doesLossOfFocusDiscardChanges() const throw()                  { return lossOfFocusDiscardsChanges; }

    /** Returns true if the user can edit this label's text. */
    bool isEditable() const throw()                                     { return editSingleClick || editDoubleClick; }

    /** Makes the editor appear as if the label had been clicked by the user.

        @see textWasEdited, setEditable
    */
    void showEditor();

    /** Hides the editor if it was being shown.

        @param discardCurrentEditorContents     if true, the label's text will be
                                                reset to whatever it was before the editor
                                                was shown; if false, the current contents of the
                                                editor will be used to set the label's text
                                                before it is hidden.
    */
    void hideEditor (const bool discardCurrentEditorContents);

    /** Returns true if the editor is currently focused and active. */
    bool isBeingEdited() const throw();

    juce_UseDebuggingNewOperator

protected:
    /** @internal */
    void paint (Graphics& g);
    /** @internal */
    void resized();
    /** @internal */
    void mouseUp (const MouseEvent& e);
    /** @internal */
    void mouseDoubleClick (const MouseEvent& e);
    /** @internal */
    void componentMovedOrResized (Component& component, bool wasMoved, bool wasResized);
    /** @internal */
    void componentParentHierarchyChanged (Component& component);
    /** @internal */
    void componentVisibilityChanged (Component& component);
    /** @internal */
    void inputAttemptWhenModal();
    /** @internal */
    void focusGained (FocusChangeType);
    /** @internal */
    void enablementChanged();
    /** @internal */
    KeyboardFocusTraverser* createFocusTraverser();
    /** @internal */
    void textEditorTextChanged (TextEditor& editor);
    /** @internal */
    void textEditorReturnKeyPressed (TextEditor& editor);
    /** @internal */
    void textEditorEscapeKeyPressed (TextEditor& editor);
    /** @internal */
    void textEditorFocusLost (TextEditor& editor);
    /** @internal */
    void handleAsyncUpdate();
    /** @internal */
    void colourChanged();

    /** Creates the TextEditor component that will be used when the user has clicked on the label.

        Subclasses can override this if they need to customise this component in some way.
    */
    virtual TextEditor* createEditorComponent();

    /** Called after the user changes the text.
    */
    virtual void textWasEdited();

private:
    String text;
    Font font;
    Justification justification;
    TextEditor* editor;
    SortedSet <void*> listeners;
    Component* ownerComponent;
    ComponentDeletionWatcher* deletionWatcher;

    bool editSingleClick : 1;
    bool editDoubleClick : 1;
    bool lossOfFocusDiscardsChanges : 1;
    bool leftOfOwnerComp : 1;

    bool updateFromTextEditorContents();

    ALabel (const ALabel&);
    const ALabel& operator= (const ALabel&);
};

/**
    A class for receiving events from a Label.

    You can register a LabelListener with a Label using the Label::addListener()
    method, and it will be called when the text of the label changes, either because
    of a call to Label::setText() or by the user editing the text (if the label is
    editable).

    @see Label::addListener, Label::removeListener
*/
class ALabelListener
{
public:
    /** Destructor. */
    virtual ~ALabelListener() {}

    /** Called when a Label's text has changed.
    */
    virtual void labelTextChanged (ALabel* labelThatHasChanged) = 0;
};

class AComboBox  : public Component,
                     public SettableTooltipClient,
                     private ALabelListener,
                     private AsyncUpdater
{
public:

    /** Creates a combo-box.

        On construction, the text field will be empty, so you should call the
        setSelectedId() or setText() method to choose the initial value before
        displaying it.

        @param componentName    the name to set for the component (see Component::setName())
    */
    AComboBox (const String& componentName);

    /** Destructor. */
    ~AComboBox();

    /** Sets whether the test in the combo-box is editable.

        The default state for a new ComboBox is non-editable, and can only be changed
        by choosing from the drop-down list.
    */
    void setEditableText (const bool isEditable);

    /** Returns true if the text is directly editable.
        @see setEditableText
    */
    bool isTextEditable() const throw();

    /** Sets the style of justification to be used for positioning the text.

        The default is Justification::centredLeft. The text is displayed using a
        Label component inside the ComboBox.
    */
    void setJustificationType (const Justification& justification) throw();

    /** Returns the current justification for the text box.
        @see setJustificationType
    */
    const Justification getJustificationType() const throw();

    /** Adds an item to be shown in the drop-down list.

        @param newItemText      the text of the item to show in the list
        @param newItemId        an associated ID number that can be set or retrieved - see
                                getSelectedId() and setSelectedId()
        @see setItemEnabled, addSeparator, addSectionHeading, removeItem, getNumItems, getItemText, getItemId
    */
    void addItem (const String& newItemText,
                  const int newItemId) throw();

    /** Adds a separator line to the drop-down list.

        This is like adding a separator to a popup menu. See PopupMenu::addSeparator().
    */
    void addSeparator() throw();

    /** Adds a heading to the drop-down list, so that you can group the items into
        different sections.

        The headings are indented slightly differently to set them apart from the
        items on the list, and obviously can't be selected. You might want to add
        separators between your sections too.

        @see addItem, addSeparator
    */
    void addSectionHeading (const String& headingName) throw();

    /** This allows items in the drop-down list to be selectively disabled.

        When you add an item, it's enabled by default, but you can call this
        method to change its status.

        If you disable an item which is already selected, this won't change the
        current selection - it just stops the user choosing that item from the list.
    */
    void setItemEnabled (const int itemId,
                         const bool isEnabled) throw();

    /** Changes the text for an existing item.
    */
    void changeItemText (const int itemId,
                         const String& newText) throw();

    /** Removes all the items from the drop-down list.

        If this call causes the content to be cleared, then a change-message
        will be broadcast unless dontSendChangeMessage is true.

        @see addItem, removeItem, getNumItems
    */
    void clear (const bool dontSendChangeMessage = false);

    /** Returns the number of items that have been added to the list.

        Note that this doesn't include headers or separators.
    */
    int getNumItems() const throw();

    /** Returns the text for one of the items in the list.

        Note that this doesn't include headers or separators.

        @param index    the item's index from 0 to (getNumItems() - 1)
    */
    const String getItemText (const int index) const throw();

    /** Returns the ID for one of the items in the list.

        Note that this doesn't include headers or separators.

        @param index    the item's index from 0 to (getNumItems() - 1)
    */
    int getItemId (const int index) const throw();

    /** Returns the ID of the item that's currently shown in the box.

        If no item is selected, or if the text is editable and the user
        has entered something which isn't one of the items in the list, then
        this will return 0.

        @see setSelectedId, getSelectedItemIndex, getText
    */
    int getSelectedId() const throw();

    /** Sets one of the items to be the current selection.

        This will set the ComboBox's text to that of the item that matches
        this ID.

        @param newItemId                the new item to select
        @param dontSendChangeMessage    if set to true, this method won't trigger a
                                        change notification
        @see getSelectedId, setSelectedItemIndex, setText
    */
    void setSelectedId (const int newItemId,
                        const bool dontSendChangeMessage = false) throw();

    /** Returns the index of the item that's currently shown in the box.

        If no item is selected, or if the text is editable and the user
        has entered something which isn't one of the items in the list, then
        this will return -1.

        @see setSelectedItemIndex, getSelectedId, getText
    */
    int getSelectedItemIndex() const throw();

    /** Sets one of the items to be the current selection.

        This will set the ComboBox's text to that of the item at the given
        index in the list.

        @param newItemIndex             the new item to select
        @param dontSendChangeMessage    if set to true, this method won't trigger a
                                        change notification
        @see getSelectedItemIndex, setSelectedId, setText
    */
    void setSelectedItemIndex (const int newItemIndex,
                               const bool dontSendChangeMessage = false) throw();

    /** Returns the text that is currently shown in the combo-box's text field.

        If the ComboBox has editable text, then this text may have been edited
        by the user; otherwise it will be one of the items from the list, or
        possibly an empty string if nothing was selected.

        @see setText, getSelectedId, getSelectedItemIndex
    */
    const String getText() const throw();

    /** Sets the contents of the combo-box's text field.

        The text passed-in will be set as the current text regardless of whether
        it is one of the items in the list. If the current text isn't one of the
        items, then getSelectedId() will return -1, otherwise it wil return
        the approriate ID.

        @param newText                  the text to select
        @param dontSendChangeMessage    if set to true, this method won't trigger a
                                        change notification
        @see getText
    */
    void setText (const String& newText,
                  const bool dontSendChangeMessage = false) throw();

    /** Programmatically opens the text editor to allow the user to edit the current item.

        This is the same effect as when the box is clicked-on.
        @see Label::showEditor();
    */
    void showEditor();

    /** Registers a listener that will be called when the box's content changes. */
    void addListener (AComboBoxListener* const listener) throw();

    /** Deregisters a previously-registered listener. */
    void removeListener (AComboBoxListener* const listener) throw();

    /** Sets a message to display when there is no item currently selected.

        @see getTextWhenNothingSelected
    */
    void setTextWhenNothingSelected (const String& newMessage) throw();

    /** Returns the text that is shown when no item is selected.

        @see setTextWhenNothingSelected
    */
    const String getTextWhenNothingSelected() const throw();

    /** Sets the message to show when there are no items in the list, and the user clicks
        on the drop-down box.

        By default it just says "no choices", but this lets you change it to something more
        meaningful.
    */
    void setTextWhenNoChoicesAvailable (const String& newMessage) throw();

    /** Returns the text shown when no items have been added to the list.
        @see setTextWhenNoChoicesAvailable
    */
    const String getTextWhenNoChoicesAvailable() const throw();

    /** Gives the ComboBox a tooltip. */
    void setTooltip (const String& newTooltip);

    /** A set of colour IDs to use to change the colour of various aspects of the combo box.

        These constants can be used either via the Component::setColour(), or LookAndFeel::setColour()
        methods.

        To change the colours of the menu that pops up

        @see Component::setColour, Component::findColour, LookAndFeel::setColour, LookAndFeel::findColour
    */
    enum ColourIds
    {
        backgroundColourId  = 0x1000b00,    /**< The background colour to fill the box with. */
        textColourId        = 0x1000a00,    /**< The colour for the text in the box. */
        outlineColourId     = 0x1000c00,    /**< The colour for an outline around the box. */
        buttonColourId      = 0x1000d00,    /**< The base colour for the button (a LookAndFeel class will probably use variations on this). */
        arrowColourId       = 0x1000e00,    /**< The colour for the arrow shape that pops up the menu */
    };

    /** @internal */
    void labelTextChanged (ALabel*);
    /** @internal */
    void enablementChanged();
    /** @internal */
    void colourChanged();
    /** @internal */
    void focusGained (Component::FocusChangeType cause);
    /** @internal */
    void focusLost (Component::FocusChangeType cause);
    /** @internal */
    void handleAsyncUpdate();
    /** @internal */
    const String getTooltip()                                       { return label->getTooltip(); }
    /** @internal */
    void mouseDown (const MouseEvent&);
    /** @internal */
    void mouseDrag (const MouseEvent&);
    /** @internal */
    void mouseUp (const MouseEvent&);
    /** @internal */
    void lookAndFeelChanged();
    /** @internal */
    void paint (Graphics&);
    /** @internal */
    void resized();
    /** @internal */
    bool keyStateChanged();
    /** @internal */
    bool keyPressed (const KeyPress&);

    juce_UseDebuggingNewOperator

private:
    struct ItemInfo
    {
        String name;
        int itemId;
        bool isEnabled : 1, isHeading : 1;

        bool isSeparator() const throw();
        bool isRealItem() const throw();
    };

    OwnedArray <ItemInfo> items;
    int currentIndex;
    bool isButtonDown;
    bool separatorPending;
    bool menuActive;
    SortedSet <void*> listeners;
    ALabel* label;
    String textWhenNothingSelected, noChoicesMessage;

    void showPopup();

    int lbx, lby;

    ItemInfo* getItemForId (const int itemId) const throw();
    ItemInfo* getItemForIndex (const int index) const throw();

    AComboBox (const AComboBox&);
    const AComboBox& operator= (const AComboBox&);
};

/**
    A button that can be toggled on/off.

    All buttons can be toggle buttons, but this lets you create one of the
    standard ones which has a tick-box and a text label next to it.

    @see Button, DrawableButton, TextButton
*/
class AToggleButton  : public Button
{
public:

    /** Creates a ToggleButton.

        @param buttonText   the text to put in the button (the component's name is also
                            initially set to this string, but these can be changed later
                            using the setName() and setButtonText() methods)
    */
    AToggleButton (const String& buttonText);

    /** Destructor. */
    ~AToggleButton();

    /** Resizes the button to fit neatly around its current text.

        The button's height won't be affected, only its width.
    */
    void changeWidthToFitText();

    void setType(int type);
    int getType();

    /** A set of colour IDs to use to change the colour of various aspects of the button.

        These constants can be used either via the Component::setColour(), or LookAndFeel::setColour()
        methods.

        @see Component::setColour, Component::findColour, LookAndFeel::setColour, LookAndFeel::findColour
    */
    enum ColourIds
    {
        textColourId                    = 0x1006501   /**< The colour to use for the button's text. */
    };

    juce_UseDebuggingNewOperator

protected:
    /** @internal */
    void paintButton (Graphics& g,
                      bool isMouseOverButton,
                      bool isButtonDown);

    /** @internal */
    void colourChanged();

private:

    AToggleButton (const AToggleButton&);
    const AToggleButton& operator= (const AToggleButton&);

    int tgtype;
};

/**
    A class for receiving callbacks from a ASlider.

    To be told when a slider's value changes, you can register a SliderListener
    object using ASlider::addListener().

    @see ASlider::addListener, ASlider::removeListener
*/
class ASliderListener
{
public:

    /** Destructor. */
    virtual ~ASliderListener() {}

    /** Called when the slider's value is changed.

        This may be caused by dragging it, or by typing in its text entry box,
        or by a call to ASlider::setValue().

        You can find out the new value using ASlider::getValue().

        @see ASlider::valueChanged
    */
    virtual void sliderValueChanged (ASlider* slider) = 0;

    /** Called when the slider is about to be dragged.

        This is called when a drag begins, then it's followed by multiple calls
        to sliderValueChanged(), and then sliderDragEnded() is called after the
        user lets go.

        @see sliderDragEnded, ASlider::startedDragging
    */
    virtual void sliderDragStarted (ASlider* slider);

    /** Called after a drag operation has finished.

        @see sliderDragStarted, ASlider::stoppedDragging
    */
    virtual void sliderDragEnded (ASlider* slider);
};

/**
    A slider control for changing a value.

    The slider can be horizontal, vertical, or rotary, and can optionally have
    a text-box inside it to show an editable display of the current value.

    To use it, create a ASlider object and use the setSliderStyle() method
    to set up the type you want. To set up the text-entry box, use setTextBoxStyle().

    To define the values that it can be set to, see the setRange() and setValue() methods.

    There are also lots of custom tweaks you can do by subclassing and overriding
    some of the virtual methods, such as changing the scaling, changing the format of
    the text display, custom ways of limiting the values, etc.

    You can register SliderListeners with a slider, which will be informed when the value
    changes, or a subclass can override valueChanged() to be informed synchronously.

    @see SliderListener
*/
class ASlider  : public Component,
                 public SettableTooltipClient,
                 private AsyncUpdater,
                 private ButtonListener,
                 private ALabelListener
{
public:

    /** Creates a slider.

        When created, you'll need to set up the slider's style and range with setSliderStyle(),
        setRange(), etc.
    */
    ASlider (const String& componentName);

    /** Destructor. */
    ~ASlider();

    /** The types of slider available.

        @see setSliderStyle, setRotaryParameters
    */
    enum SliderStyle
    {
        LinearHorizontal,       /**< A traditional horizontal slider. */
        LinearHorizontalSigned, /**< A traditional horizontal slider signed. */
        LinearVertical,         /**< A traditional vertical slider. */
        LinearBar,              /**< A horizontal bar slider with the text label drawn on top of it. */
        Rotary,                 /**< A rotary control that you move by dragging the mouse in a circular motion, like a knob.
                                     @see setRotaryParameters */
        RotaryHorizontalDrag,   /**< A rotary control that you move by dragging the mouse left-to-right.
                                     @see setRotaryParameters */
        RotaryVerticalDrag,     /**< A rotary control that you move by dragging the mouse up-and-down.
                                     @see setRotaryParameters */
        IncDecButtons,          /**< A pair of buttons that increment or decrement the slider's value by the increment set in setRange(). */

        TwoValueHorizontal,     /**< A horizontal slider that has two thumbs instead of one, so it can show a minimum and maximum value.
                                     @see setMinValue, setMaxValue */
        TwoValueVertical,       /**< A vertical slider that has two thumbs instead of one, so it can show a minimum and maximum value.
                                     @see setMinValue, setMaxValue */

        ThreeValueHorizontal,   /**< A horizontal slider that has three thumbs instead of one, so it can show a minimum and maximum
                                     value, with the current value being somewhere between them.
                                     @see setMinValue, setMaxValue */
        ThreeValueVertical,     /**< A vertical slider that has three thumbs instead of one, so it can show a minimum and maximum
                                     value, with the current value being somewhere between them.
                                     @see setMinValue, setMaxValue */
    };

    /** Changes the type of slider interface being used.

        @param newStyle         the type of interface
        @see setRotaryParameters, setVelocityBasedMode,
    */
    void setSliderStyle (const SliderStyle newStyle);

    /** Returns the slider's current style.

        @see setSliderStyle
    */
    SliderStyle getSliderStyle() const throw()                                  { return style; }

    /** Changes the properties of a rotary slider.

        @param startAngleRadians        the angle (in radians, clockwise from the top) at which
                                        the slider's minimum value is represented
        @param endAngleRadians          the angle (in radians, clockwise from the top) at which
                                        the slider's maximum value is represented. This must be
                                        greater than startAngleRadians
        @param stopAtEnd                if true, then when the slider is dragged around past the
                                        minimum or maximum, it'll stop there; if false, it'll wrap
                                        back to the opposite value
    */
    void setRotaryParameters (const float startAngleRadians,
                              const float endAngleRadians,
                              const bool stopAtEnd);

    /** Sets the distance the mouse has to move to drag the slider across
        the full extent of its range.

        This only applies when in modes like RotaryHorizontalDrag, where it's using
        relative mouse movements to adjust the slider.
    */
    void setMouseDragSensitivity (const int distanceForFullScaleDrag);

    /** Changes the way the the mouse is used when dragging the slider.

        If true, this will turn on velocity-sensitive dragging, so that
        the faster the mouse moves, the bigger the movement to the slider. This
        helps when making accurate adjustments if the slider's range is quite large.

        If false, the slider will just try to snap to wherever the mouse is.
    */
    void setVelocityBasedMode (const bool isVelocityBased) throw();

    /** Changes aspects of the scaling used when in velocity-sensitive mode.

        These apply when you've used setVelocityBasedMode() to turn on velocity mode,
        or if you're holding down ctrl.

        @param sensitivity      higher values than 1.0 increase the range of acceleration used
        @param threshold        the minimum number of pixels that the mouse needs to move for it
                                to be treated as a movement
        @param offset           values greater than 0.0 increase the minimum speed that will be used when
                                the threshold is reached
        @param userCanPressKeyToSwapMode    if true, then the user can hold down the ctrl or command
                                key to toggle velocity-sensitive mode
    */
    void setVelocityModeParameters (const double sensitivity = 1.0,
                                    const int threshold = 1.0,
                                    const double offset = 0.0,
                                    const bool userCanPressKeyToSwapMode = true) throw();

    /** Sets up a skew factor to alter the way values are distributed.

        You may want to use a range of values on the slider where more accuracy
        is required towards one end of the range, so this will logarithmically
        spread the values across the length of the slider.

        If the factor is < 1.0, the lower end of the range will fill more of the
        slider's length; if the factor is > 1.0, the upper end of the range
        will be expanded instead. A factor of 1.0 doesn't skew it at all.

        To set the skew position by using a mid-point, use the setSkewFactorFromMidPoint()
        method instead.

        @see getSkewFactor, setSkewFactorFromMidPoint
    */
    void setSkewFactor (const double factor) throw();

    /** Sets up a skew factor to alter the way values are distributed.

        This allows you to specify the slider value that should appear in the
        centre of the slider's visible range.

        @see setSkewFactor, getSkewFactor
     */
    void setSkewFactorFromMidPoint (const double sliderValueToShowAtMidPoint) throw();

    /** Returns the current skew factor.

        See setSkewFactor for more info.

        @see setSkewFactor, setSkewFactorFromMidPoint
    */
    double getSkewFactor() const throw()                        { return skewFactor; }

    /** Used by setIncDecButtonsMode().
    */
    enum IncDecButtonMode
    {
        incDecButtonsNotDraggable,
        incDecButtonsDraggable_AutoDirection,
        incDecButtonsDraggable_Horizontal,
        incDecButtonsDraggable_Vertical
    };

    /** When the style is IncDecButtons, this lets you turn on a mode where the mouse
        can be dragged on the buttons to drag the values.

        By default this is turned off. When enabled, clicking on the buttons still works
        them as normal, but by holding down the mouse on a button and dragging it a little
        distance, it flips into a mode where the value can be dragged. The drag direction can
        either be set explicitly to be vertical or horizontal, or can be set to
        incDecButtonsDraggable_AutoDirection so that it depends on whether the buttons
        are side-by-side or above each other.
    */
    void setIncDecButtonsMode (const IncDecButtonMode mode);

    /** The position of the slider's text-entry box.

        @see setTextBoxStyle
    */
    enum TextEntryBoxPosition
    {
        NoTextBox,              /**< Doesn't display a text box.  */
        TextBoxLeft,            /**< Puts the text box to the left of the slider, vertically centred.  */
        TextBoxRight,           /**< Puts the text box to the right of the slider, vertically centred.  */
        TextBoxAbove,           /**< Puts the text box above the slider, horizontally centred.  */
        TextBoxBelow,            /**< Puts the text box below the slider, horizontally centred.  */
        TextBoxPositioned
    };

    /** Changes the location and properties of the text-entry box.

        @param newPosition          where it should go (or NoTextBox to not have one at all)
        @param isReadOnly           if true, it's a read-only display
        @param textEntryBoxWidth    the width of the text-box in pixels. Make sure this leaves enough
                                    room for the slider as well!
        @param textEntryBoxHeight   the height of the text-box in pixels. Make sure this leaves enough
                                    room for the slider as well!

        @see setTextBoxIsEditable, getValueFromText, getTextFromValue
    */
    void setTextBoxStyle (const TextEntryBoxPosition newPosition,
                          const bool isReadOnly,
                          const int textEntryBoxWidth,
                          const int textEntryBoxHeight);

    /** Returns the status of the text-box.
        @see setTextBoxStyle
    */
    const TextEntryBoxPosition getTextBoxPosition() const throw()           { return textBoxPos; }

    /** Returns the width used for the text-box.
        @see setTextBoxStyle
    */
    int getTextBoxWidth() const throw()                                     { return textBoxWidth; }

    /** Returns the height used for the text-box.
        @see setTextBoxStyle
    */
    int getTextBoxHeight() const throw()                                    { return textBoxHeight; }

    void setTextBoxXY(int x, int y) {textBoxX = x; textBoxY = y; resized();};

    /** Makes the text-box editable.

        By default this is true, and the user can enter values into the textbox,
        but it can be turned off if that's not suitable.

        @see setTextBoxStyle, getValueFromText, getTextFromValue
    */
    void setTextBoxIsEditable (const bool shouldBeEditable) throw();

    /** Returns true if the text-box is read-only.
        @see setTextBoxStyle
    */
    bool isTextBoxEditable() const throw()                                  { return editableText; }

    /** If the text-box is editable, this will give it the focus so that the user can
        type directly into it.

        This is basically the effect as the user clicking on it.
    */
    void showTextBox();

    /** If the text-box currently has focus and is being edited, this resets it and takes keyboard
        focus away from it.

        @param discardCurrentEditorContents     if true, the slider's value will be left
                                                unchanged; if false, the current contents of the
                                                text editor will be used to set the slider position
                                                before it is hidden.
    */
    void hideTextBox (const bool discardCurrentEditorContents);

    /** Changes the slider's current value.

        This will trigger a callback to SliderListener::sliderValueChanged() for any listeners
        that are registered, and will synchronously call the valueChanged() method in case subclasses
        want to handle it.

        @param newValue                 the new value to set - this will be restricted by the
                                        minimum and maximum range, and will be snapped to the
                                        nearest interval if one has been set
        @param sendUpdateMessage        if false, a change to the value will not trigger a call to
                                        any SliderListeners or the valueChanged() method
        @param sendMessageSynchronously if true, then a call to the SliderListeners will be made
                                        synchronously; if false, it will be asynchronous
    */
    void setValue (double newValue,
                   const bool sendUpdateMessage = true,
                   const bool sendMessageSynchronously = false);

    /** Returns the slider's current value. */
    double getValue() const throw();

    /** Sets the limits that the slider's value can take.

        @param newMinimum   the lowest value allowed
        @param newMaximum   the highest value allowed
        @param newInterval  the steps in which the value is allowed to increase - if this
                            is not zero, the value will always be (newMinimum + (newInterval * an integer)).
    */
    void setRange (const double newMinimum,
                   const double newMaximum,
                   const double newInterval = 0);

    /** Returns the current maximum value.
        @see setRange
    */
    double getMaximum() const throw()                                       { return maximum; }

    /** Returns the current minimum value.
        @see setRange
    */
    double getMinimum() const throw()                                       { return minimum; }

    /** Returns the current step-size for values.
        @see setRange
    */
    double getInterval() const throw()                                      { return interval; }

    /** For a slider with two or three thumbs, this returns the lower of its values.

        For a two-value slider, the values are controlled with getMinValue() and getMaxValue().
        A slider with three values also uses the normal getValue() and setValue() methods to
        control the middle value.

        @see setMinValue, getMaxValue, TwoValueHorizontal, TwoValueVertical, ThreeValueHorizontal, ThreeValueVertical
    */
    double getMinValue() const throw();

    /** For a slider with two or three thumbs, this sets the lower of its values.

        This will trigger a callback to SliderListener::sliderValueChanged() for any listeners
        that are registered, and will synchronously call the valueChanged() method in case subclasses
        want to handle it.

        @param newValue                 the new value to set - this will be restricted by the
                                        minimum and maximum range, and the max value (in a two-value
                                        slider) or the mid value (in a three-value slider), and
                                        will be snapped to the nearest interval if one has been set.
        @param sendUpdateMessage        if false, a change to the value will not trigger a call to
                                        any SliderListeners or the valueChanged() method
        @param sendMessageSynchronously if true, then a call to the SliderListeners will be made
                                        synchronously; if false, it will be asynchronous
        @see getMinValue, setMaxValue, setValue
    */
    void setMinValue (double newValue,
                      const bool sendUpdateMessage = true,
                      const bool sendMessageSynchronously = false);

    /** For a slider with two or three thumbs, this returns the higher of its values.

        For a two-value slider, the values are controlled with getMinValue() and getMaxValue().
        A slider with three values also uses the normal getValue() and setValue() methods to
        control the middle value.

        @see getMinValue, TwoValueHorizontal, TwoValueVertical, ThreeValueHorizontal, ThreeValueVertical
    */
    double getMaxValue() const throw();

    /** For a slider with two or three thumbs, this sets the lower of its values.

        This will trigger a callback to SliderListener::sliderValueChanged() for any listeners
        that are registered, and will synchronously call the valueChanged() method in case subclasses
        want to handle it.

        @param newValue                 the new value to set - this will be restricted by the
                                        minimum and maximum range, and the max value (in a two-value
                                        slider) or the mid value (in a three-value slider), and
                                        will be snapped to the nearest interval if one has been set.
        @param sendUpdateMessage        if false, a change to the value will not trigger a call to
                                        any SliderListeners or the valueChanged() method
        @param sendMessageSynchronously if true, then a call to the SliderListeners will be made
                                        synchronously; if false, it will be asynchronous
        @see getMaxValue, setMinValue, setValue
    */
    void setMaxValue (double newValue,
                      const bool sendUpdateMessage = true,
                      const bool sendMessageSynchronously = false);

    /** Adds a listener to be called when this slider's value changes. */
    void addListener (ASliderListener* const listener) throw();

    /** Removes a previously-registered listener. */
    void removeListener (ASliderListener* const listener) throw();

    /** This lets you choose whether double-clicking moves the slider to a given position.

        By default this is turned off, but it's handy if you want a double-click to act
        as a quick way of resetting a slider. Just pass in the value you want it to
        go to when double-clicked.

        @see getDoubleClickReturnValue
    */
    void setDoubleClickReturnValue (const bool isDoubleClickEnabled,
                                    const double valueToSetOnDoubleClick) throw();

    /** Returns the values last set by setDoubleClickReturnValue() method.

        Sets isEnabled to true if double-click is enabled, and returns the value
        that was set.

        @see setDoubleClickReturnValue
    */
    double getDoubleClickReturnValue (bool& isEnabled) const throw();

    /** Tells the slider whether to keep sending change messages while the user
        is dragging the slider.

        If set to true, a change message will only be sent when the user has
        dragged the slider and let go. If set to false (the default), then messages
        will be continuously sent as they drag it while the mouse button is still
        held down.
    */
    void setChangeNotificationOnlyOnRelease (const bool onlyNotifyOnRelease) throw();

    /** This lets you change whether the slider thumb jumps to the mouse position
        when you click.

        By default, this is true. If it's false, then the slider moves with relative
        motion when you drag it.

        This only applies to linear bars, and won't affect two- or three- value
        sliders.
    */
    void setSliderSnapsToMousePosition (const bool shouldSnapToMouse) throw();

    /** If enabled, this gives the slider a pop-up bubble which appears while the
        slider is being dragged.

        This can be handy if your slider doesn't have a text-box, so that users can
        see the value just when they're changing it.

        If you pass a component as the parentComponentToUse parameter, the pop-up
        bubble will be added as a child of that component when it's needed. If you
        pass 0, the pop-up will be placed on the desktop instead (note that it's a
        transparent window, so if you're using an OS that can't do transparent windows
        you'll have to add it to a parent component instead).
    */
    void setPopupDisplayEnabled (const bool isEnabled,
                                 Component* const parentComponentToUse) throw();

    /** If this is set to true, then right-clicking on the slider will pop-up
        a menu to let the user change the way it works.

        By default this is turned off, but when turned on, the menu will include
        things like velocity sensitivity, and for rotary sliders, whether they
        use a linear or rotary mouse-drag to move them.
    */
    void setPopupMenuEnabled (const bool menuEnabled) throw();

    /** This can be used to stop the mouse scroll-wheel from moving the slider.

        By default it's enabled.
    */
    void setScrollWheelEnabled (const bool enabled) throw();

    /** Callback to indicate that the user is about to start dragging the slider.

        @see SliderListener::sliderDragStarted
    */
    virtual void startedDragging();

    /** Callback to indicate that the user has just stopped dragging the slider.

        @see SliderListener::sliderDragEnded
    */
    virtual void stoppedDragging();

    /** Callback to indicate that the user has just moved the slider.

        @see SliderListener::sliderValueChanged
    */
    virtual void valueChanged();

    /** Callback to indicate that the user has just moved the slider.
        Note - the valueChanged() method has changed its format and now no longer has
        any parameters. Update your code to use the new version.
        This version has been left here with an int as its return value to cause
        a syntax error if you've got existing code that uses the old version.
    */
    virtual int valueChanged (double) { jassertfalse; return 0; }

    /** Subclasses can override this to convert a text string to a value.

        When the user enters something into the text-entry box, this method is
        called to convert it to a value.

        The default routine just tries to convert it to a double.

        @see getTextFromValue
    */
    virtual double getValueFromText (const String& text);

    /** Turns the slider's current value into a text string.

        Subclasses can override this to customise the formatting of the text-entry box.

        The default implementation just turns the value into a string, using
        a number of decimal places based on the range interval. If a suffix string
        has been set using setTextValueSuffix(), this will be appended to the text.

        @see getValueFromText
    */
    virtual const String getTextFromValue (double value);

    /** Sets a suffix to append to the end of the numeric value when it's displayed as
        a string.

        This is used by the default implementation of getTextFromValue(), and is just
        appended to the numeric value. For more advanced formatting, you can override
        getTextFromValue() and do something else.
    */
    void setTextValueSuffix (const String& suffix);

    /** Allows a user-defined mapping of distance along the slider to its value.

        The default implementation for this performs the skewing operation that
        can be set up in the setSkewFactor() method. Override it if you need
        some kind of custom mapping instead, but make sure you also implement the
        inverse function in valueToProportionOfLength().

        @param proportion       a value 0 to 1.0, indicating a distance along the slider
        @returns                the slider value that is represented by this position
        @see valueToProportionOfLength
    */
    virtual double proportionOfLengthToValue (double proportion);

    /** Allows a user-defined mapping of value to the position of the slider along its length.

        The default implementation for this performs the skewing operation that
        can be set up in the setSkewFactor() method. Override it if you need
        some kind of custom mapping instead, but make sure you also implement the
        inverse function in proportionOfLengthToValue().

        @param value            a valid slider value, between the range of values specified in
                                setRange()
        @returns                a value 0 to 1.0 indicating the distance along the slider that
                                represents this value
        @see proportionOfLengthToValue
    */
    virtual double valueToProportionOfLength (double value);

    /** Returns the X or Y coordinate of a value along the slider's length.

        If the slider is horizontal, this will be the X coordinate of the given
        value, relative to the left of the slider. If it's vertical, then this will
        be the Y coordinate, relative to the top of the slider.

        If the slider is rotary, this will throw an assertion and return 0. If the
        value is out-of-range, it will be constrained to the length of the slider.
    */
    float getPositionOfValue (const double value);

    /** This can be overridden to allow the slider to snap to user-definable values.

        If overridden, it will be called when the user tries to move the slider to
        a given position, and allows a subclass to sanity-check this value, possibly
        returning a different value to use instead.

        @param attemptedValue       the value the user is trying to enter
        @param userIsDragging       true if the user is dragging with the mouse; false if
                                    they are entering the value using the text box
        @returns                    the value to use instead
    */
    virtual double snapValue (double attemptedValue, const bool userIsDragging);

    /** This can be called to force the text box to update its contents.

        (Not normally needed, as this is done automatically).
    */
    void updateText();

    void setParameter(Parameter* param);
    Parameter* getParameter();

    void setText(const String& newText,  const bool broadcastChangeMessage);

    void setTextColour (Colour& newcolour);

    /** True if the slider moves horizontally. */
    bool isHorizontal() const throw();
    /** True if the slider moves vertically. */
    bool isVertical() const throw();
    /** Get index of the bound parameter. */
    int getParamIndex();
    /** Set index of the bound parameter. */
    void setParamIndex(int index);
    /** Get index of the bound parameter. */
    bool getSigned();
    /** Set index of the bound parameter. */
    void setSigned(bool sgn);

    void handleMessage (const Message& message);


    void postParamMessage (Parameter* param) throw();


    /** A set of colour IDs to use to change the colour of various aspects of the slider.

        These constants can be used either via the Component::setColour(), or LookAndFeel::setColour()
        methods.

        @see Component::setColour, Component::findColour, LookAndFeel::setColour, LookAndFeel::findColour
    */
    enum ColourIds
    {
        backgroundColourId          = 0x1001200,  /**< A colour to use to fill the slider's background. */
        thumbColourId               = 0x1001300,  /**< The colour to draw the thumb with. It's up to the look
                                                       and feel class how this is used. */
        trackColourId               = 0x1001310,  /**< The colour to draw the groove that the thumb moves along. */
        rotarySliderFillColourId    = 0x1001311,  /**< For rotary sliders, this colour fills the outer curve. */
        rotarySliderOutlineColourId = 0x1001312,  /**< For rotary sliders, this colour is used to draw the outer curve's outline. */

        textBoxTextColourId         = 0x1001400,  /**< The colour for the text in the text-editor box used for editing the value. */
        textBoxBackgroundColourId   = 0x1001500,  /**< The background colour for the text-editor box. */
        textBoxHighlightColourId    = 0x1001600,  /**< The text highlight colour for the text-editor box. */
        textBoxOutlineColourId      = 0x1001700   /**< The colour to use for a border around the text-editor box. */
    };

    juce_UseDebuggingNewOperator

protected:
    /** @internal */
    void labelTextChanged (ALabel*);
    /** @internal */
    void paint (Graphics& g);
    /** @internal */
    void resized();
    /** @internal */
    void mouseDown (const MouseEvent& e);
    /** @internal */
    void mouseUp (const MouseEvent& e);
    /** @internal */
    void mouseDrag (const MouseEvent& e);
    /** @internal */
    void mouseDoubleClick (const MouseEvent& e);
    /** @internal */
    void mouseWheelMove (const MouseEvent& e, float wheelIncrementX, float wheelIncrementY);
    /** @internal */
    void modifierKeysChanged (const ModifierKeys& modifiers);
    /** @internal */
    void buttonClicked (Button* button);
    /** @internal */
    void lookAndFeelChanged();
    /** @internal */
    void enablementChanged();
    /** @internal */
    void focusOfChildComponentChanged (FocusChangeType cause);
    /** @internal */
    void handleAsyncUpdate();
    /** @internal */
    void colourChanged();

private:
    SortedSet <void*> listeners;
    double currentValue, valueMin, valueMax;
    double minimum, maximum, interval, doubleClickReturnValue;
    double valueWhenLastDragged, valueOnMouseDown, skewFactor, lastAngle;
    double velocityModeSensitivity, velocityModeOffset, minMaxDiff;
    int velocityModeThreshold;
    float rotaryStart, rotaryEnd;
    int numDecimalPlaces, mouseXWhenLastDragged, mouseYWhenLastDragged;
    int sliderRegionStart, sliderRegionSize;
    int sliderBeingDragged;
    int pixelsForFullDragExtent;
    Rectangle sliderRect;
    String textSuffix;
    int textBoxX, textBoxY;

    SliderStyle style;
    TextEntryBoxPosition textBoxPos;
    int textBoxWidth, textBoxHeight;
    IncDecButtonMode incDecButtonMode;

    bool editableText : 1, doubleClickToValue : 1;
    bool isVelocityBased : 1, userKeyOverridesVelocity : 1, rotaryStop : 1;
    bool incDecButtonsSideBySide : 1, sendChangeOnlyOnRelease : 1, popupDisplayEnabled : 1;
    bool menuEnabled : 1, menuShown : 1, mouseWasHidden : 1, incDecDragged : 1;
    bool scrollWheelEnabled : 1, snapsToMousePos : 1;
    Font font;
    ALabel* valueBox;
    Button* incButton;
    Button* decButton;
    Component* popupDisplay;
    Component* parentForPopupDisplay;
    int paramIndex;
    Parameter* param;
    bool is_signed;

    float getLinearSliderPos (const double value);
    void restoreMouseIfHidden();
    void sendDragStart();
    void sendDragEnd();
    double constrainedValue (double value) const throw();
    void triggerChangeMessage (const bool synchronous);
    bool incDecDragDirectionIsHorizontal() const throw();

    ASlider (const ASlider&);
    const ASlider& operator= (const ASlider&);
};

class ConfigComponent : public Component,
                        public ButtonListener,
                        public AComboBoxListener,
                        public ASliderListener
{
public:
    ATextButton* OKButt;
    ATextButton* EdButt;
    AGroupComponent* Group;
    AGroupComponent* Group1;
    AGroupComponent* Group2;

    //Slider to control audio buffer size
    ASlider* pBufferSlider;
    ALabel* buffLabel;

    //Toggles to choose sample rate
    AToggleButton *tb22, *tb44, *tb48, *tb96, *tb192;
    AToggleButton *tbLength;
    AToggleButton *tbBind;
    AToggleButton *tbOverlapping;
    AToggleButton *tbAutoRescan;

    AComboBox* int_comboBox;

    AComboBox* juceAudioDeviceDropDown;

    AComboBox* comboAudOutput;
    AComboBox* comboAudInput;
    AComboBox* comboMidiIn;
    ATextButton* menuButton;
    AMidiInputSelectorComponentListBox* midiInputsList;
    ALabel* midiInputsLabel;

    Image*  sunimg;

    ConfigComponent ();
    ~ConfigComponent();
    void    mouseDown(const MouseEvent& e);
    void    buttonClicked(Button* buttonThatWasClicked);
    void    resized();
    void    InitSampleRateList();

    //slider control functions
    void    sliderValueChanged (ASlider* slider);
    void    sliderDragEnded (ASlider* slider);
    void    comboBoxChanged (AComboBox* comboBoxThatHasChanged);
    void    UpdateComponentsVisibility();

    void    paint(Graphics& g);

private:

};

class ProgressTimer: public juce::Timer
{
public:
    ProgressTimer(MainComponent * pContent) { this->pContent = pContent; };
    ~ProgressTimer() {};
    void timerCallback();

private:
    MainComponent* pContent;
};

class RenderComponent : public Component,
                            public ButtonListener,
                            public AComboBoxListener,
                            public TextEditorListener
{
public:
    ATextButton* startButt;
    ATextButton* foldButt, *expButt, *resButt;
    AGroupComponent* settingsGroup;
    AGroupComponent* nameGroup;
    ASlider* slider;
    AComboBox* int_comboBox;
    ALabel* textFileStore;
    ALabel* textFolderStore;
    ALabel* formatLabel;
    ALabel* qLabel;
    ALabel* intLabel;
    Image*  waveimg;

    File    outdir;

    //These buttons are to choose between various output formats
    AToggleButton *tbWav, *tbOgg, *tbFlac;

    //These buttons control quality (quantization accuracy) for wav and flac
    AToggleButton *tb8Bit, *tb16Bit, *tb24Bit, *tb32Bit, *tb32BitF;

    //Vorbis encoding measures quality in range from 0.1 to 1 from worst to best
    AToggleButton *tb00, *tb01, *tb02, *tb03, *tb04, *tb05, *tb06, *tb07, *tb08, *tb09, *tb10;
    TextEditor* fileName;
    ProgressTimer *pSlideTimer;

    RenderComponent ();
    ~RenderComponent();
    void mouseDown(const MouseEvent& e);
    void    buttonClicked(Button* buttonThatWasClicked);
    void comboBoxChanged (AComboBox* comboBoxThatHasChanged);
    void textEditorReturnKeyPressed (TextEditor& editor);
    void textEditorTextChanged (TextEditor& editor);
    void textEditorEscapeKeyPressed (TextEditor& editor) {};
    void textEditorFocusLost (TextEditor& editor) {};
    void RenderComponent::SetSlider(unsigned int progress);

    void    paint(Graphics& g);
    void    resized();

private:
};

class PluginEditComponent : public Component,
                            public ButtonListener,
                            public AComboBoxListener
{
public:
    PluginEditComponent();
    ~PluginEditComponent();

    void buttonClicked(Button* buttonThatWasClicked) {};
    void comboBoxChanged (AComboBox* comboBoxThatHasChanged) {};

};

/********* End of inlined file: juce_ComboBox.h *********/

/**
    Manages the state of some audio and midi i/o devices.

    This class keeps tracks of a currently-selected audio device, through
    with which it continuously streams data from an audio callback, as well as
    one or more midi inputs.

    The idea is that your application will create one global instance of this object,
    and let it take care of creating and deleting specific types of audio devices
    internally. So when the device is changed, your callbacks will just keep running
    without having to worry about this.

    The manager can save and reload all of its device settings as XML, which
    makes it very easy for you to save and reload the audio setup of your
    application.

    And to make it easy to let the user change its settings, there's a component
    to do just that - the AudioDeviceSelectorComponent class, which contains a set of
    device selection/sample-rate/latency controls.

    To use an AudioDeviceManager, create one, and use initialise() to set it up. Then
    call addAudioCallback() to register your audio callback with it, and use that to process
    your audio data.

    The manager also acts as a handy hub for incoming midi messages, allowing a
    listener to register for messages from either a specific midi device, or from whatever
    the current default midi input device is. The listener then doesn't have to worry about
    re-registering with different midi devices if they are changed or deleted.

    And yet another neat trick is that amount of CPU time being used is measured and
    available with the getCpuUsage() method.

    The AudioDeviceManager is a ChangeBroadcaster, and will send a change message to
    listeners whenever one of its settings is changed.

    @see AudioDeviceSelectorComponent, AudioIODevice, AudioIODeviceType
*/
class JUCE_API  CAudioDeviceManager  : public ChangeBroadcaster
{
public:

    /** Creates a default AudioDeviceManager.

        Initially no audio device will be selected. You should call the initialise() method
        and register an audio callback with setAudioCallback() before it'll be able to
        actually make any noise.
    */
    CAudioDeviceManager();

    /** Destructor. */
    ~CAudioDeviceManager();

    /**
        This structure holds a set of properties describing the current audio setup.

        @see AudioDeviceManager::setAudioDeviceSetup()
    */
    struct JUCE_API  AudioDeviceSetup
    {
        AudioDeviceSetup();
        bool operator== (const AudioDeviceSetup& other) const;

        /** The name of the audio device used for output.
            This may be the same as the input device.
        */
        String outputDeviceName;

        /** The name of the audio device used for input.
            This may be the same as the output device.
        */
        String inputDeviceName;

        /** The current sample rate.
            This rate is used for both the input and output devices.
        */
        double sampleRate;

        /** The buffer size, in samples.
            This buffer size is used for both the input and output devices.
        */
        int bufferSize;

        /** The set of active input channels.
            The bits that are set in this array indicate the channels of the
            input device that are active.
        */
        BitArray inputChannels;

        /** If this is true, it indicates that the inputChannels array
            should be ignored, and instead, the device's default channels
            should be used.
        */
        bool useDefaultInputChannels;

        /** The set of active output channels.
            The bits that are set in this array indicate the channels of the
            input device that are active.
        */
        BitArray outputChannels;

        /** If this is true, it indicates that the outputChannels array
            should be ignored, and instead, the device's default channels
            should be used.
        */
        bool useDefaultOutputChannels;
    };

    /** Opens a set of audio devices ready for use.

        This will attempt to open either a default audio device, or one that was
        previously saved as XML.

        @param numInputChannelsNeeded       a minimum number of input channels needed
                                            by your app.
        @param numOutputChannelsNeeded      a minimum number of output channels to open
        @param savedState                   either a previously-saved state that was produced
                                            by createStateXml(), or 0 if you want the manager
                                            to choose the best device to open.
        @param selectDefaultDeviceOnFailure if true, then if the device specified in the XML
                                            fails to open, then a default device will be used
                                            instead. If false, then on failure, no device is
                                            opened.
        @param preferredDefaultDeviceName   if this is not empty, and there's a device with this
                                            name, then that will be used as the default device
                                            (assuming that there wasn't one specified in the XML).
                                            The string can actually be a simple wildcard, containing "*"
                                            and "?" characters
        @param preferredSetupOptions        if this is non-null, the structure will be used as the
                                            set of preferred settings when opening the device. If you
                                            use this parameter, the preferredDefaultDeviceName
                                            field will be ignored

        @returns an error message if anything went wrong, or an empty string if it worked ok.
    */
    const String initialise (const int numInputChannelsNeeded,
                             const int numOutputChannelsNeeded,
                             const XmlElement* const savedState,
                             const bool selectDefaultDeviceOnFailure,
                             const String& preferredDefaultDeviceName = String::empty,
                             const AudioDeviceSetup* preferredSetupOptions = 0);

    /** Returns some XML representing the current state of the manager.

        This stores the current device, its samplerate, block size, etc, and
        can be restored later with initialise().
    */
    XmlElement* createStateXml() const;

    /** Returns the current device properties that are in use.

        @see setAudioDeviceSetup
    */
    void getAudioDeviceSetup (AudioDeviceSetup& setup);

    /** Changes the current device or its settings.

        If you want to change a device property, like the current sample rate or
        block size, you can call getAudioDeviceSetup() to retrieve the current
        settings, then tweak the appropriate fields in the AudioDeviceSetup structure,
        and pass it back into this method to apply the new settings.

        @param newSetup             the settings that you'd like to use
        @param treatAsChosenDevice  if this is true and if the device opens correctly, these new
                                    settings will be taken as having been explicitly chosen by the
                                    user, and the next time createStateXml() is called, these settings
                                    will be returned. If it's false, then the device is treated as a
                                    temporary or default device, and a call to createStateXml() will
                                    return either the last settings that were made with treatAsChosenDevice
                                    as true, or the last XML settings that were passed into initialise().
        @returns an error message if anything went wrong, or an empty string if it worked ok.

        @see getAudioDeviceSetup
    */
    const String setAudioDeviceSetup (const AudioDeviceSetup& newSetup,
                                      const bool treatAsChosenDevice);

    /** Returns the currently-active audio device. */
    AudioIODevice* getCurrentAudioDevice() const throw()                { return currentAudioDevice; }

    /** Returns the type of audio device currently in use.
        @see setCurrentAudioDeviceType
    */
    const String getCurrentAudioDeviceType() const throw()              { return currentDeviceType; }

    /** Changes the class of audio device being used.

        This switches between, e.g. ASIO and DirectSound. On the Mac you probably won't ever call
        this because there's only one type: CoreAudio.

        For a list of types, see getAvailableDeviceTypes().
    */
    void setCurrentAudioDeviceType (const String& type,
                                    const bool treatAsChosenDevice);

    /** Closes the currently-open device.

        You can call restartLastAudioDevice() later to reopen it in the same state
        that it was just in.
    */
    void closeAudioDevice();

    /** Tries to reload the last audio device that was running.

        Note that this only reloads the last device that was running before
        closeAudioDevice() was called - it doesn't reload any kind of saved-state,
        and can only be called after a device has been opened with SetAudioDevice().

        If a device is already open, this call will do nothing.
    */
    void restartLastAudioDevice();

    /** Registers an audio callback to be used.

        The manager will redirect callbacks from whatever audio device is currently
        in use to all registered callback objects. If more than one callback is
        active, they will all be given the same input data, and their outputs will
        be summed.

        If necessary, this method will invoke audioDeviceAboutToStart() on the callback
        object before returning.

        To remove a callback, use removeAudioCallback().
    */
    void addAudioCallback (AudioIODeviceCallback* newCallback);

    /** Deregisters a previously added callback.

        If necessary, this method will invoke audioDeviceStopped() on the callback
        object before returning.

        @see addAudioCallback
    */
    void removeAudioCallback (AudioIODeviceCallback* callback);

    /** Returns the average proportion of available CPU being spent inside the audio callbacks.

        Returns a value between 0 and 1.0
    */
    double getCpuUsage() const;

    /** Enables or disables a midi input device.

        The list of devices can be obtained with the MidiInput::getDevices() method.

        Any incoming messages from enabled input devices will be forwarded on to all the
        listeners that have been registered with the addMidiInputCallback() method. They
        can either register for messages from a particular device, or from just the
        "default" midi input.

        Routing the midi input via an AudioDeviceManager means that when a listener
        registers for the default midi input, this default device can be changed by the
        manager without the listeners having to know about it or re-register.

        It also means that a listener can stay registered for a midi input that is disabled
        or not present, so that when the input is re-enabled, the listener will start
        receiving messages again.

        @see addMidiInputCallback, isMidiInputEnabled
    */
    void setMidiInputEnabled (const String& midiInputDeviceName,
                              const bool enabled);

    /** Returns true if a given midi input device is being used.

        @see setMidiInputEnabled
    */
    bool isMidiInputEnabled (const String& midiInputDeviceName) const;

    /** Registers a listener for callbacks when midi events arrive from a midi input.

        The device name can be empty to indicate that it wants events from whatever the
        current "default" device is. Or it can be the name of one of the midi input devices
        (see MidiInput::getDevices() for the names).

        Only devices which are enabled (see the setMidiInputEnabled() method) will have their
        events forwarded on to listeners.
    */
    void addMidiInputCallback (const String& midiInputDeviceName,
                               MidiInputCallback* callback);

    /** Removes a listener that was previously registered with addMidiInputCallback().
    */
    void removeMidiInputCallback (const String& midiInputDeviceName,
                                  MidiInputCallback* callback);

    /** Sets a midi output device to use as the default.

        The list of devices can be obtained with the MidiOutput::getDevices() method.

        The specified device will be opened automatically and can be retrieved with the
        getDefaultMidiOutput() method.

        Pass in an empty string to deselect all devices. For the default device, you
        can use MidiOutput::getDevices() [MidiOutput::getDefaultDeviceIndex()].

        @see getDefaultMidiOutput, getDefaultMidiOutputName
    */
    void setDefaultMidiOutput (const String& deviceName);

    /** Returns the name of the default midi output.

        @see setDefaultMidiOutput, getDefaultMidiOutput
    */
    const String getDefaultMidiOutputName() const throw()           { return defaultMidiOutputName; }

    /** Returns the current default midi output device.

        If no device has been selected, or the device can't be opened, this will
        return 0.

        @see getDefaultMidiOutputName
    */
    MidiOutput* getDefaultMidiOutput() const throw()                { return defaultMidiOutput; }

    /** Just adds the list of device names to a combo box.

        The only reason this is in this class is so that it can divide DSound
        and ASIO devices into labelled sections, which makes it look much neater.
    */
    void addDeviceNamesToComboBox (AComboBox& combo) const;

    /** Returns a list of the types of device supported.
    */
    const OwnedArray <AudioIODeviceType>& getAvailableDeviceTypes();

    /** Creates a list of available types.

        This will add a set of new AudioIODeviceType objects to the specified list, to
        represent each available types of device.

        You can override this if your app needs to do something specific, like avoid
        using DirectSound devices, etc.
    */
    virtual void createAudioDeviceTypes (OwnedArray <AudioIODeviceType>& types);

    /** Plays a beep through the current audio device.

        This is here to allow the audio setup UI panels to easily include a "test"
        button so that the user can check where the audio is coming from.
    */
    void playTestSound();

    /** Turns on level-measuring.

        When enabled, the device manager will measure the peak input level
        across all channels, and you can get this level by calling getCurrentInputLevel().

        This is mainly intended for audio setup UI panels to use to create a mic
        level display, so that the user can check that they've selected the right
        device.

        A simple filter is used to make the level decay smoothly, but this is
        only intended for giving rough feedback, and not for any kind of accurate
        measurement.
    */
    void enableInputLevelMeasurement (const bool enableMeasurement);

    /** Returns the current input level.

        To use this, you must first enable it by calling enableInputLevelMeasurement().

        See enableInputLevelMeasurement() for more info.
    */
    double getCurrentInputLevel() const;

	AudioIODeviceType* getDeviceTypeObjectByDeviceName(const String& devName);

    juce_UseDebuggingNewOperator

private:

    OwnedArray <AudioIODeviceType> availableDeviceTypes;
    OwnedArray <AudioDeviceSetup> lastDeviceTypeConfigs;

    AudioDeviceSetup currentSetup;
    AudioIODevice* currentAudioDevice;
    SortedSet <AudioIODeviceCallback*> callbacks;
    int numInputChansNeeded, numOutputChansNeeded;
    String currentDeviceType;
    BitArray inputChannels, outputChannels;
    XmlElement* lastExplicitSettings;
    mutable bool listNeedsScanning;
    bool useInputNames;
    int inputLevelMeasurementEnabledCount;
    double inputLevel;
    AudioSampleBuffer* testSound;
    int testSoundPosition;
    AudioSampleBuffer tempBuffer;

    StringArray midiInsFromXml;
    OwnedArray <MidiInput> enabledMidiInputs;
    Array <MidiInputCallback*> midiCallbacks;
    Array <MidiInput*> midiCallbackDevices;
    String defaultMidiOutputName;
    MidiOutput* defaultMidiOutput;
    CriticalSection audioCallbackLock, midiCallbackLock;

    double cpuUsageMs, timeToCpuScale;

    class CallbackHandler  : public AudioIODeviceCallback,
                             public MidiInputCallback
    {
    public:
        CAudioDeviceManager* owner;

        void audioDeviceIOCallback (const float** inputChannelData,
                                    int totalNumInputChannels,
                                    float** outputChannelData,
                                    int totalNumOutputChannels,
                                    int numSamples);

        void audioDeviceAboutToStart (AudioIODevice*);

        void audioDeviceStopped();

        void handleIncomingMidiMessage (MidiInput* source, const MidiMessage& message);
    };

    CallbackHandler callbackHandler;
    friend class CallbackHandler;

    void audioDeviceIOCallbackInt (const float** inputChannelData,
                                   int totalNumInputChannels,
                                   float** outputChannelData,
                                   int totalNumOutputChannels,
                                   int numSamples);
    void audioDeviceAboutToStartInt (AudioIODevice* const device);
    void audioDeviceStoppedInt();

    void handleIncomingMidiMessageInt (MidiInput* source, const MidiMessage& message);

    const String restartDevice (int blockSizeToUse, double sampleRateToUse,
                                const BitArray& ins, const BitArray& outs);
    void stopDevice();

    void updateXml();

    void createDeviceTypesIfNeeded();
    void scanDevicesIfNeeded();
    void deleteCurrentDevice();
    double chooseBestSampleRate (double preferred) const;
    AudioIODeviceType* getCurrentDeviceTypeObject() const;

    void insertDefaultDeviceNames (AudioDeviceSetup& setup) const;

    AudioIODeviceType* findType (const String& inputName, const String& outputName);

    CAudioDeviceManager (const CAudioDeviceManager&);
    const CAudioDeviceManager& operator= (const CAudioDeviceManager&);
};

#endif   // __JUCER_HEADER_MAINCOMPONENT_MAINCOMPONENT_D0F6CD31__
