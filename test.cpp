#include "./fstRand.h"
#include <chrono>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#define ushort unsigned short

using namespace std;

class localTools{

    struct Stats{
        ulong count;
        float min;
        float max;
        float mean;
        float median;
        float lowerQuartile;
        float upperQuartile;
        float interQuartileRange;
    };

    struct Size{
        uint height;
        uint width;
    };

    public:

    static Stats ProcessValues(vector<float> vals){
        vals = quickSort(vals);

        float max = vals[vals.size() - 1];
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
        return {vals.size(), vals[0], vals[length - 1], tot/length, median, LQ, UQ, UQ - LQ};
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

    static void PrintStats(vector<float> vals){
        auto stats = ProcessValues(vals);
        cout << "Count: " << stats.count << endl;
        cout << "Min: " << stats.min << endl;
        cout << "Max: " << stats.max << endl;
        cout << "Mean: " << stats.mean << endl;
        cout << "Median: " << stats.median << endl;
        cout << "Lower Quartile: " << stats.lowerQuartile << endl;
        cout << "Upper Quartile: " << stats.upperQuartile << endl;
        cout << "Inter-Quartile Range: " << stats.interQuartileRange << endl;
        PrintGraph(vals, stats, 5, 0.25);
    }

    static Size GetSize(){
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        return {w.ws_row, w.ws_col};
    }

    static float ReScale(float value, float oldMax, float newMax){
        return value * (newMax/oldMax);
    }

    static string GetBlock(float frac){
        int oct = floor(frac * 8);
        switch(oct){
            case 0: return " ";
            case 1: return "\u2581";
            case 2: return "\u2582";
            case 3: return "\u2583";
            case 4: return "\u2584";
            case 5: return "\u2585";
            case 6: return "\u2586";
            case 7: return"\u2587";
            default: return "\u2588";
        }
    }

    static void PrintGraph(vector<float> vals, Stats stats, ushort DP = 3, float height = 0.3){
        auto size = GetSize();
        int Swidth = size.width;
        int Sheight = size.height * height;

        ushort borderFromLeft = DP;

        ushort activeCols = Swidth - borderFromLeft;

        vector<uint> columns(activeCols, 0);

        // cout << "created " << columns.size() << " columns" << endl;

        for (int i = 0; i < vals.size(); i++){
            float scaled = ReScale(vals[i], stats.max, activeCols - 1);
            ushort col = floor(scaled);
            // cout << "calculated col " << i << " of " << vals.size() << " , " << vals[i] << " : " << scaled << " : " << col << endl;
            columns[col]++;
        }

        int colMax = 0;
        for (int i = 0; i < columns.size(); i++){
            if (columns[i] > colMax){
                colMax = columns[i];
            }
        }

        uint LQ = ReScale(stats.lowerQuartile, stats.max, activeCols - 1);
        uint Mean = ReScale(stats.mean, stats.max, activeCols - 1);
        uint Median = ReScale(stats.median, stats.max, activeCols - 1);
        uint UQ = ReScale(stats.upperQuartile, stats.max, activeCols - 1);


        ushort borderFromBottom = 1;
        string colsLabel = to_string(colMax);
        string rowsLabel = to_string(stats.max);

        while (colsLabel.size() < DP){
            colsLabel = ' ' + colsLabel;
        }

        for (int y = Sheight - 1; y >= 0; y--){
            string text = "";
            for (int x = 0; x < Swidth; x++){
                if (x < borderFromLeft){
                    //to left of Y axis
                    if (y == borderFromBottom && x == borderFromLeft - 1){
                        text += '0';
                    }
                    else if (y == Sheight - 1){
                        text += colsLabel[x];
                    }
                    else{
                        text += ' ';
                    }
                }
                else if(y < borderFromBottom){
                    //below X axis
                    if (x == borderFromLeft && y == borderFromBottom - 1){
                        text += '0';
                    }
                    else if (Swidth - x <= rowsLabel.size()){
                        text += rowsLabel[(x - Swidth) + rowsLabel.size()];
                    }
                    else if (x == Mean + borderFromLeft) text += "M";
                    else if (x == Median + borderFromLeft) text += "m";
                    else if (x == UQ + borderFromLeft) text += "U";
                    else if (x == LQ + borderFromLeft) text += "L";
                    else{
                        text += ' ';
                    }
                }
                else if (x == borderFromLeft && y == borderFromBottom){
                    text += "+";
                }
                else if (y == borderFromBottom){
                    text += "-";
                }
                else if (x == borderFromLeft){
                    text += "|";
                }
                else{
                    auto height = ReScale(columns[x - borderFromLeft], colMax, Sheight - borderFromBottom);
                    int_fast64_t W = floor(height);
                    float F = height - W;
                    // cout << "height: " << height << " , W: " << W << " , F: " << F << " , y: " << y << endl;
                    if (height > y){
                        if (y == W){
                            text += GetBlock(F);
                        }
                        else{
                            if (x == Mean + borderFromLeft) text += "\u2592";
                            else if (x == Median + borderFromLeft) text += "\u2592";
                            else if (x == UQ + borderFromLeft) text += "\u2592";
                            else if (x == LQ + borderFromLeft) text += "\u2592";
                            else{
                                text += "\u2588";
                            }
                        }
                    }
                    else{
                        text += ' ';
                    }
                }
            }
            cout << text << endl;
        }
    }
};

int main(){
    vector<float> vals;
    vector<float> times;
    auto rnd = new fstRand();
    for (uint i = 0; i < 10000; i++){
        auto start = chrono::high_resolution_clock::now();
        vals.push_back(rnd->Next());
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start);
        times.push_back(duration.count());
        // gotoxy(0, 0);
        cout << "\r" << i + 1 << " / 10000";
        // cout << "\nValue Stats: " << endl;
        // localTools::PrintStats(vals);
        // cout << "\nTime Stats: " << endl;
        // localTools::PrintStats(times);
        // cout << endl;
    }
    cout << "\nValue Stats: " << endl;
    localTools::PrintStats(vals);
    cout << "\nTime Stats: " << endl;
    localTools::PrintStats(times);
    cout << endl;
}