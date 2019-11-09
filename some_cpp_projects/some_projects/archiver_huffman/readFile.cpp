#include <iostream>
#include <fstream>

using namespace std;

int main()
{
  ifstream inF("a.in", ios_base::in|ios_base::binary);
  if (!inF)
    {
       cout << "oops failed to open\n";
    }
  else
    {
      cout << "opened\n";
    }
}
