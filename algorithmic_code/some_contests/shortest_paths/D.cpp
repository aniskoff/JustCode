#include <iostream>
#include <vector>
using namespace std;
const int INF = (int)1e7 + 1;
const int N = (int)1e4 + 1;
const int K = 100 + 1;
int d[N][K];
bool processed[N][K];
struct Edge
{
    int a, b, cost;
    Edge(int A, int  B, int Cost)
            :a(A)
            ,b(B)
            ,cost(Cost)
    {}
};
vector<Edge> in_edges[N];

int D(int i, int j)
{
    if (processed[i][j])
        return d[i][j];
    for (auto& edge: in_edges[i])
    {
        int d_prev =  D(edge.a, j - 1);
        if (d_prev != INF)
        {
            if (d[i][j] > d_prev + edge.cost)
                d[i][j] = d_prev + edge.cost;
        }
    }
    processed[i][j] = true;
    return d[i][j];
}

int main()
{
    std::ios_base::sync_with_stdio(false);std::cin.tie(0);std::cout.tie(0);
    int n, m, k, s;
    cin >> n >> m >> k >> s;
    vector<Edge> out_edges_of_s;
    while (m--)
    {
        int a, b, cost;
        cin >> a >> b >> cost;
        if (a == s)
            out_edges_of_s.emplace_back(s, b, cost);
        in_edges[b].emplace_back(a, b, cost);
    }
    for (int i = 0; i <= n; ++i)
        fill(d[i], d[i] + k + 1, INF);
    d[s][0] = 0;

    for (auto& edge: out_edges_of_s)
    {
        if (d[edge.b][1] > edge.cost)
            d[edge.b][1] = edge.cost;
    }
    for (int j = 0; j <= 1; ++j)
    {
        for (int i = 1; i <= n; ++i)
            processed[i][j] = true;
    }


    for (int i = 1; i <= n; ++i)
    {
        if (D(i, k) != INF)
            cout << D(i, k) << "\n";
        else
            cout << -1 << "\n";
    }
    return  0;
}