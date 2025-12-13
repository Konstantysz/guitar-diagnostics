#include "UI/TabController.h"

#include <imgui.h>

namespace GuitarDiagnostics::UI
{

    TabController::TabController(std::unique_ptr<Panel> fretBuzzPanel,
        std::unique_ptr<Panel> intonationPanel,
        std::unique_ptr<Panel> stringHealthPanel,
        std::unique_ptr<Panel> audioMonitorPanel)
        : panels(), activeTabIndex(0)
    {
        panels.reserve(4);
        panels.push_back(std::move(fretBuzzPanel));
        panels.push_back(std::move(intonationPanel));
        panels.push_back(std::move(stringHealthPanel));
        panels.push_back(std::move(audioMonitorPanel));
    }

    TabController::~TabController()
    {
    }

    void TabController::OnAttach()
    {
        for (auto &panel : panels)
        {
            panel->OnAttach();
        }

        if (!panels.empty())
        {
            panels[0]->SetActive(true);
        }
    }

    void TabController::OnDetach()
    {
        for (auto &panel : panels)
        {
            panel->OnDetach();
        }
    }

    void TabController::OnUpdate(float deltaTime)
    {
        for (auto &panel : panels)
        {
            panel->OnUpdate(deltaTime);
        }
    }

    void TabController::Render()
    {
        // Offset Y position by 50px for AudioStatusBar header
        constexpr float kHeaderHeight = 50.0f;
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        ImGui::SetNextWindowPos(ImVec2(0, kHeaderHeight));
        ImGui::SetNextWindowSize(ImVec2(displaySize.x, displaySize.y - kHeaderHeight));

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                                       | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;

        if (ImGui::Begin("Guitar Diagnostic Analyzer", nullptr, windowFlags))
        {
            if (ImGui::BeginTabBar("DiagnosticTabs", ImGuiTabBarFlags_None))
            {
                for (size_t i = 0; i < panels.size(); ++i)
                {
                    const std::string &tabName = panels[i]->GetName();

                    if (ImGui::BeginTabItem(tabName.c_str()))
                    {
                        activeTabIndex = static_cast<int>(i);
                        panels[i]->SetActive(true);
                        panels[i]->OnImGuiRender();
                        ImGui::EndTabItem();
                    }
                    else
                    {
                        panels[i]->SetActive(false);
                    }
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

} // namespace GuitarDiagnostics::UI
