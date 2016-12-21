#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/Cursor.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/ToolTip.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>

#include "coolantclient.hpp"

#include <Urho3D/DebugNew.h>
// URHO3D_DEFINE_APPLICATION_MAIN(CoolAntClient)

CoolAntClient::CoolAntClient(Urho3D::Context* pContext)
    : Sample(pContext)
    , m_UIRoot(GetSubsystem<UI>()->GetRoot())
    , m_DragPos0(IntVector2::ZERO)
{
}

void CoolAntClient::Start()
{
    if(GetPlatform() == "Android" || GetPlatform() == "iOS"){
        InitTouchInput();
    }else if(GetSubsystem<Input>()->GetNumJoysticks() == 0){
        // On desktop platform, do not detect touch when we already got a joystick
        SubscribeToEvent(E_TOUCHBEGIN, URHO3D_HANDLER(Sample, HandleTouchBegin));
    }

    CreateLogo();
    SetWindowTitleAndIcon();
    CreateConsoleAndDebugHud();

    SubscribeToEvent(Urho3D::E_KEYDOWN, URHO3D_HANDLER(Sample, HandleKeyDown));
    SubscribeToEvent(Urho3D::E_KEYUP, URHO3D_HANDLER(Sample, HandleKeyUp));
    SubscribeToEvent(Urho3D::E_SCENEUPDATE, URHO3D_HANDLER(Sample, HandleSceneUpdate));

    GetSubsystem<Input>()->SetMouseVisible(true);

    auto pCache = GetSubsystem<ResourceCache>();
    auto pStyle = pCache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    m_UIRoot->SetDefaultStyle(pStyle);

    InitWindow();
    InitControls();
    CreateDraggableFish();
    InitMouseMode(Urho3D::MM_FREE);
}

void CoolAntClient::InitControls()
{
    // Create a CheckBox
    CheckBox* checkBox = new CheckBox(context_);
    checkBox->SetName("CheckBox");

    // Create a Button
    Button* button = new Button(context_);
    button->SetName("Button");
    button->SetMinHeight(24);

    // Create a LineEdit
    LineEdit* lineEdit = new LineEdit(context_);
    lineEdit->SetName("LineEdit");
    lineEdit->SetMinHeight(24);

    // Add controls to Window
    m_Window->AddChild(checkBox);
    m_Window->AddChild(button);
    m_Window->AddChild(lineEdit);

    // Apply previously set default style
    checkBox->SetStyleAuto();
    button->SetStyleAuto();
    lineEdit->SetStyleAuto();
}

void CoolAntClient::InitWindow()
{
    // Create the Window and add it to the UI's root node
    m_Window = new Window(context_);
    m_UIRoot->AddChild(m_Window);

    // Set Window size and layout settings
    m_Window->SetMinWidth(384);
    m_Window->SetLayout(LM_VERTICAL, 6, IntRect(6, 6, 6, 6));
    m_Window->SetAlignment(HA_CENTER, VA_CENTER);
    m_Window->SetName("Window");

    // Create Window 'titlebar' container
    UIElement* titleBar = new UIElement(context_);
    titleBar->SetMinSize(0, 24);
    titleBar->SetVerticalAlignment(VA_TOP);
    titleBar->SetLayoutMode(LM_HORIZONTAL);

    // Create the Window title Text
    Text* windowTitle = new Text(context_);
    windowTitle->SetName("WindowTitle");
    windowTitle->SetText("Hello GUI!");

    // Create the Window's close button
    Button* buttonClose = new Button(context_);
    buttonClose->SetName("CloseButton");

    // Add the controls to the title bar
    titleBar->AddChild(windowTitle);
    titleBar->AddChild(buttonClose);

    // Add the title bar to the Window
    m_Window->AddChild(titleBar);

    // Apply styles
    m_Window->SetStyleAuto();
    windowTitle->SetStyleAuto();
    buttonClose->SetStyle("CloseButton");

    // Subscribe to buttonClose release (following a 'press') events
    SubscribeToEvent(buttonClose, E_RELEASED, URHO3D_HANDLER(CoolAntClient, HandleClosePressed));

    // Subscribe also to all UI mouse clicks just to see where we have clicked
    SubscribeToEvent(E_UIMOUSECLICK, URHO3D_HANDLER(CoolAntClient, HandleControlClicked));
}

void CoolAntClient::CreateDraggableFish()
{
    ResourceCache* pCache = GetSubsystem<ResourceCache>();
    Graphics* graphics = GetSubsystem<Graphics>();

    // Create a draggable Fish button
    Button* draggableFish = new Button(context_);
    draggableFish->SetTexture(pCache->GetResource<Texture2D>("Textures/UrhoDecal.dds")); // Set texture
    draggableFish->SetBlendMode(BLEND_ADD);
    draggableFish->SetSize(128, 128);
    draggableFish->SetPosition((graphics->GetWidth() - draggableFish->GetWidth()) / 2, 200);
    draggableFish->SetName("Fish");
    m_UIRoot->AddChild(draggableFish);

    // Add a tooltip to Fish button
    ToolTip* toolTip = new ToolTip(context_);
    draggableFish->AddChild(toolTip);
    toolTip->SetPosition(IntVector2(draggableFish->GetWidth() + 5, draggableFish->GetWidth() / 2)); // slightly offset from close button
    BorderImage* textHolder = new BorderImage(context_);
    toolTip->AddChild(textHolder);
    textHolder->SetStyle("ToolTipBorderImage");
    Text* toolTipText = new Text(context_);
    textHolder->AddChild(toolTipText);
    toolTipText->SetStyle("ToolTipText");
    toolTipText->SetText("Please drag me!");

    // Subscribe draggableFish to Drag Events (in order to make it draggable)
    // See "Event list" in documentation's Main Page for reference on available Events and their eventData
    SubscribeToEvent(draggableFish, E_DRAGBEGIN, URHO3D_HANDLER(CoolAntClient, HandleDragBegin));
    SubscribeToEvent(draggableFish, E_DRAGMOVE, URHO3D_HANDLER(CoolAntClient, HandleDragMove));
    SubscribeToEvent(draggableFish, E_DRAGEND, URHO3D_HANDLER(CoolAntClient, HandleDragEnd));
}

void CoolAntClient::HandleDragBegin(StringHash eventType, VariantMap& eventData)
{
    // Get UIElement relative position where input (touch or click) occurred (top-left = IntVector2(0,0))
    m_DragPos0 = IntVector2(eventData["ElementX"].GetInt(), eventData["ElementY"].GetInt());
}

void CoolAntClient::HandleDragMove(StringHash eventType, VariantMap& eventData)
{
    IntVector2 dragCurrentPosition = IntVector2(eventData["X"].GetInt(), eventData["Y"].GetInt());
    UIElement* draggedElement = static_cast<UIElement*>(eventData["Element"].GetPtr());
    draggedElement->SetPosition(dragCurrentPosition - m_DragPos0);
}

void CoolAntClient::HandleDragEnd(StringHash eventType, VariantMap& eventData) // For reference (not used here)
{
}

void CoolAntClient::HandleClosePressed(StringHash eventType, VariantMap& eventData)
{
    if (GetPlatform() != "Web")
        engine_->Exit();
}

void CoolAntClient::HandleControlClicked(StringHash eventType, VariantMap& eventData)
{
    // Get the Text control acting as the Window's title
    Text* windowTitle = m_Window->GetChildStaticCast<Text>("WindowTitle", true);

    // Get control that was clicked
    UIElement* clicked = static_cast<UIElement*>(eventData[UIMouseClick::P_ELEMENT].GetPtr());

    String name = "...?";
    if (clicked)
    {
        // Get the name of the control that was clicked
        name = clicked->GetName();
    }

    // Update the Window's title text
    windowTitle->SetText("Hello " + name + "!");
}

void CoolAntClient::InitMouseMode(MouseMode mode)
{
    useMouseMode_ = mode;

    Input* input = GetSubsystem<Input>();

    if (GetPlatform() != "Web")
    {
        if (useMouseMode_ == MM_FREE)
            input->SetMouseVisible(true);

        Console* console = GetSubsystem<Console>();
        if (useMouseMode_ != MM_ABSOLUTE)
        {
            input->SetMouseMode(useMouseMode_);
            if (console && console->IsVisible())
                input->SetMouseMode(MM_ABSOLUTE, true);
        }
    }
    else
    {
        input->SetMouseVisible(true);
        SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(Sample, HandleMouseModeRequest));
        SubscribeToEvent(E_MOUSEMODECHANGED, URHO3D_HANDLER(Sample, HandleMouseModeChange));
    }
}
