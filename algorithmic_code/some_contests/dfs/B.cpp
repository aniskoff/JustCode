#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <vector>

const int N = (int)1e4 + 1;
std::unordered_set<int> childrenG[N];
std::unordered_set<int> childrenGR[N];
int color[N];
std::vector<int> sortedByToutGR;
bool visitedGR[N];
bool cycleFound = false;
std::unordered_map<int, std::unordered_set<int>> metaEdges;
int ans = 0;
int col = 1;


void go(int v)
{
    visitedGR[v] = true;
    for (auto c: childrenGR[v])
    {
        if (!visitedGR[c])
            go(c);
    }
    sortedByToutGR.push_back(v);
}

void dfs(int v)
{
    color[v] = col;
    for (auto c: childrenG[v])
    {
        if (color[c] == 0)
            dfs(c);
        if (color[v] != color[c])
        {
            if (metaEdges[color[v]].find(color[c]) == metaEdges[color[v]].end())
            {
                metaEdges[color[v]].insert(color[c]);
                ++ans;
            }
        }
    }
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
        childrenG[v1].insert(v2);
        childrenGR[v2].insert(v1);
    }
    for (int v = 1; v <= n; ++v)
    {
        if (!visitedGR[v])
            go(v);
    }
    for (auto i = sortedByToutGR.rbegin(); i != sortedByToutGR.rend(); ++i)
    {
        if (color[*i] == 0)
        {
            dfs(*i);
            ++col;

        }
    }
    std::cout << ans << "\n";
    return 0;
}
