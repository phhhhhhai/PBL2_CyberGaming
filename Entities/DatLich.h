#ifndef DATLICH_H
#define DATLICH_H

#include <string>
using namespace std;

class DatLich {
private:
    int id;
    int userId; 
    int idMay;
    string ngayDat;
    int gioBatDau;
    int phutBatDau;
    int thoiLuong;
    int trangThai; 

public:
    DatLich(int i=0, int uId=0, int mId=0, string ngay="", int g=0, int p=0, int tl=0, int tt=0)
        : id(i), userId(uId), idMay(mId), ngayDat(ngay), gioBatDau(g), phutBatDau(p), thoiLuong(tl), trangThai(tt) {}

    int getId() const { return id; }
    int getUserId() const { return userId; } 
    int getIdMay() const { return idMay; }
    string getNgayDat() const { return ngayDat; }
    int getGioBatDau() const { return gioBatDau; }
    int getPhutBatDau() const { return phutBatDau; }
    int getThoiLuong() const { return thoiLuong; }
    int getTrangThai() const { return trangThai; }

    void setTrangThai(int tt) { trangThai = tt; }
    void setUserId(int uId) { userId = uId; } 
    void updateLich(int mId, string ngay, int g, int p, int tl) {
        idMay = mId; ngayDat = ngay; gioBatDau = g; phutBatDau = p; thoiLuong = tl;
    }
};

#endif