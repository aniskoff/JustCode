#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <stack>
#include <unordered_set>

using namespace std;
using pii = pair<int, int>;
const int N = (int)1e5 + 1;
const int INF = INT32_MAX;
vector<int> children[N];
int d[N];
int nearest[N];
void bfs(int n, set<int>&& exits)
{
    fill(d, d + n + 1, INF);
    queue<int> Q;
    for (auto& exit: exits)
    {
        Q.push(exit);
        d[exit] = 0;
        nearest[exit] = exit;
    }
    while (!Q.empty())
    {
        auto w = Q.front();
        Q.pop();
        for (auto& c: children[w])
        {
            if (d[c] == INF)
            {
                d[c] = d[w] + 1;
                nearest[c] = nearest[w];
                Q.push(c);
            }
        }
    }
}

int main()
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(0); std::cout.tie(0);
    int n, k;
    cin >> n >> k;
    set<int> exits;
    while(k--)
    {
        int exit;
        cin >> exit;
        exits.insert(exit);
    }
    int m;
    cin >> m;
    while(m--)
    {
        int a, b;
        cin >> a >> b;
        children[a].insert(b);
        children[b].insert(a);
    }
    bfs(n, move(exits));
    for (int i = 1; i <= n; ++i)
        cout << d[i] << " ";
    cout << "\n";
    for (int i = 1; i <= n; ++i)
        cout << nearest[i] << " ";


    return 0;
}