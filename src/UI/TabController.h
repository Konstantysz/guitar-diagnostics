#pragma once

#include "UI/Panel.h"

#include <memory>
#include <vector>

namespace GuitarDiagnostics::UI
{

    /**
     * @brief Manages navigation between different diagnostic panels.
     *
     * Handles rendering of the main tab bar and switching active panels.
     */
    class TabController
    {
    public:
        /**
         * @brief Constructs the TabController with all sub-panels.
         * @param fretBuzzPanel Panel for Fret Buzz analysis.
         * @param intonationPanel Panel for Intonation analysis.
         * @param stringHealthPanel Panel for String Health analysis.
         * @param audioMonitorPanel Panel for raw audio monitoring.
         */
        TabController(std::unique_ptr<Panel> fretBuzzPanel,
            std::unique_ptr<Panel> intonationPanel,
            std::unique_ptr<Panel> stringHealthPanel,
            std::unique_ptr<Panel> audioMonitorPanel);

        /**
         * @brief Destructor.
         */
        ~TabController();

        TabController(const TabController &) = delete;

        TabController &operator=(const TabController &) = delete;

        TabController(TabController &&) = delete;

        TabController &operator=(TabController &&) = delete;

        /**
         * @brief Initializes all managed panels.
         */
        void OnAttach();

        /**
         * @brief Shuts down all managed panels.
         */
        void OnDetach();

        /**
         * @brief Updates the currently active panel.
         * @param deltaTime Time elapsed since the last frame.
         */
        void OnUpdate(float deltaTime);

        /**
         * @brief Renders the tab bar and the active panel.
         */
        void Render();

    private:
        std::vector<std::unique_ptr<Panel>> panels; ///< Collection of managed panels.
        int activeTabIndex;                         ///< Index of the currently active tab.
    };

} // namespace GuitarDiagnostics::UI
