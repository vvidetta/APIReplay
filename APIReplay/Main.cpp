// (c) Copyright  Vito Videtta  2018 - 2019

#include <iostream>
#include <string>
#include <Windows.h>
#include "Library.h"
#include <map>

using namespace std;

FARPROC get_function_pointer
(
    string const& library_name,
    string const& procedure_name,
    int&          error_status
)
{
    error_status = 0;

    HMODULE myDLL = LoadLibrary(library_name.c_str());
    if (myDLL == NULL)
    {
        error_status = GetLastError();
        cout << "Error " << error_status << " occurred when attempting to load library " << library_name << endl;
        return nullptr;
    }

    FARPROC p_function = GetProcAddress(myDLL, procedure_name.c_str());
    if (p_function == nullptr)
    {
        error_status = GetLastError();
        cout << "Error " << error_status << " occurred when attempting to get address of procedure \"" << procedure_name << '"' << endl;
        return nullptr;
    }
    return p_function;
}

void invoke_DLL_function
(
    string const& library_name,
    string const& procedure_name,
    int&          error_status
)
{
    FARPROC p_function = get_function_pointer(library_name, procedure_name, error_status);
    if (p_function == nullptr)
    {
        return;
    }

    __asm
    {
        call p_function
    }
}

int invoke_int_DLL_function
(
    string const& library_name,
    string const& procedure_name,
    int&          error_status
)
{
    FARPROC p_function = get_function_pointer(library_name, procedure_name, error_status);
    if (p_function == nullptr)
    {
        return 0;
    }

    int return_value = 0;
    __asm
    {
        call p_function
        mov dword ptr [return_value], eax
    }
    return return_value;
}

int invoke_int_function_with_params
(
    string const& library_name,
    string const& procedure_name,
    int&          error_status,
    void* const   p_param,
    size_t const  param_len
)
{
    FARPROC p_function = get_function_pointer(library_name, procedure_name, error_status);
    if (p_function == nullptr)
    {
        return 0;
    }

    int return_value = 0;
    __asm
    {
        // push params onto stack
        push ecx                      // save ecx
        mov ecx, dword ptr[param_len] // Copy param_len bytes
        mov esi, dword ptr[p_param]   // Copy from the param pointer
        sub esp, ecx                  // Allocate stack memory for params
        mov edi, esp                  // Copy to the allocated block
        rep movsb                     // copy memory onto stack

        // Call the function
        call p_function

        add esp, dword ptr[param_len] // Free stack memory
        mov dword ptr[return_value], eax // store return value
        pop ecx                       // restore ecx
    }
    return return_value;
}

/* void* invoke_struct_function
(
    string const& library_name,
    string const& procedure_name,
    int&          error_status,
    void* const   p_param,
    size_t const  param_len,
    size_t const  struct_size
)
{
    FARPROC p_function = get_function_pointer(library_name, procedure_name, error_status);
    if (p_function == nullptr)
    {
        return 0;
    }

    int return_value = 0;
    __asm
    {
        // push params onto stack
        push ecx                      // save ecx
        mov ecx, dword ptr[param_len] // Copy param_len bytes
        mov esi, dword ptr[p_param]   // Copy from the param pointer
        sub esp, ecx                  // Allocate stack memory for params
        mov edi, esp                  // Copy to the allocated block
        rep movsb                     // copy memory onto stack

        // Call the function
        call p_function

        add esp, dword ptr[param_len] // Free stack memory
        mov dword ptr[return_value], eax // store return value
        pop ecx                       // restore ecx
    }
    return;
} */

int main
(
    int argc,
    char* argv[]
)
{
    std::cout << "APIReplay Console for Windows" << std::endl
        << "(c) Copyright  Vito Videtta  2018 - 2019" << std::endl
        << std::endl;

    auto libraries = std::map<std::filesystem::path, APIReplay::Library>{};
    while (true)
    {
        std::cout << ">";
        auto command = std::string{};
        std::getline(std::cin, command);

        if (command.empty())
        {
            continue;
        }

        if (command == "quit")
        {
            break;
        }
        if (command.substr(0, 4) == "open")
        {
            auto const libraryPath = std::filesystem::path{ command.substr(5) };
            try
            {
                libraries[libraryPath] = APIReplay::Library{ libraryPath };
            }
            catch (std::exception & ex)
            {
                std::cout << "Failed to open library" << std::endl
                    << ex.what() << std::endl;
            }
            catch (...)
            {
                std::cout << "Failed to open library" << std::endl;
            }
        }
        else if (command.substr(0, 5) == "close")
        {
            auto const libraryPath = command.substr(6);
            auto const it = libraries.find(libraryPath);
            if (it == libraries.end())
            {
                std::cout << "Library not found" << std::endl;
            }
            else
            {
                libraries.erase(it);
                std::cout << "Library closed" << std::endl;
            }
        }
        else
        {
            std::cout << "Unrecognised command" << std::endl;
        }

        std::cout << std::endl;
    }

    return 0;
}

