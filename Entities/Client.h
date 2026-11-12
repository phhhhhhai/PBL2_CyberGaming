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

    // --- CÁC HÀM XỬ LÝ VOUCHER (TÍNH ĐÓNG GÓI OOP) ---
    void setVoucher(Voucher* v) {
        if (myVoucher != nullptr) delete myVoucher;
        myVoucher = v;
    }

    bool hasVoucher() const {
        return myVoucher != nullptr;
    }

    Voucher* getVoucher() const {
        return myVoucher;
    }

    double tinhTienGiamFnB(double tongTienGoc) const {
        if (myVoucher != nullptr) {
            return myVoucher->tinhSoTienGiam(tongTienGoc);
        }
        return 0;
    }

    double tinhTienFnB(double tongTienGoc) const {
        return tongTienGoc - tinhTienGiamFnB(tongTienGoc);
    }

    void showInfo() const override {
        cout << "[CLIENT] ID: " << id << " | SĐT: " << username << " - So du: " << soDu << " VND\n";
    }
};

#endif