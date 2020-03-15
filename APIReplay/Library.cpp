#include "Library.h"

namespace APIReplay
{
    Library::Library()
        : m_module{ NULL },
        m_libraryPath{ }
    {}

    Library::Library(std::filesystem::path libraryPath)
        : m_module{ NULL },
        m_libraryPath{ libraryPath }
    {
        auto hModule = LoadLibrary(libraryPath.string().c_str());
        if (hModule == NULL)
        {
            auto const errorCode = GetLastError();
            throw PlatformException{ errorCode };
        }

        m_module = hModule;
    }

    Library::~Library()
    {
        if (m_module != NULL)
        {
            FreeLibrary(m_module);
        }
    }

    Library::Library(Library&& other) noexcept
        : m_module{ other.m_module },
        m_libraryPath{ other.m_libraryPath }
    {}

    Library& Library::operator=(Library&& other) noexcept
    {
        m_module = NULL;
        m_libraryPath = std::filesystem::path{};
        std::swap(m_module, other.m_module);
        std::swap(m_libraryPath, other.m_libraryPath);
        return *this;
    }
}
