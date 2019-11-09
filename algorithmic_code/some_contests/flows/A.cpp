#include <iostream>
#include <vector>
#include <climits>

using namespace std;

int n, m, marker;
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
vector<int> visited;

int dfs(int x, int flow)
{
    if (x == n)
        return flow;
    visited[x] = marker;
    for (int i = head[x]; i != -1; i = edges[i].next)
    {
        Edge& e = edges[i];
        int add = 0;
        if (visited[e.to] != marker && e.f < e.cap && (add = dfs(e.to, min(e.cap - e.f, flow))) > 0)
        {
            e.f += add;
            edges[i ^ 1].f -= add;
            return add;
        }
    }
    return 0;
}


int main()
{
    cin >> n >> m;
    edges.reserve(2 * m);
    head = vector<int>(n + 1, -1);

    for (int i = 0; i < m; ++i)
    {
        int a, b, c;
        cin >> a >> b >> c;
        edges.push_back({a, b, head[a], 0, c});
        head[a] = edges.size() - 1;
        edges.push_back({b, a, head[b], 0, c});
        head[b] = edges.size() - 1;
    }

    marker = 1;
    visited = vector<int>(n, 0);

    int res_flow = 0, add = 0;
    do
    {
        add = dfs(1, INF);
        res_flow += add;
        ++marker;
    } while (add > 0);

    cout << res_flow << "\n";
    for (int i = 0; i < 2 * m; i += 2)
    {
        int edge_flow = edges[i].f > 0 ? edges[i].f : -edges[i ^ 1].f;
        cout << edge_flow << "\n";
    }

    return 0;
}