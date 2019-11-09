#include <iostream>
#include <vector>
using namespace std;
const int N = 2e5;

struct mstack
{
    int top_weight;
    mstack* next;
    mstack() = default;
    mstack(const mstack& other) = default;
    explicit mstack(int m, mstack* next)
        :top_weight(m)
        ,next(next)
    {}

    mstack* mpush(int m)
    {
        auto* new_top = new mstack(m, this);
        return new_top;
    }

    mstack* mpop()
    {
        return next;
    }

};

mstack* snowmen[N];
unsigned long long weights[N];
int sum;

int main()
{
    int n; cin >> n;
    for (int i = 1; i <= n; ++i)
    {
        int t, m;
        cin >> t >> m;
        if (m == 0)
        {
            sum += (weights[i] = weights[t] - snowmen[t]->top_weight);
            snowmen[i] = snowmen[t]->mpop();
        }
        else
        {
            sum += (weights[i] = weights[t] + m);
            snowmen[i] = snowmen[t]->mpush(m);
        }
    }
    cout << sum << "\n";
    return 0;
}