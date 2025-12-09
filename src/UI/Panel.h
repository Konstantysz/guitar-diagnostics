#pragma once

#include <string>

namespace GuitarDiagnostics::UI
{

    /**
     * @brief Abstract base class for UI panels.
     *
     * Defines the interface for all pluggable UI components managed by the TabController.
     */
    class Panel
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~Panel() = default;

        /**
         * @brief Called when the panel is first attached to the view.
         */
        virtual void OnAttach() = 0;

        /**
         * @brief Called when the panel is detached.
         */
        virtual void OnDetach() = 0;

        /**
         * @brief Called every frame to update panel logic.
         * @param deltaTime Time elapsed since the last frame.
         */
        virtual void OnUpdate(float deltaTime) = 0;

        /**
         * @brief Called to render the panel's ImGui elements.
         */
        virtual void OnImGuiRender() = 0;

        /**
         * @brief Retrieves the display name of the panel.
         * @return Name string.
         */
        virtual const std::string &GetName() const = 0;

        /**
         * @brief Checks if the panel is currently active/visible.
         * @return True if active, false otherwise.
         */
        virtual bool IsActive() const = 0;

        /**
         * @brief Sets the active state of the panel.
         * @param active True to make active, false to hide.
         */
        virtual void SetActive(bool active) = 0;

    protected:
        Panel() = default;

        Panel(const Panel &) = default;

        Panel(Panel &&) noexcept = default;

        Panel &operator=(const Panel &) = default;

        Panel &operator=(Panel &&) noexcept = default;
    };

} // namespace GuitarDiagnostics::UI
