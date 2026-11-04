#ifndef CLIENT_H
#define CLIENT_H

#include "User.h"
#include <iostream>

class Client : public User {
private:
    double soDu;

public:
    Client(int id = 0, string u = "", string p = "", double tien = 0, int st = 1) 
        : User(id, u, p, 2, st), soDu(tien) {}

    double getSoDu() const { return soDu; }
    void setSoDu(double tien) { soDu = tien; }

    void napTien(double tien) { if (tien > 0) soDu += tien; }
    bool truTien(double tien) {
        if (soDu >= tien) {
            soDu -= tien;
            return true;
        }
        return false;
    }

    void showInfo() const override {
        cout << "[CLIENT] ID: " << id << " | SĐT: " << username << " - So du: " << soDu << " VND\n";
    }
};

#endif