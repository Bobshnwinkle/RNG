#include "./fstRand.h"

#define ushort unsigned short

using namespace std;

class localTools{
    public:
    static void ProcessValues(vector<float> vals){
        vals = bubbleSort(vals);
        float tot = 0;
        for (ushort i = 0; i < vals.size(); i++){
            tot += vals[i];
            cout << i + 1 << " : " << vals[i] << endl;
        }

        ushort length = vals.size();

        cout << endl << "Generated " + to_string(length) + " values" << endl;
        cout << "Min " << vals[0] << endl;
        cout << "Max " << vals[length - 1] << endl;
        cout << "Mean " << tot/length << endl;
        float median = 0;
        if (length % 2 == 0){
            median = (vals[length / 2] + vals[(length / 2) - 1]) / 2;
        }
        else{
            median = vals[length / 2];
        }
        cout << "Median " << median << endl;
        float LQ = vals[length / 4];
        float UQ = vals[length - (length / 4)];
        cout << "Lower Quartile " << LQ << endl;
        cout << "Upper Quartile " << UQ << endl;
        cout << "Inter-quartile range " << UQ - LQ << endl;
    }

    static vector<float> bubbleSort(vector<float> vals){
        float c = 0;
        bool clear = false;
        while (!clear){
            clear = true;
            for (ushort i = 0; i < vals.size(); i++){
                c = vals[i];
                if (i + 1 == vals.size()) break;
                if (vals[i + 1] < c){
                    clear = false;
                    vals[i] = vals[i + 1];
                    vals[i + 1] = c;
                }
            }
        }
        return vals;
    }
};

int main(){
    vector<float> vals;
    auto rnd = new fstRand();
    for (ushort i = 0; i < 1000; i++){
        vals.push_back(rnd->Next());
    }
    localTools::ProcessValues(vals);
}
