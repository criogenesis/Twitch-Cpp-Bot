#include <algorithm>
#include <iostream>

int main()
{
    std::string str = "H e l l o";
    str.erase(remove(str.begin(), str.end(), ' '), str.end());
    std::cout << str; // Output Hello
    
    return 0;
}
