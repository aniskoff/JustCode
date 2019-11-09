#include <iostream>
#include <vector>
#include <climits>

using namespace std;

int n, m, marker, min_cut_size;
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
vector<bool> in_min_cut;


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
    in_min_cut = vector<bool>(2 * m, false);
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
    visited = vector<int>(n + 1, 0);

    int res_flow = 0, add = 0;
    do
    {
        add = dfs(1, INF);
        res_flow += add;
        ++marker;
    } while (add > 0);

    for (int v = 1; v < n + 1; ++v)
    {
        if (visited[v] == marker - 1)
        {
            for (int i = head[v]; i != -1; i = edges[i].next)
            {
                Edge &e = edges[i];
                if (visited[e.to] < marker - 1)
                {
                    in_min_cut[i] = true;
                    ++min_cut_size;
                }
            }
        }
    }

    cout << min_cut_size << " " << res_flow << "\n";

    for (int i = 0; i < m; ++i)
    {
        if (in_min_cut[2 * i] || in_min_cut[(2 * i) ^ 1])
            cout << (i + 1) << " ";
    }

    return 0;
}