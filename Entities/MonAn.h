#ifndef MONAN_H
#define MONAN_H

#include <string>
using namespace std;

class MonAn {
private:
    int id;
    string tenMon;
    double donGia;

public:
    MonAn(int i = 0, string ten = "", double gia = 0) 
        : id(i), tenMon(ten), donGia(gia) {}

    int getId() const { return id; }
    string getTenMon() const { return tenMon; }
    double getDonGia() const { return donGia; }

    void setTenMon(string ten) { tenMon = ten; }
    void setDonGia(double gia) { donGia = gia; }
};

#endif