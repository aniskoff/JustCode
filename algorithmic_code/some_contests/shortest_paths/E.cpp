#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>
using namespace std;
const int INF = (int)1e7 + 1;
const int N = (int)1e2 + 1;
int d[N][N];
bool visited[N];
int n;
unordered_set<int> reachable_from[N];
vector<int> children[N];
int ans[N][N];

void Floyd()
{
    for (int k = 1; k <= n; ++k)
    {
        for (int i = 1; i <= n; ++i)
        {
            for (int j = 1; j <= n; ++j)
            {
                if (d[i][k] != INF && d[k][j] != INF)
                    d[i][j] = max(-INF, min(d[i][j], d[i][k] + d[k][j]));
            }
        }
    }
}

void dfs(int source, int v)
{
    visited[v] = true;
    reachable_from[source].insert(v);
    for (auto& c: children[v])
    {
        if (!visited[c])
            dfs(source, c);
    }
}

int main()
{
    std::ios_base::sync_with_stdio(false);std::cin.tie(0);std::cout.tie(0);
    cin >> n;
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            int w;
            cin >> w;
            if (w == 0)
                d[i][j] = INF;
            else
            {
                d[i][j] = w;
                children[i].push_back(j);
            }
        }
    }
    for (int i = 1; i <= n; ++i)
        d[i][i] = min(0, d[i][i]);

    Floyd();
    for (int v = 1; v <= n; ++v)
    {
        fill (visited, visited + n  + 1, false);
        dfs(v, v);
    }
    for (int i = 1; i <= n; ++i)
        fill(ans[i], ans[i] + n + 1, 1);

    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            if (d[i][j] == INF)
                ans[i][j] = 0;

            for (auto& t: reachable_from[i])
            {
                if (d[t][t] < 0)
                {
                    if (reachable_from[t].find(j) != reachable_from[t].end())
                    {
                        ans[i][j] = 2;
                        break;
                    }
                }
            }
        }
    }
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= n; ++j)
            cout << ans[i][j] << " ";
        cout << "\n";
    }

    return  0;
}
