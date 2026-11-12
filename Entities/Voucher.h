#ifndef VOUCHER_H
#define VOUCHER_H

#include <string>
using namespace std;

class Voucher {
private:
    int phanTramGiam; // Ví dụ: 10 (tương ứng 10%)
    string moTa;

public:
    Voucher(int pt = 10, string mt = "Voucher Top 5 (-10% F&B)")
        : phanTramGiam(pt), moTa(mt) {}

    int getPhanTramGiam() const { return phanTramGiam; }
    string getMoTa() const { return moTa; }

    double tinhSoTienGiam(double tongTienGoc) const {
        return tongTienGoc * (phanTramGiam / 100.0);
    }
};

#endif