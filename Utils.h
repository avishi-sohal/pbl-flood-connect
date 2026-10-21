#ifndef UTILS_H
#define UTILS_H

// Small helper functions used by every module (input reading + validation).
// They are marked "inline" so this header can be included in many .cpp files
// without causing "duplicate definition" linker errors.

#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
using namespace std;

// Remove spaces/tabs/newlines from both ends of a string
inline string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

inline string toUpper(string s) {
    for (size_t i = 0; i < s.size(); i++)
        s[i] = (char)toupper((unsigned char)s[i]);
    return s;
}

// '|' is our file separator, so it must not appear inside user text
inline string removePipe(string s) {
    for (size_t i = 0; i < s.size(); i++)
        if (s[i] == '|') s[i] = '/';
    return s;
}

// Shorten text so it fits in a table column
inline string fit(const string& s, size_t width) {
    if (s.size() <= width) return s;
    return s.substr(0, width - 1) + ".";
}

// Accepts only digits (non-negative whole number, max 9 digits)
inline bool parseInt(const string& s, int& value) {
    if (s.empty() || s.size() > 9) return false;
    for (size_t i = 0; i < s.size(); i++)
        if (!isdigit((unsigned char)s[i])) return false;
    value = atoi(s.c_str());
    return true;
}

// Show prompt, read one line. If input is closed (Ctrl+D / end of piped file) exit cleanly.
inline string readLine(const string& prompt) {
    cout << prompt;
    string line;
    if (!getline(cin, line)) {
        cout << "\nInput closed. Exiting.\n";
        exit(0);
    }
    return trim(line);
}

// Keep asking until the user types something
inline string readNonEmpty(const string& prompt) {
    while (true) {
        string s = readLine(prompt);
        if (!s.empty()) return removePipe(s);
        cout << "  Error: this field cannot be empty.\n";
    }
}

// Keep asking until the user types a whole number between minVal and maxVal
inline int readInt(const string& prompt, int minVal, int maxVal) {
    while (true) {
        string s = readLine(prompt);
        int v;
        if (parseInt(s, v) && v >= minVal && v <= maxVal) return v;
        cout << "  Error: enter a whole number between " << minVal << " and " << maxVal << ".\n";
    }
}

inline string readSeverity(const string& prompt) {
    while (true) {
        string s = toUpper(readLine(prompt));
        if (s == "HIGH" || s == "MEDIUM" || s == "LOW") return s;
        cout << "  Error: severity must be HIGH, MEDIUM or LOW.\n";
    }
}

inline string readContact(const string& prompt) {
    while (true) {
        string s = readLine(prompt);
        bool ok = (s.size() == 10);
        for (size_t i = 0; ok && i < s.size(); i++)
            if (!isdigit((unsigned char)s[i])) ok = false;
        if (ok) return s;
        cout << "  Error: contact number must be exactly 10 digits.\n";
    }
}

inline bool confirm(const string& prompt) {
    return toUpper(readLine(prompt)) == "Y";
}

inline void printHeader(const string& title) {
    cout << "\n===== " << title << " =====\n\n";
}

#endif
