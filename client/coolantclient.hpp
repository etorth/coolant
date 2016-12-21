/*
 * =====================================================================================
 *
 *       Filename: coolantclient.hpp
 *        Created: 12/20/2016 19:56:15
 *  Last Modified: 12/20/2016 21:47:17
 *
 *    Description: main entry here
 *
 *        Version: 1.0
 *       Revision: none
 *       Compiler: gcc
 *
 *         Author: ANHONG
 *          Email: anhonghe@gmail.com
 *   Organization: USTC
 *
 * =====================================================================================
 */

#pragma once

#include "Sample.h"
#include <Urho3D/UI/Window.h>

class CoolAntClient : public Sample
{
    private:
        Urho3D::SharedPtr<Urho3D::Window>       m_Window;
        Urho3D::SharedPtr<Urho3D::UIElement>    m_UIRoot;
        Urho3D::IntVector2                      m_DragPos0;

    public:
        URHO3D_OBJECT(CoolAntClient, Sample);

    public:
        CoolAntClient(Urho3D::Context* );

    public:
        virtual void Start();

    protected:
        virtual Urho3D::String GetScreenJoystickPatchString() const
        {
            return
                "<patch>"
                "    <add sel=\"/element/element[./attribute[@name='Name' and @value='Hat0']]\">"
                "        <attribute name=\"Is Visible\" value=\"false\" />"
                "    </add>"
                "</patch>";
        }

    private:
        void InitWindow();
        void InitControls();
        void CreateDraggableFish();
        void HandleDragBegin(Urho3D::StringHash, Urho3D::VariantMap&);
        void HandleDragMove(Urho3D::StringHash, Urho3D::VariantMap&);
        void HandleDragEnd(Urho3D::StringHash, Urho3D::VariantMap&);
        void HandleControlClicked(Urho3D::StringHash, Urho3D::VariantMap&);
        void HandleClosePressed(Urho3D::StringHash, Urho3D::VariantMap&);

    private:
        void InitMouseMode(Urho3D::MouseMode mode);
};
