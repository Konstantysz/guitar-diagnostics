#pragma once

#include <string>

namespace GuitarDiagnostics::UI
{

    class Panel
    {
    public:
        virtual ~Panel() = default;

        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;
        virtual void OnUpdate(float deltaTime) = 0;
        virtual void OnImGuiRender() = 0;

        virtual const std::string &GetName() const = 0;
        virtual bool IsActive() const = 0;
        virtual void SetActive(bool active) = 0;

    protected:
        Panel() = default;
        Panel(const Panel &) = default;
        Panel(Panel &&) noexcept = default;
        Panel &operator=(const Panel &) = default;
        Panel &operator=(Panel &&) noexcept = default;
    };

} // namespace GuitarDiagnostics::UI
