#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>

/** Begin fast allocation */
const int MAX_MEM = 1e8;
int mpos = 0;
char mem[MAX_MEM];
inline void * operator new ( size_t n ) {
    assert((mpos += n) <= MAX_MEM);
    return (void *)(mem + mpos - n);
}
inline void operator delete ( void * ) noexcept { } // must have!
/** End fast allocation */


//#define DEBUG
using namespace std;
using pair_clss = pair<int, int>;
const int pow = 20;
const int N = 1 << pow;
int n;
int p[N];
int p2[N];
int pos[N];
int cls[N];
int cls2[N];
string s, word;
int word_len;

#ifdef DEBUG
void PrintCyclicSubstr(int position, int l)
{
    int tail = min(n - position, l);
    cout << s.substr(position, tail) << s.substr(0, l - tail);
}
void PrintSufArray(const int suf_arr[], int l)
{
    for (int i = 0; i < n; ++i)
    {
        cout << "SufArr[" << i << "] = " << suf_arr[i] << " :";
        PrintCyclicSubstr(suf_arr[i], l);
        cout << "\n";
    }
    cout << "\n";
}
#endif // DEBUG

bool char_less(int i, int j)
{
    return s[i] < s[j];
}


void ConstructSufArray()
{
    int l = 1;
    for (int i = 0; i < n; ++i)
        p[i] = i;
    sort(p, p + n, char_less);

    int cc = 0;
    for (int i = 0; i < n; ++i)
        cc += (i && (s[p[i]] != s[p[i - 1]])), cls[p[i]] = cc;

    while (l < n)
    {
        for (int i = 0; i < n; ++i)
            pos[i] = 0;

        for (int i = 0; i < n; ++i)
            ++pos[cls[i] + 1];

        for (int i = 1; i < n; ++i)
            pos[i] += pos[i - 1];

        for (int i = 0; i < n; ++i)
        {
            int j = (p[i] - l + n) % n;
            p2[pos[cls[j]]++] = j;
        }

        cc = 0;
        for (int i = 0; i < n; ++i)
        {
            cc += (i && (pair_clss{cls[p2[i]], cls[(p2[i] + l + n) % n]} !=
                         pair_clss{cls[p2[i - 1]], cls[(p2[i - 1] + l + n) % n]}));
            cls2[p2[i]] = cc;
        }

        swap(cls2, cls); swap(p2, p);
        l *= 2;
//#ifdef DEBUG
//        PrintSufArray(p, l);
//#endif //DEBUG
    }
}

bool BinSearch(int l = 1, int r = n)
{
    int mid = (l + r) / 2;
    int i = 0;
    while (p[mid] + i < n - 1 && i < word_len && s[p[mid] + i] == word[i]) ++i;

    if (i == word_len)
        return true;
    if (l == r)
        return false;
    if (p[mid] + i == n - 1)
        return BinSearch(mid + 1, r);
    if (s[p[mid] + i] > word[i])
        return BinSearch(l, mid);
    return BinSearch(mid + 1, r);
}

int main()
{
    ios_base::sync_with_stdio(false); cin.tie(0); cout.tie(0);
    cin >> s; s += '$';
    n = s.size();

    ConstructSufArray();

    int m; cin >> m;
    while (m--)
    {
        cin >> word;
        word_len = word.size();
        if (BinSearch())
            cout << "Yes" << "\n";
        else
            cout << "No" << "\n";
    }
#ifdef DEBUG
    PrintSufArray(p, n);
#endif //DEBUG
    return 0;
}