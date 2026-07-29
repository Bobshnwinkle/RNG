#include "./fstRand.h"

#define ushort unsigned short

using namespace std;

class localTools{

    struct Stats{
        float min;
        float max;
        float mean;
        float median;
        float lowerQuartile;
        float upperQuartile;
        float interQuartileRange;
    };

    public:
    static Stats ProcessValues(vector<float> vals){
        vals = quickSort(vals);
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
        return {vals[0], vals[length - 1], tot/length, median, LQ, UQ, UQ - LQ};
    }

    static vector<float> quickSort(vector<float> vals){
        if (vals.size() <= 1) return vals;
        vector<float> left, right;
        float pivot = vals[0];
        for (ushort i = 1; i < vals.size(); i++){
            if (vals[i] < pivot) left.push_back(vals[i]);
            else right.push_back(vals[i]);
        }
        left = quickSort(left);
        right = quickSort(right);
        left.push_back(pivot);
        left.insert(left.end(), right.begin(), right.end());
        return left;
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
