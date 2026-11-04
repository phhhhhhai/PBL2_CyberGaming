#ifndef ADMIN_H
#define ADMIN_H

#include "User.h"
#include <iostream>

class Admin : public User {
public:
    Admin(int id = 0, string u = "", string p = "", int st = 1) 
        : User(id, u, p, 1, st) {}

    void showInfo() const override {
        cout << "[ADMIN] ID: " << id << " | SĐT: " << username << " - Quan tri vien\n";
    }
};

#endif