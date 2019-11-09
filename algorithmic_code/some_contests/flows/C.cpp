#include <iostream>
#include <vector>
#include <climits>

using namespace std;


int n, m, s, t,marker;
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
pair<vector<int>, vector<int>> paths;


int dfs(int x, int flow)
{
    if (x == t)
    {
        return flow;
    }
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

void get_path(vector<int>& path, int x)
{
    if (x == t)
    {
        path.push_back(x);
        return;
    }
    visited[x] = marker;
    for (int i = head[x]; i != -1; i = edges[i].next)
    {
        Edge& e = edges[i];
        if (e.f == 1  && visited[e.to] != marker)
        {
            e.f--;
            path.push_back(e.from);
            if (x == s)
                head[s] = e.next;
            get_path(path, e.to);
            break;
        }
    }
}

int main()
{
    cin >> n >> m >> s >> t;
    edges.reserve(2 * m);
    head = vector<int>(n + 1, -1);

    for (int i = 0; i < m; ++i)
    {
        int a, b;
        cin >> a >> b;
        if (a != b)
        {
            edges.push_back({a, b, head[a], 0, 1});
            head[a] = edges.size() - 1;
            edges.push_back({b, a, head[b], 0, 0});
            head[b] = edges.size() - 1;
        }
    }

    marker = 1;
    visited = vector<int>(n + 1, 0);

    if ((dfs(s, INF)) == 0)
    {
        cout << "NO\n";
        return 0;
    }

    ++marker;

    if ((dfs(s, INF)) == 0)
    {
        cout << "NO\n";
        return 0;
    }

    ++marker;

    get_path(paths.first, s);

    ++marker;

    get_path(paths.second, s);


    cout << "YES\n";
    for (auto it = paths.first.begin(); it != paths.first.end(); ++it)
        cout << *it << " ";

    cout << "\n";
    for (auto it = paths.second.begin(); it != paths.second.end(); ++it)
        cout << *it << " ";

    return 0;
}