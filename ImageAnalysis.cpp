#include "ImageAnalysis.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
using namespace std;

namespace {

const int Z = 224;     // the picture is shrunk to Z x Z first, so every photo is treated the same

// next whitespace-separated token, skipping '#' comment lines
bool nextToken(istream& in, string& tok) {
    tok.clear();
    char c;
    while (in.get(c)) {
        if (c == '#') { string skip; getline(in, skip); continue; }
        if (!isspace((unsigned char)c)) { tok += c; break; }
    }
    if (tok.empty()) return false;
    while (in.get(c) && !isspace((unsigned char)c)) tok += c;
    return true;
}

bool readPPM(const string& path, int& w, int& h, vector<unsigned char>& rgb) {
    ifstream in(path.c_str(), ios::binary);
    if (!in) return false;
    string magic, sw, sh, sm;
    if (!nextToken(in, magic) || !nextToken(in, sw) || !nextToken(in, sh) || !nextToken(in, sm)) return false;
    int maxv;
    if ((magic != "P6" && magic != "P3") || !parseInt(sw, w) || !parseInt(sh, h) || !parseInt(sm, maxv)) return false;
    if (w < 1 || h < 1 || maxv < 1 || maxv > 255) return false;
    size_t total = (size_t)w * h * 3;
    rgb.assign(total, 0);
    if (magic == "P6") {
        in.read((char*)&rgb[0], (streamsize)total);
        if ((size_t)in.gcount() != total) return false;
    } else {
        for (size_t i = 0; i < total; i++) {
            string t; int v;
            if (!nextToken(in, t) || !parseInt(t, v)) return false;
            rgb[i] = (unsigned char)v;
        }
    }
    if (maxv != 255)
        for (size_t i = 0; i < total; i++) rgb[i] = (unsigned char)(rgb[i] * 255 / maxv);
    return true;
}

double lum(const vector<unsigned char>& d, int i) {
    return 0.3 * d[i] + 0.59 * d[i + 1] + 0.11 * d[i + 2];
}

// Same rule as the web version: water-like colour AND smooth surface, lower 3/4 of the picture
double waterPercent(int w, int h, const vector<unsigned char>& src) {
    vector<unsigned char> d(Z * Z * 3);
    for (int y = 0; y < Z; y++)
        for (int x = 0; x < Z; x++) {
            int s = ((y * h / Z) * w + (x * w / Z)) * 3, t = (y * Z + x) * 3;
            d[t] = src[s]; d[t + 1] = src[s + 1]; d[t + 2] = src[s + 2];
        }
    int n = 0, total = 0;
    for (int y = Z / 4; y < Z - 1; y++)
        for (int x = 0; x < Z - 1; x++) {
            int i = (y * Z + x) * 3;
            int r = d[i], g = d[i + 1], b = d[i + 2];
            int mx = max(r, max(g, b)), mn = min(r, min(g, b));
            double sat = mx ? (double)(mx - mn) / mx : 0, br = lum(d, i);
            total++;
            bool colour = (b > r + 10) || (r >= g && g >= b && sat > 0.1 && sat < 0.65 && br > 50 && br < 215);
            bool smooth = fabs(br - lum(d, i + 3)) < 16 && fabs(br - lum(d, i + Z * 3)) < 16;
            if (colour && smooth) n++;
        }
    return (double)n / total * 100.0;
}

} // namespace

void imageAnalysisMenu() {
    printHeader("FLOOD IMAGE ANALYSIS");
    cout << "Enter the path of a PPM image (.ppm). Convert a JPG/PNG first, for example:\n";
    cout << "  magick photo.jpg photo.ppm        (or use any online JPG-to-PPM converter)\n\n";
    string path = readLine("Image path (empty to cancel): ");
    if (path.empty()) return;

    int w, h;
    vector<unsigned char> rgb;
    if (!readPPM(path, w, h, rgb)) {
        cout << "Error: could not read this file as a PPM (P3/P6) image.\n";
        return;
    }
    int pct = (int)(waterPercent(w, h, rgb) + 0.5);
    string sev = pct >= 35 ? "HIGH" : (pct >= 15 ? "MEDIUM" : "LOW");
    cout << "\nImage size       : " << w << " x " << h << "\n";
    cout << "Flood-water area : " << pct << "%\n";
    cout << "Estimated severity: " << sev << "\n";
    cout << "(Colour analysis only - an estimate, not a trained AI model.)\n";
}
