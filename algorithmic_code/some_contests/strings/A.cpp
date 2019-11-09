#include <iostream>
#include <string>
#include <algorithm>

//#define DEBUG
using namespace std;
const int N = 1e6 + 1;
int zf[N];

void calc_zf(const string& s, int n)
{
    int left = 0; int right = 0;

    for(int i = 1; i < n; ++i)
    {
        zf[i] = max(0, min(right - i, zf[i - left]));
        while(i + zf[i] < n && s[zf[i]] == s[i + zf[i]]) ++zf[i];
        if(i + zf[i] > right)
        {
            left  = i;
            right = i + zf[i];
        }
    }
}
int main()
{
    ios_base::sync_with_stdio(false); cin.tie(0); cout.tie(0);
    string text; cin >> text;
    int n = text.size();
    calc_zf(text, n);

#ifdef DEBUG
    for (int i = 0; i < n; ++i)
        cout << "zf[" << i  << "] = " << zf[i] << ", ";
    cout << "\n";
#endif // DEBUG

    for(int t = 1; t <= n / 2; ++t)
    {
        if(zf[t] >= n - t && n % t == 0)
        {
            cout << t;
            return 0;
        }
    }
    cout << n;
    return 0;
}