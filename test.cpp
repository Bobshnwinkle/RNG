#include "./fstRand.h"
#include <chrono>

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

    struct ValStats{
        vector<float> values;
        vector<float> times;
        Stats valueStats;
        Stats timeStats;
    };

    public:
    static Stats ProcessValues(vector<float> vals){
        vals = quickSort(vals);
        float tot = 0;
        for (ushort i = 0; i < vals.size(); i++){
            tot += vals[i];
        }

        ushort length = vals.size();

        float median = 0;
        if (length % 2 == 0){
            median = (vals[length / 2] + vals[(length / 2) - 1]) / 2;
        }
        else{
            median = vals[length / 2];
        }
        float LQ = vals[length / 4];
        float UQ = vals[length - (length / 4)];
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

    static void PrintStats(Stats stats){
        cout << "Min: " << stats.min << endl;
        cout << "Max: " << stats.max << endl;
        cout << "Mean: " << stats.mean << endl;
        cout << "Median: " << stats.median << endl;
        cout << "Lower Quartile: " << stats.lowerQuartile << endl;
        cout << "Upper Quartile: " << stats.upperQuartile << endl;
        cout << "Inter-Quartile Range: " << stats.interQuartileRange << endl;
    }
};

int main(){
    vector<float> vals;
    vector<float> times;
    auto rnd = new fstRand();
    for (ushort i = 0; i < 1000; i++){
        auto start = chrono::high_resolution_clock::now();
        vals.push_back(rnd->Next());
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start);
        times.push_back(duration.count());
    }
    auto valStats = localTools::ProcessValues(vals);
    auto timeStats = localTools::ProcessValues(times);
    cout << "Value Stats: " << endl;
    localTools::PrintStats(valStats);
    cout << "Time Stats: " << endl;
    localTools::PrintStats(timeStats);
}