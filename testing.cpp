#include <iostream>
#include <vector>
#include <string>

using namespace std;

class whatup
{
    int value[10];
    int* ptr;
public:
    whatup(int val[10])
    {
        for (int i = 0; i < 10; ++i)
        {
            value[i]=val[i];
        }
        ptr=NULL;
    }   
    
    void print()
    {
        cout<<"The value is : "<<value[0]<<"\n";
    }
};

int main(int argc, char const *argv[])
{
    int arr[10] = {1,2,3,4,5,6,7,8,9,10};
    whatup w(arr);
    w.print();
    
    w.print();
    return 0;
}