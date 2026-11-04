#ifndef DATLICH_H
#define DATLICH_H

#include <string>
using namespace std;

class DatLich {
private:
    int id;
    string sdtKhach;
    int idMay;
    string ngayDat;
    int gioBatDau;
    int phutBatDau;
    int thoiLuong;
    int trangThai; // 0: Dang cho, 1: Da hoan thanh, 2: Da huy

public:
    DatLich(int i=0, string sdt="", int mId=0, string ngay="", int g=0, int p=0, int tl=0, int tt=0)
        : id(i), sdtKhach(sdt), idMay(mId), ngayDat(ngay), gioBatDau(g), phutBatDau(p), thoiLuong(tl), trangThai(tt) {}

    int getId() const { return id; }
    string getSdtKhach() const { return sdtKhach; }
    int getIdMay() const { return idMay; }
    string getNgayDat() const { return ngayDat; }
    int getGioBatDau() const { return gioBatDau; }
    int getPhutBatDau() const { return phutBatDau; }
    int getThoiLuong() const { return thoiLuong; }
    int getTrangThai() const { return trangThai; }

    void setTrangThai(int tt) { trangThai = tt; }
    void updateLich(int mId, string ngay, int g, int p, int tl) {
        idMay = mId; ngayDat = ngay; gioBatDau = g; phutBatDau = p; thoiLuong = tl;
    }
};

#endif