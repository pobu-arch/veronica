#include "veronica.h"
#include <vector>
#include <string>
#include <iostream>

class pmu_counter
{
    public:
    std::string event_sel;
    std::string umask;
    std::string cmask;
    std::string name;
    std::string arch;

    pmu_counter(std::string &p1, std::string& p2, std::string &p3, std::string &p4, std::string &p5)
    {
        event_sel = p1;
        umask     = p2;
        cmask     = p3;
        name      = p4;
        arch      = p5;
    }

    string& gen_perf_subcmd()
    {
        // cpu/event=0xa8,umask=0x1,cmask=0x1,name=\'LSD.UOPS_CYCLES:cmask=0x1\'/ 
        return "cpu/event=" + event_sel +
               ",umask=" + umask +
               ",cmask=" + cmask +
               ",name=\\'" + name + "\\'//";
    }
};

std::vector<pmu_counter> global_pmu_list;

void init_pmu_list()
{
    global_pmu_list.clear();

    global_pmu_list.push_back(new pmu_counter("0xa8", "0x1", "0x1", "LSD.UOPS_CYCLES"));
}