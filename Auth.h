#ifndef AUTH_H
#define AUTH_H

#include <iostream>
#include <string>
#include "Utils.h"
using namespace std;

// MODULE 9: login and role-based access.
// Passwords are never compared as plain text: they are turned into a number with the
// djb2 hash first. This is a classroom demo, not real security (no salt, accounts are in the code).
enum Role { ROLE_NONE, ROLE_ADMIN, ROLE_RESCUE, ROLE_CITIZEN };

inline unsigned long hashPassword(const string& s) {
    unsigned long h = 5381;
    for (size_t i = 0; i < s.size(); i++)
        h = (h * 33 + (unsigned char)s[i]) & 0xFFFFFFFFUL;
    return h;
}

inline string roleName(Role r) {
    if (r == ROLE_ADMIN) return "admin";
    if (r == ROLE_RESCUE) return "rescue team";
    if (r == ROLE_CITIZEN) return "citizen";
    return "none";
}

// Demo accounts: admin/admin123, rescue/rescue123, citizen/citizen123. 3 attempts allowed.
inline Role login() {
    printHeader("LOGIN");
    cout << "Demo accounts: admin / admin123, rescue / rescue123, citizen / citizen123\n\n";
    for (int attempt = 1; attempt <= 3; attempt++) {
        string user = toLowerCase(readLine("Username: "));
        unsigned long pw = hashPassword(readLine("Password: "));
        if (user == "admin"   && pw == hashPassword("admin123"))   return ROLE_ADMIN;
        if (user == "rescue"  && pw == hashPassword("rescue123"))  return ROLE_RESCUE;
        if (user == "citizen" && pw == hashPassword("citizen123")) return ROLE_CITIZEN;
        cout << "  Wrong username or password.\n";
    }
    return ROLE_NONE;
}

#endif
