#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
const int N = 100 + 1;
int adjMat[N][N];
int d[N];
vector<int> prev_(N, -1);
int n;
struct Edge
{
    int a, b, cost;
    Edge(int A,int  B, int Cost)
        :a(A)
        ,b(B)
        ,cost(Cost)
    {}
};

vector<Edge> edges;

bool relax(Edge& edge)
{
    if (d[edge.a] + edge.cost < d[edge.b])
    {
        d[edge.b] = d[edge.a] + edge.cost;
        prev_[edge.b] = edge.a;
        return  true;
    }
    return false;
}

vector<int> FordBellman()
{
    bool relaxed = false;
    for (int i = 0; i < n - 1; ++i)
    {
        for (auto& edge: edges)
        {
            if (relax(edge))
                relaxed = true;
        }
        if (!relaxed)
            return vector<int>();
    }
    relaxed = false;
    for (auto& edge: edges)
    {
        if (relax(edge))
        {
            relaxed = true;
            int v = edge.a;
            for (int i = 0; i < n; ++i)
                v = prev_[v];
            vector<int> r_cycle;
            r_cycle.push_back(v);
            int u = prev_[v];
            while (u != v)
            {
                r_cycle.push_back(u);
                u = prev_[u];
            }
            return r_cycle;
        }
    }
    return vector<int>();
}


int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    std::cout.tie(0);
    cin >> n;
    for (int i = 1; i <=n; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            int cost;
            cin >> cost;
            if (cost != (int)1e5)
                edges.emplace_back(i, j, cost);
        }
    }
    if (vector r_cycle = FordBellman(); !r_cycle.empty())
    {
        cout << "YES\n" << r_cycle.size() << "\n";
        for (auto r_it  = r_cycle.rbegin(); r_it != r_cycle.rend(); ++r_it)
        {
            cout << *r_it << " ";
        }

    }
    else
        cout << "NO\n";

    return 0;
}
