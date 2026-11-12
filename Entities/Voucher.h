#ifndef VOUCHER_H
#define VOUCHER_H

#include <string>
using namespace std;

class Voucher {
private:
    int phanTramGiam;
    int ngayHetHan;
    bool daSuDung;
    string moTa;

public:
    // Đã đổi thứ tự param để C++ tự map đúng: (int, int, bool, string)
    Voucher(int pt, int expDate, bool used, string mt = "Voucher Top 5 (-10% F&B)")
        : phanTramGiam(pt), ngayHetHan(expDate), daSuDung(used), moTa(mt) {}

    int getPhanTramGiam() const { return phanTramGiam; }
    int getNgayHetHan() const { return ngayHetHan; }
    bool isDaSuDung() const { return daSuDung; }
    string getMoTa() const { return moTa; }

    void setDaSuDung(bool used) { daSuDung = used; }

    bool isValid(int currentDateInt) const {
        return !daSuDung && (currentDateInt <= ngayHetHan);
    }

    double tinhSoTienGiam(double tongTienGoc, int currentDateInt) const {
        if (isValid(currentDateInt)) {
            return tongTienGoc * (phanTramGiam / 100.0);
        }
        return 0;
    }
};

#endif