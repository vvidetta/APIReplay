// (c) Copyright  Vito Videtta  2018

#include <iostream>
#include <string>
#include <Windows.h>

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

    void* stack_pointer = nullptr;
    __asm
    {
        mov dword ptr [stack_pointer], esp
        sub esp, dword ptr[param_len]
    }

    void* param_start = (char*)stack_pointer - param_len;
    memcpy_s(param_start, param_len, p_param, param_len);

    int return_value = 0;
    __asm
    {
        call p_function
        add esp, 4
        mov dword ptr[return_value], eax
    }
    return return_value;
}


int main
(
    int argc,
    char* argv[]
)
{
    string const library_name = "SampleLibrary.dll";
    string const procedure_name = "SampleIntFunctionWithParam";

    int error = 0;
    int param = 75;
    int return_value = invoke_int_function_with_params(library_name, procedure_name, error, &param, sizeof(int));
    if (error == 0)
    {
        cout << "Returned value was " << return_value << endl;
    }

    cout << "Press any key to continue..." << endl;
    cin.ignore();
    return error;
}

