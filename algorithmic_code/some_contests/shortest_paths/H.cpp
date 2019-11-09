#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <iomanip>
#include <ctime>
#include <set>
#include <cassert>
#define double float
using namespace std;
const double INF_d = numeric_limits<double>::infinity();
const int INF_i = INT32_MAX;
const int N = 2000 + 1;
vector<int> children[N];
vector<int> prev_(N, -1);
vector<double> d(N, INF_d);
bool processed[N];
bool visited[N][N];
int t[N];
int v[N];
int S[N][N];
double cost[N][N];

struct Vertex
{
    int v;
    double prior;
    Vertex(int V, double Prior)
            :v(V)
            ,prior(Prior)
    {}
    bool operator<(const Vertex& other) const
    {
        return prior < other.prior;
    }
};

vector<Vertex> Q;

void dfs(int source, int curr, int dist)
{
    visited[source][curr] = true;
    S[source][curr] = S[curr][source] = dist;
    for (auto& c: children[curr])
    {
        if (!visited[source][c])
            dfs(source, c, dist + S[curr][c]);
    }
}

//void Dijkstra(int v)
//{
//    d[v] = 0;
//    priority_queue<Vertex> Q;
//    Q.emplace(v, 0.);
//    while (!Q.empty())
//    {
//        int u = Q.top().v;
//        Q.pop();
//        processed[u] = true;
//        for (auto& c : children[u])
//        {
//            if (!processed[c])
//            {
//                if (d[u] + cost[c][u] < d[c])
//                {
//                    d[c] = d[u] + cost[c][u];
//                    prev_[c] = u;
//                    Q.emplace(c, d[c]);find
//                }
//            }
//        }
//    }
//}

/*
void Dijkstra_set(int v)
{
    d[v] = 0;
    set<Vertex> Q;
    Q.emplace(v, 0.);
    while (!Q.empty())
    {
        auto beg_it = Q.begin();
        int u = beg_it->v;
        Q.erase(beg_it);
        processed[u] = true;
        for (auto& c : children[u])
        {
            if (!processed[c])
            {
                if (d[u] + cost[c][u] < d[c])
                {
                    d[c] = d[u] + cost[c][u];
                    prev_[c] = u;
                    Q.emplace(c, d[c]);
                }
            }
        }
    }
}
*/

void Dijkstra(int v)
{
    d[v] = 0;
    vector<Vertex> Q;
    Q.reserve(N);
    Q.emplace_back(v, 0.);
    while (!Q.empty())
    {
        auto iter_to_min = min_element(Q.begin(), Q.end());
        int u = iter_to_min->v;
        Q.erase(iter_to_min);
        processed[u] = true;
        for (auto& c : children[u])
        {
            if (!processed[c])
            {
                if (d[u] + cost[c][u] < d[c])
                {
                    d[c] = d[u] + cost[c][u];
                    prev_[c] = u;
                    auto iter = find_if(Q.begin(), Q.end(), [&c](Vertex& ver){return ver.v == c;});
                    if (iter == Q.end())
                        Q.emplace_back(c, d[c]);
                    else
                        iter->prior = d[c];
                }
            }
        }
    }
}
int main() {
    //clock_t beg = clock();
    //cout << "start... " << "\n";
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    std::cout.tie(0);
    int n;
    cin >> n;
    for (int i = 1; i <= n; ++i)
    {
        fill(S[i], S[i] + n + 1, INF_i);
        fill(cost[i], cost[i] + n + 1, INF_d);
    }
    for (int i = 1; i <= n; ++i)
    {
        int t_i, v_i;;
        cin >> t_i >> v_i;
        t[i] = t_i;
        v[i] = v_i;

        cost[i][i] = 0;
        S[i][i] = 0;
    }

    for (int i = 1; i <= n - 1; ++i)
    {
        int a, b, S_i;
        cin >> a >> b >> S_i;
        S[a][b] = S[b][a] =  S_i;
        children[a].push_back(b);
        children[b].push_back(a);
    }

    for (int v = 1; v <= n; ++v)
    {
        //fill (visited, visited + n  + 1, false);
        dfs(v, v, 0);
    }


//    clock_t curr = clock();
//    cout << "after n dfs elapsed " << double(curr - beg) / CLOCKS_PER_SEC << "\n";


    for (int a = 1; a <= n; ++a)
    {
        for (int b = 1; b <= n; ++b)
        {
            children[a].push_back(b);
            if (cost[a][b] == INF_d) {
                cost[a][b] = t[a] + (double) S[a][b] / (double) v[a];
            }
        }
    }


//    curr = clock();
//    cout << "after cost[i][j] computation elapsed  " << double(curr - beg) / CLOCKS_PER_SEC << "\n";


    Dijkstra(1);


//    curr = clock();
//    cout << "after Dijkstra elapsed " << double(curr - beg) / CLOCKS_PER_SEC << "\n";


    int max_t_city = 1;
    for (int i = 1; i <= n; ++i)
        max_t_city = (d[i] > d[max_t_city]) ? i : max_t_city;
    cout << setprecision(10) << d[max_t_city] << "\n";
    for (int u = max_t_city; u != 1; u = prev_[u])
        cout << u << " ";
    cout << 1 << "\n";


//    clock_t end = clock();
//    cout << "runtime is " << double(end - beg) / CLOCKS_PER_SEC << "\n";

    return  0;
}
