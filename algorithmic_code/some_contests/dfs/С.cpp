#include <iostream>
#include <vector>
#include <forward_list>
#define forn(i,n) for(int i = 0; i < n; ++i)
using namespace std;
const int N = (int)5e5 + 1;
bool visited1[N];
bool visited2[N];
forward_list<int> children[N];
int dist = 0;
int max_dist = 0;
int most_distant = 0;


pair<int, int> most_distant_with_dfs(int v, bool visited[])
{

    visited[v] = true;
    if (dist > max_dist)
    {
        max_dist = dist;
        most_distant = v;
    }
    for (auto c: children[v])
    {
        if (!visited[c])
        {
            ++dist;
            most_distant_with_dfs(c, visited);
        }
    }
    --dist;
    return {most_distant, max_dist};
}


int main()
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(0); std::cout.tie(0);
    int n, m;
    cin >> n >> m;
    int a, b;
    forn(i, n - 1)
    {
        cin >> a >> b;
        children[a].push_front(b);
        children[b].push_front(a);
    }
    int w = most_distant_with_dfs(a, visited1).first;
    dist = max_dist = most_distant = 0;
    int diameter = most_distant_with_dfs(w, visited2).second;
    int ans = (diameter >= m) ? (m + 1) : ( diameter + (m - diameter) / 2  + 1);
    if (ans > n)
        ans = n;
    cout << ans << endl;
}
