#ifndef LIBRARY_H
#define LIBRARY_H

#include "Windows.h"
#include <filesystem>
#include <memory>
#include <string>

namespace APIReplay
{
	class Library final
	{
	public:
		Library();
		explicit Library(std::filesystem::path libraryPath);

		~Library();
		Library(Library const&) = delete;
		Library& operator=(Library const&) = delete;
		Library(Library&&) noexcept;
		Library& operator=(Library&&) noexcept;

	private:
		HMODULE m_module;
		std::filesystem::path m_libraryPath;
	};

	class PlatformException : public std::exception
	{
	public:
		DWORD errorCode;

		PlatformException(DWORD errorCode)
			: errorCode{ errorCode }
		{ }

		char const* what() const override
		{
			if (m_message == nullptr)
			{
				auto message = std::string{ "Windows error " } + std::to_string(errorCode);
				m_message = std::make_unique<std::string>(message);
			}

			return m_message->c_str();
		}

	private:
		mutable std::unique_ptr<std::string> m_message = nullptr;
	};
}

#endif // LIBRARY_H