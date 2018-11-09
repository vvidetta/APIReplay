#include "SampleLibrary.h"
#include <iostream>

using namespace std;

extern "C" void SampleFunction()
{
    cout << "It worked!" << endl;
}

extern "C" int SampleIntFunction()
{
    return 42;
}

extern "C" int SampleIntFunctionWithParam
(
    int param
)
{
    return param;
}
