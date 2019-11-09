#include <iostream>
#include <algorithm>
#include <stack>
#include <vector>
#define forn(i, n) for(int i=0;i<n;++i)


enum colors {white, grey, black};

const int N = (int)1e5 + 1;
std::forward_list<int> children[N];
colors color[N];
std::vector<int> path;
bool cycleFound = false;


inline void PrintCycle(int v)
{
    if(!cycleFound)
    {
        cycleFound = true;
        std::cout << "YES\n";
        auto cycleBeg = std::find(path.cbegin(), path.cend(), v);
        for (auto i = cycleBeg; i != path.cend(); ++i)
            std::cout << *i << " ";
    }
}
void dfs(int v)
{
    color[v] = grey;
    path.push_back(v);
    for (auto c: children[v])
    {
        if (color[c] == white)
            dfs(c);
        if (color[c] == grey)
            PrintCycle(c);
    }
    color[v] = black;
    path.pop_back();
}


int main()
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(0); std::cout.tie(0);
    int n, m;
    std::cin >> n >> m;
    while(m--)
    {
        int v1, v2;
        std::cin >> v1 >> v2;
        children[v1].push_front(v2);
    }
    for (int v = 1; v <= n; ++v)
    {
        if (color[v] != black)
            dfs(v);
    }
    if (!cycleFound)
        std::cout << "NO";

    return 0;
}