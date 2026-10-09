#include "parentheses-tree_benchmark.hpp"

#include <chrono>
#include <vector>
#include <string>

double run_rmmq_benchmark(ParenthesesTreeRMMQ &PTRMMQ, int order, string operation, vector<ULL>& indexes)
{
    chrono::high_resolution_clock::time_point start;
    chrono::high_resolution_clock::time_point end;

    if (operation == "enclose")
    {
        start = chrono::high_resolution_clock::now();
        for(ULL i : indexes)
        {
            PTRMMQ.enclose(i);
        }
        end = chrono::high_resolution_clock::now();
    }
    else if (operation == "close")
    {
        start = chrono::high_resolution_clock::now();
        for (ULL i : indexes){
            PTRMMQ.close(i);
        }
        end = chrono::high_resolution_clock::now();
    }
    
    std::chrono::duration<double, nano> elapsed_time{end - start};

    return (elapsed_time.count()/indexes.size());
}

double run_naive_benchmark(ParenthesesTree& PT, int order, string operation, vector<ULL>& indexes)
{
    chrono::high_resolution_clock::time_point start;
    chrono::high_resolution_clock::time_point end;

    if (operation == "enclose") 
    {
        start = chrono::high_resolution_clock::now();
        for(ULL i : indexes)
        {
            PT.enclose(i);
        }
        end = chrono::high_resolution_clock::now();
    }
    else if (operation == "close")
    {
        start = chrono::high_resolution_clock::now();
        for (ULL i : indexes){                
            PT.close(i);
        }
        end = chrono::high_resolution_clock::now();
    }

    std::chrono::duration<double, nano> elapsed_time{end - start};

    return (elapsed_time.count()/indexes.size());
}