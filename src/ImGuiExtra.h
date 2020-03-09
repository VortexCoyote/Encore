#pragma once

#include "imgui.h"

namespace ImGui
{
    bool BeginButtonDropDown(const char* label, ImVec2 buttonSize)
    {
        ImGui::SameLine(0.f, 0.f);


        const ImGuiStyle& style = ImGui::GetStyle();

        float x = ImGui::GetCursorPosX();
        float y = ImGui::GetCursorPosY();

        ImVec2 size(20, buttonSize.y);
        bool pressed = ImGui::Button("##", size);

        // Arrow
        ImVec2 center(ImGui::GetWindowPos().x + x + 10, ImGui::GetWindowPos().y + y + buttonSize.y / 2);
        float r = 8.f;
        center.y -= r * 0.25f;
        ImVec2 a = center + ImVec2(0 * r, 1 * r);
        ImVec2 b = center + ImVec2(-0.866f * r, -0.5f * r);
        ImVec2 c = center + ImVec2(0.866f * r, -0.5f * r);

        ImGui::GetWindowDrawList()->AddTriangleFilled(a, b, c, GetColorU32(ImGuiCol_Text));

        // Popup

        ImVec2 popupPos;

        popupPos.x = ImGui::GetWindowPos().x + x - buttonSize.x;
        popupPos.y = ImGui::GetWindowPos().y + y + buttonSize.y;

        ImGui::SetNextWindowPos(popupPos);

        if (pressed)
        {
            ImGui::OpenPopup(label);
        }

        if (ImGui::BeginPopup(label))
        {
            ImGui::PushStyleColor(ImGuiCol_FrameBg, style.Colors[ImGuiCol_Button]);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, style.Colors[ImGuiCol_Button]);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_Button]);
            return true;
        }

        return false;
    }

    void EndButtonDropDown()
    {
        ImGui::PopStyleColor(3);
        ImGui::EndPopup();
    }

}