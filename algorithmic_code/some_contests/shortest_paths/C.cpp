#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
using namespace std;
using ull = unsigned long long;
const ull INF = UINT64_MAX;
const int N = 300 + 1;
vector<int> children[N];
int cost[N][N];
vector<int> prev_(N, -1);
vector<ull> d(N, INF);
bool processed[N];
struct Vertex
{
    int v;
    ull prior;
    Vertex(int V, ull Prior)
        :v(V)
        ,prior(Prior)
    {}
    bool operator<(const Vertex& other) const
    {
        return prior > other.prior;
    }
};

void Dijkstra(int v)
{
    fill(d.begin(), d.end(), INF);
    fill(prev_.begin(), prev_.end(), -1);
    fill(processed, processed + N, false);
    d[v] = 0;
    priority_queue<Vertex> Q;
    Q.emplace(v, 0);
    while (!Q.empty())
    {
        int u = Q.top().v;
        Q.pop();
        processed[u] = true;
        for (auto& c : children[u])
        {
            if (!processed[c])
            {
                if (d[u] + cost[u][c] < d[c])
                {
                    d[c] = d[u] + cost[u][c];
                    prev_[c] = u;
                    Q.emplace(c, d[c]);
                }
            }
        }
    }
}
int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    std::cout.tie(0);
    int n, m;
    cin >> n >> m;

    while (m--) {
        int a, b, c;
        cin >> a >> b >> c;
        children[a].push_back(b);
        children[b].push_back(a);
        cost[a][b] = cost[b][a] = c;
    }

    Dijkstra(1);
    ull sh_path_len = d[n];
    vector<int> path;
    for (int v = n; v != 1; v = prev_[v])
        path.push_back(v);
    path.push_back(1);
    ull res = 0;
    for (int i = 0; i != path.size() - 1; ++i) {
        cost[path[i]][path[i + 1]] = cost[path[i + 1]][path[i]] *= 2;
        Dijkstra(1);
        res = (d[n] - sh_path_len > res) ? (d[n] - sh_path_len) : (res);
        cost[path[i]][path[i + 1]] = cost[path[i + 1]][path[i]] /= 2;
    }
    cout << res;
    return  0;
}

