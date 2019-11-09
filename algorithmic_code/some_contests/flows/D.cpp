#include <iostream>
#include <vector>
#include <climits>
#include <deque>
#include <cmath>

using namespace std;
using ull = unsigned long long;

int n, m, U;
ull marker;
const int INF = INT_MAX;

struct Edge
{
    int from, to;
    int next;
    int f;
    int cap;
};

vector<Edge> edges;
vector<int> head;
vector<int> head_geo;
vector<ull> visited;
vector<int> d;

inline int ilog2(int v) // such that ilog2(0) == 0
{
    const size_t b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
    const size_t S[] = {1, 2, 4, 8, 16};

    size_t r = 0;
    for (int i = 4; i >= 0; --i)
    {
        if (v & b[i]) {
            v >>= S[i];
            r |= S[i];
        }
    }
    return r;
}

inline int power_of_2(int i)
{
    int res = 1;
    return res << i;
}

bool bfs(int source, int dest, int scale)
{
    d[source] = 0; visited[source] = marker;
    deque<int> deq{source};
    while (!deq.empty())
    {
        int v = deq.front(); deq.pop_front();
        for (int i = head[v]; i != -1; i = edges[i].next)
        {
            Edge& e = edges[i];
            if (visited[e.to] != marker && e.f < e.cap / power_of_2(scale))
            {
                d[e.to] = d[e.from] + 1;
                deq.push_back(e.to);
                visited[e.to] = marker;
            }
        }
    }
    return visited[dest] == marker;
}

int dfs(int x, int flow, int scale)
{
    if (x == n || flow == 0)
        return flow;
    for (int i = head_geo[x]; i != -1; i = edges[i].next)
    {
        Edge& e = edges[i];
        int add = 0;
        if (d[e.to] == d[e.from] + 1 )
        {
            if (e.f < (e.cap / power_of_2(scale)) &&
            (add = dfs(e.to, min((e.cap / power_of_2(scale)  - e.f), flow), scale)) > 0)
            {
                e.f += add;
                edges[i ^ 1].f -= add;
                return add;
            }
        }
        head_geo[x] = e.next;
    }
    return 0;
}

ull dinic(int scale)
{
    ull add = 0;
    ull res = 0;
    while(bfs(1, n, scale))
    {
        head_geo = head;
        add = dfs(1, INF, scale);
        while (add)
        {
            res += add;
            add = dfs(1, INF, scale);
        }
        ++marker;
    }
    return res;
}

int main()
{
    cin >> n >> m;
    edges.reserve(2 * m);
    head = vector<int>(n + 1, -1);
    d = vector<int>(n + 1, INF);
    int max_cap = 0;
    for (int i = 0; i < m; ++i) {
        int a, b, c;
        cin >> a >> b >> c;
        edges.push_back({a, b, head[a], 0, c});
        head[a] = edges.size() - 1;
        edges.push_back({b, a, head[b], 0, 0});
        head[b] = edges.size() - 1;
        max_cap = max(c, max_cap);
    }
    U = ilog2(max_cap);
    marker = 1;
    visited = vector<ull>(n + 1, 0);

    ull res_flow = 0;
    for (int scale = U; scale >= 0; --scale)
        res_flow += dinic(scale);

    cout << res_flow << "\n";
    for (int i = 0; i < 2 * m; i += 2)
        cout << edges[i].f << "\n";

}
