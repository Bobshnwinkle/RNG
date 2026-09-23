#include "./fstRand.h"
#include <chrono>
#include <csignal>
#include <iomanip>
#include <sstream>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <thread>

#define ushort unsigned short

using namespace std;

class localTools{

    struct Stats{
        ulong count;
        double min;
        double max;
        double mean;
        double median;
        double lowerQuartile;
        double upperQuartile;
        double interQuartileRange;
    };

    struct Size{
        uint height;
        uint width;
    };

    public:

    static Stats ProcessValues(vector<double> vals){
        if (vals.empty()) return {0, 0, 0, 0, 0, 0, 0, 0};

        vals = quickSort(vals);

        size_t length = vals.size();

        double tot = 0;
        for (size_t i = 0; i < length; i++){
            tot += vals[i];
        }

        double median = 0;
        if (length % 2 == 0){
            median = (vals[length / 2] + vals[(length / 2) - 1]) / 2;
        }
        else{
            median = vals[length / 2];
        }
        double LQ = vals[length / 4];
        double UQ = vals[length - (length / 4) - 1];
        return {length, vals[0], vals[length - 1], tot/length, median, LQ, UQ, UQ - LQ};
    }

    // Median-of-three pivot with a three-way partition, so runs of equal values
    // (the timing data is almost all one number) stay O(n) instead of O(n^2).
    // Recurses into the smaller side only, keeping stack depth at O(log n).
    static vector<double> quickSort(vector<double> vals){
        SortRange(vals, 0, (long)vals.size() - 1);
        return vals;
    }

    static void SortRange(vector<double>& v, long lo, long hi){
        while (lo < hi){
            long mid = lo + (hi - lo) / 2;
            if (v[mid] < v[lo]) swap(v[mid], v[lo]);
            if (v[hi] < v[lo]) swap(v[hi], v[lo]);
            if (v[hi] < v[mid]) swap(v[hi], v[mid]);
            double pivot = v[mid];

            long lt = lo, i = lo, gt = hi;
            while (i <= gt){
                if (v[i] < pivot) swap(v[lt++], v[i++]);
                else if (v[i] > pivot) swap(v[i], v[gt--]);
                else i++;
            }

            if (lt - lo < hi - gt){
                SortRange(v, lo, lt - 1);
                lo = gt + 1;
            }
            else{
                SortRange(v, gt + 1, hi);
                hi = lt - 1;
            }
        }
    }

    static void PrintStats(const vector<double>& vals, ostream& out = cout, ushort DP = 5, float heightFrac = 0.25){
        auto stats = ProcessValues(vals);
        out << "Count: " << stats.count << "\033[K\n";
        out << "Min: " << stats.min << "\033[K\n";
        out << "Max: " << stats.max << "\033[K\n";
        out << "Mean: " << stats.mean << "\033[K\n";
        out << "Median: " << stats.median << "\033[K\n";
        out << "Lower Quartile: " << stats.lowerQuartile << "\033[K\n";
        out << "Upper Quartile: " << stats.upperQuartile << "\033[K\n";
        out << "Inter-Quartile Range: " << stats.interQuartileRange << "\033[K\n";
        PrintGraph(vals, stats, out, DP, heightFrac);
    }

    // ---- terminal control ----

    static void Home(){ cout << "\033[H"; }
    static void GotoXY(uint x, uint y){ cout << "\033[" << y + 1 << ';' << x + 1 << 'H'; }
    static void ClearBelow(){ cout << "\033[J"; }
    static void ClearScreen(){ cout << "\033[2J\033[H"; }
    static void HideCursor(bool hide){ cout << (hide ? "\033[?25l" : "\033[?25h"); }
    static void AltScreen(bool on){ cout << (on ? "\033[?1049h" : "\033[?1049l"); }

    static Size GetSize(){
        struct winsize w{};
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1 || w.ws_col == 0 || w.ws_row == 0){
            return {24, 80}; // not a terminal (piped/redirected) - fall back
        }
        return {w.ws_row, w.ws_col};
    }

    // ---- formatting ----

    static float ReScale(float value, float oldMax, float newMax){
        if (oldMax == 0) return 0;
        return value * (newMax/oldMax);
    }

    // Compact, non-padded number. Unlike to_string(double) this gives
    // "20332" and "0.999878" rather than "20332.000000".
    static string Fmt(double v){
        ostringstream s;
        s << setprecision(6) << v;
        return s.str();
    }

    // Right-aligned count that never silently loses digits: falls back to a
    // k/M/G suffix when it will not fit in the label gutter.
    static string FitCount(unsigned long long n, size_t width){
        string s = to_string(n);
        if (s.size() > width){
            const char* suffix = "kMGT";
            double v = n;
            for (int i = 0; i < 4 && s.size() > width; i++){
                v /= 1000.0;
                ostringstream o;
                o << setprecision(3) << v << suffix[i];
                s = o.str();
            }
        }
        if (s.size() > width) s = string(width, '+');
        while (s.size() < width) s = ' ' + s;
        return s;
    }

    static string GetBlock(float frac){
        int oct = floor(frac * 8);
        switch(oct){
            case 0: return "▁";
            case 1: return "▂";
            case 2: return "▃";
            case 3: return "▄";
            case 4: return "▅";
            case 5: return "▆";
            case 6: return "▇";
            default: return "█";
        }
    }

    static string MarkerColour(int col, int LQ, int Mean, int Median, int UQ){
        if (col == Mean) return "\033[0;34m";
        if (col == Median) return "\033[0;32m";
        if (col == UQ) return "\033[0;33m";
        if (col == LQ) return "\033[0;31m";
        return "";
    }

    static char MarkerLetter(int col, int LQ, int Mean, int Median, int UQ){
        if (col == Mean) return 'M';
        if (col == Median) return 'm';
        if (col == UQ) return 'U';
        if (col == LQ) return 'L';
        return 0;
    }

    static void PrintGraph(const vector<double>& vals, const Stats& stats, ostream& out, ushort DP = 3, float heightFrac = 0.3){
        auto size = GetSize();
        int Swidth = size.width;
        int Sheight = size.height * heightFrac;

        ushort borderFromLeft = DP;
        ushort borderFromBottom = 1;

        // Plot area is x in [borderFromLeft + 1, Swidth - 2] and
        // y in [borderFromBottom + 1, Sheight - 1]. The Y axis occupies
        // x == borderFromLeft, so the first bin must start one to its right.
        int plotCols = Swidth - borderFromLeft - 2;
        int plotRows = Sheight - borderFromBottom - 1;

        if (plotCols < 1 || plotRows < 1 || vals.empty()){
            out << "[no graph: " << (vals.empty() ? "no data" : "terminal too small") << "]\033[K\n";
            return;
        }

        // Bin across [min, max] rather than [0, max], otherwise any data set
        // that does not start at zero collapses into the leftmost bins.
        double lo = stats.min;
        double span = stats.max - stats.min;

        auto toCol = [&](double v) -> int {
            if (span <= 0) return 0;
            int c = floor((v - lo) / span * plotCols);
            if (c < 0) c = 0;
            if (c >= plotCols) c = plotCols - 1; // the max value belongs in the last bin
            return c;
        };

        vector<uint> columns(plotCols, 0);
        for (size_t i = 0; i < vals.size(); i++){
            columns[toCol(vals[i])]++;
        }

        uint colMax = 0;
        for (int i = 0; i < plotCols; i++){
            if (columns[i] > colMax) colMax = columns[i];
        }
        if (colMax == 0) colMax = 1;

        // Height per column, computed once rather than once per cell.
        vector<float> barH(plotCols);
        for (int i = 0; i < plotCols; i++){
            barH[i] = ReScale(columns[i], colMax, plotRows);
        }

        int LQ = toCol(stats.lowerQuartile);
        int Mean = toCol(stats.mean);
        int Median = toCol(stats.median);
        int UQ = toCol(stats.upperQuartile);

        string colsLabel = FitCount(colMax, DP);
        string loLabel = Fmt(stats.min);
        string hiLabel = Fmt(stats.max);

        int loStart = borderFromLeft + 1;
        int hiStart = (Swidth - 1) - (int)hiLabel.size();
        if (hiStart < loStart){
            hiLabel.clear();
            hiStart = Swidth;
        }
        if (hiStart < loStart + (int)loLabel.size() + 1) loLabel.clear();

        for (int y = Sheight - 1; y >= 0; y--){
            string text = "";
            for (int x = 0; x < Swidth - 1; x++){
                int col = x - borderFromLeft - 1; // plot column, -1 when left of the plot
                int row = y - borderFromBottom - 1; // plot row, 0 at the bottom

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
                    if (x >= loStart && x < loStart + (int)loLabel.size()){
                        text += loLabel[x - loStart];
                    }
                    else if (x >= hiStart && x < hiStart + (int)hiLabel.size()){
                        text += hiLabel[x - hiStart];
                    }
                    else if (col >= 0 && MarkerLetter(col, LQ, Mean, Median, UQ)){
                        text += MarkerColour(col, LQ, Mean, Median, UQ);
                        text += MarkerLetter(col, LQ, Mean, Median, UQ);
                        text += "\033[0m";
                    }
                    else{
                        text += ' ';
                    }
                }
                else if (x == borderFromLeft && y == borderFromBottom){
                    text += "┼";
                }
                else if (y == borderFromBottom){
                    string colour = (col >= 0) ? MarkerColour(col, LQ, Mean, Median, UQ) : "";
                    if (!colour.empty()) text += colour + "┼" + "\033[0m";
                    else text += "─";
                }
                else if (x == borderFromLeft){
                    text += "│";
                }
                else{
                    float h = barH[col];
                    int W = floor(h);
                    float F = h - W;
                    if (h > row){
                        string colour = MarkerColour(col, LQ, Mean, Median, UQ);
                        text += colour;
                        text += (row == W) ? GetBlock(F) : "█";
                        if (!colour.empty()) text += "\033[0m";
                    }
                    else{
                        text += ' ';
                    }
                }
            }
            out << text << "\033[K\n";
        }
    }
};

// Async-signal-safe terminal restore, so Ctrl-C does not leave the cursor
// hidden and the alternate screen buffer active.
static const char kRestore[] = "\033[?25h\033[?1049l";

extern "C" void onSignal(int sig){
    ssize_t ignored = write(STDOUT_FILENO, kRestore, sizeof(kRestore) - 1);
    (void)ignored;
    _exit(128 + sig);
}

int main(){
    signal(SIGINT, onSignal);
    signal(SIGTERM, onSignal);

    const uint SAMPLES = 10000;
    const auto REDRAW_INTERVAL = chrono::milliseconds(1);

    vector<double> vals;
    vector<double> times;
    fstRand rnd;

    localTools::AltScreen(true);
    localTools::HideCursor(true);

    auto lastDraw = chrono::steady_clock::now();

    for (uint i = 0; i < SAMPLES; i++){
        auto start = chrono::high_resolution_clock::now();
        vals.push_back(rnd.Next());
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start);
        times.push_back(duration.count());

        // Redraw on a wall-clock budget: every sample would re-sort both
        // vectors thousands of times a second for no visible benefit.
        if (chrono::steady_clock::now() - lastDraw >= REDRAW_INTERVAL || i + 1 == SAMPLES){
            // Build the whole frame first, then home the cursor and write it
            // in one go - flushing per line is what makes redraws tear.
            ostringstream frame;
            frame << i + 1 << " / " << SAMPLES << "\033[K\n";
            frame << "Value Stats:\033[K\n";
            localTools::PrintStats(vals, frame);
            frame << "Time Stats:\033[K\n";
            localTools::PrintStats(times, frame);

            cout << "\033[H" << frame.str() << "\033[J" << flush;

            // Stamp AFTER rendering. Stamping before it means that once a
            // frame costs more than REDRAW_INTERVAL the next sample is always
            // overdue, and the loop collapses into redrawing every iteration.
            lastDraw = chrono::steady_clock::now();
        }
        std::this_thread::sleep_for(REDRAW_INTERVAL);

    }

    localTools::HideCursor(false);
    localTools::AltScreen(false);

    // Leaving the alternate screen restores the old contents, so paint the
    // final result onto the normal screen to make it persist.
    cout << "Value Stats:\n";
    localTools::PrintStats(vals);
    cout << "Time Stats:\n";
    localTools::PrintStats(times);
    cout << flush;
}
