#include "./fstRand.h"

#define ushort unsigned short

using namespace std;

int main(){
    auto rnd = new fstRand();
    for (ushort i = 0; i < 100; i++){
        cout << rnd->Next() << endl;
    }
}