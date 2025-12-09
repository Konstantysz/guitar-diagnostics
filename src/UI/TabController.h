#pragma once

#include "UI/Panel.h"

#include <memory>
#include <vector>

namespace GuitarDiagnostics::UI
{

    class TabController
    {
    public:
        TabController(std::unique_ptr<Panel> fretBuzzPanel,
            std::unique_ptr<Panel> intonationPanel,
            std::unique_ptr<Panel> stringHealthPanel,
            std::unique_ptr<Panel> audioMonitorPanel);
        ~TabController();

        TabController(const TabController &) = delete;
        TabController &operator=(const TabController &) = delete;
        TabController(TabController &&) = delete;
        TabController &operator=(TabController &&) = delete;

        void OnAttach();
        void OnDetach();
        void OnUpdate(float deltaTime);
        void Render();

    private:
        std::vector<std::unique_ptr<Panel>> panels;
        int activeTabIndex;
    };

} // namespace GuitarDiagnostics::UI
