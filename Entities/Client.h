#ifndef CLIENT_H
#define CLIENT_H

#include "User.h"
#include "Voucher.h"
#include <iostream>

class Client : public User {
private:
    double soDu;
    Voucher* myVoucher;

public:
    Client(int id = 0, string u = "", string p = "", double tien = 0, int st = 1) 
        : User(id, u, p, 2, st), soDu(tien), myVoucher(nullptr) {}

    ~Client() override {
        if (myVoucher != nullptr) {
            delete myVoucher;
            myVoucher = nullptr;
        }
    }

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

    void setVoucher(Voucher* v) {
        if (myVoucher != nullptr) delete myVoucher;
        myVoucher = v;
    }

    bool hasValidVoucher(int currentDateInt) const {
        return (myVoucher != nullptr && myVoucher->isValid(currentDateInt));
    }

    Voucher* getVoucher() const { return myVoucher; }

    void suDungVoucher() {
        if (myVoucher != nullptr) {
            myVoucher->setDaSuDung(true);
        }
    }

    double tinhTienGiamFnB(double tongTienGoc, int currentDateInt) const {
        if (myVoucher != nullptr) {
            return myVoucher->tinhSoTienGiam(tongTienGoc, currentDateInt);
        }
        return 0;
    }

    double tinhTienFnB(double tongTienGoc, int currentDateInt) const {
        return tongTienGoc - tinhTienGiamFnB(tongTienGoc, currentDateInt);
    }

    void showInfo() const override {
        cout << "[CLIENT] ID: " << id << " | SĐT: " << username << " - So du: " << soDu << " VND\n";
    }
};

#endif