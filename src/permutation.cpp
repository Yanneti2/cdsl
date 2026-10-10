#include "bitvector.hpp"
#include "permutation.hpp"
#include <vector>

// t is a parameter that can be used to control max number of steps to find the
// inverse of permutation. To ensure this, every t steps along cycles of length > t
// will introduce a shortcut that maps i to inverse permutation of i.
// If the cycle lenght is not a multiple of t, then a final shortcut, shorted than t
// is added to enforce the limit of t steps.
Permutation::Permutation(std::vector<int> perm, size_t t)
{
    size_t size = perm.size();
    BitVector visited(size);

    for (size_t i = 0; i < size; i++)
    {
        visited.append0();
        shortcuts.append0();
    }

    // Building tau (a permutation made with in a way that all cycles are one 
    // after another.
    for (size_t i = 0; i < size; i++)
    {
        // Linear scan
        if (visited[i])
        {
            continue;
        }

        size_t j = i;

        // Following the cycle
        while (!visited[j])
        {
            visited.set1(j);
            permutation.push_back(j);
            cycles.append0();

            j = perm[j];
        }
        cycles.set1(permutation.size() - 1);
    }

    // Reset visited for the second pass
    visited = BitVector(size);
    for (size_t i = 0; i < size; i++)
    {
        visited.append0();
    }

    // Building the shortcuts based on tau
    std::vector<std::pair<size_t, size_t>> links;
    for (size_t i = 0; i < size; i++)
    {
        // Linear scan
        if (visited[i])
        {
            continue;
        }

        size_t j = i;
        size_t cycle_length = 1;
        std::vector<size_t> marked;

        // Following the cycle
        while (!visited[j])
        {
            visited.set1(j);
            if (j == i || cycle_length % t == 0)
            {
                shortcuts.set1(j);
                marked.push_back(j);
            }

            j = permutation[j];
            cycle_length++;
        }

        for (size_t i = 0; i < marked.size(); i++)
        {
            int prev = marked[(i + marked.size() - 1) % marked.size()];
            links.push_back({marked[i], prev});
        }
    }


    shortcuts_endpoints.assign(links.size(), 0);

    for (auto &[element, prev]: links)
    {
        shortcuts_endpoints[shortcuts.naive_rank1(element)] = prev;
    }
}

// returns PI(i)
int Permutation::operator()(int i)
{
    return power(i,1);
}

// returns PI^n(i) = PI(PI(...PI(i)...))
int Permutation::power(int i, int n)
{
    size_t j = tau_inverse(i);

    size_t r = cycles.naive_rank1(j);
    size_t p = cycles.naive_select1(r);
    size_t s = cycles.naive_select1(r + 1);
    long long len = s - p;


    long long off = (((long long)(j - p) + n) % len + len ) % len;

    return permutation[p + off];

}

int Permutation::inverse(int i)
{
    return power(i, -1);
}

// Returns PI^-1(i)
int Permutation::tau_inverse(int i)
{
    size_t j = i;
    bool s = true;

    while (permutation[j] != i)
    {
        if (s and shortcuts[j])
        {
            j = shortcuts_endpoints[shortcuts.naive_rank1(j)];
            s = false;
        }
        else
        {
            j = permutation[j];
        }
    }
    return j;
}


size_t Permutation::size()
{
    return permutation.size();
}
