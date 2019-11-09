#include <iostream>
#include <vector>
#include <algorithm>

struct Node
{
    int val;
    Node* leftChild;
    Node* rightChild;
    explicit Node(int v = 0, Node* l = nullptr, Node* r = nullptr)
        :val(v)
        ,leftChild(l)
        ,rightChild(r)
    {}
};

const int INF = INT32_MAX;
int n, n2, m;
const int N = 131072 + 1; // 2^17 + 1 >= 1e5
const int M = 1e5 + 1;
int rec_p[N];
int next[N];
int pos[M];
Node* versions[N];


void getNext()
{
    for (int r = 1; r <= n; ++r)
    {
        if (pos[rec_p[r]] != 0)
            next[pos[rec_p[r]]] = r;
        pos[rec_p[r]] = r;
    }
}

inline int roundUp2thPow(int v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

void buildBase(Node* root, int level = n2)
{
    if (level == 1)
        return;
    root->leftChild  = new Node;
    root->rightChild = new Node;
    buildBase(root->leftChild,  level / 2);
    buildBase(root->rightChild, level / 2);
}


Node* incr(Node* root, int l, int r, int pos)
{
    Node* copy = new Node(root->val + 1);
    if (l != r)
    {
        int mid = (l + r) / 2;
        if (pos <= mid)
        {
            copy->leftChild = incr(root->leftChild, l, mid, pos);
            copy->rightChild = root->rightChild;
        }
        else
        {
            copy->rightChild= incr(root->rightChild, mid + 1, r, pos);
            copy->leftChild = root->leftChild;
        }
    }
    return copy;
}


int get_sum(Node* segm, int l ,int r, int i, int j)
{
    if(r < i || l > j) //out of range
        return 0;
    if (i > j) //seg empty
        return 0;
    if (i == l && j == r)
        return segm->val;
    int mid = (l + r) / 2;
    return  get_sum(segm->leftChild, l, mid, i, std::min(mid, j))
            +
            get_sum(segm->rightChild, mid + 1, r, std::max(mid + 1, i), j);
}



int binSearchAns(int li, int ki)
{
    int l_b = li; int r_b = n;
    while (true)
    {

        int mid = (l_b + r_b) / 2;
        if (l_b == r_b)
            return mid; //mid == l_b == r_b

        int res = get_sum(versions[mid], 1, n2, mid + 1, n2)
                  -
                  get_sum(versions[li - 1], 1, n2, mid + 1, n2);
        if (res >= ki)
            r_b = mid;

        else
            l_b = mid + 1;
    }
}

int main()
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(0); std::cout.tie(0);
    std::cin >> n >> m;
    n2 = roundUp2thPow(n + 1);
    for (int i = 1; i <= n; ++i)
    {
        int ser; std::cin >> ser;
        rec_p[i] =  ser;
    }

    getNext();
    versions[0] = new Node;
    buildBase(versions[0]);

    for (int i = 1; i <= n; ++i)
    {
        if (next[i] == 0)
            versions[i] = incr(versions[i - 1], 1, n2, n + 1);
        else
            versions[i] = incr(versions[i - 1], 1, n2, next[i]);
    }

    int p = 0;
    int q; std::cin >> q;

    for (int i = 1; i <= q; ++i)
    {
        int xi, yi;
        std::cin >> xi >> yi;
        int li = (xi + p) % n + 1;
        int ki = (yi + p) % m + 1;
        int distinct_servs_num = get_sum(versions[n], 1, n2, n + 1, n2) - get_sum(versions[li - 1], 1, n2, n + 1, n2);
        if (distinct_servs_num < ki)
        {
            std::cout << 0 << "\n";
            p = 0;
        }
        else
        {
            p =  binSearchAns(li, ki);
            std::cout << p << "\n";
        }
    }
}