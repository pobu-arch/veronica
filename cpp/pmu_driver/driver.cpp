#include <iostream>
#include "veronica.h"
#include "pmu_list.h"

using namespace std;

int main()
{
    init_pmu_list();

    for(auto it = global_pmu_list.begin(); it != global_pmu_list.end(); it++)
    {
        cout << it->gen_perf_subcmd() << endl;
    }
}