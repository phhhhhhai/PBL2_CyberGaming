#ifndef VOUCHER_H
#define VOUCHER_H

#include <string>
using namespace std;

class GiaoDichDoanhThu {
private:
    string thoiGian;
    string loai;
    int userId;
    string chiTiet;
    int soLuong;
    double thanhTien;
    int thuTuLog;

public:
    GiaoDichDoanhThu(string tg = "", string l = "", int uId = 0, string ct = "", int sl = 0, double tt = 0, int idx = 0)
        : thoiGian(tg), loai(l), userId(uId), chiTiet(ct), soLuong(sl), thanhTien(tt), thuTuLog(idx) {}

    string getThoiGian() const { return thoiGian; }
    string getLoai() const { return loai; }
    int getUserId() const { return userId; }
    string getChiTiet() const { return chiTiet; }
    int getSoLuong() const { return soLuong; }
    double getThanhTien() const { return thanhTien; }
    int getThuTuLog() const { return thuTuLog; }
};

class HangXepHang {
private:
    int hang;
    int userId;
    string soDienThoai;
    double tongDoanhThu;
    int mocDatCuoi;

public:
    HangXepHang(int h = 0, int uId = 0, string sdt = "", double tdt = 0, int moc = 0)
        : hang(h), userId(uId), soDienThoai(sdt), tongDoanhThu(tdt), mocDatCuoi(moc) {}

    int getHang() const { return hang; }
    int getUserId() const { return userId; }
    string getSoDienThoai() const { return soDienThoai; }
    double getTongDoanhThu() const { return tongDoanhThu; }
    int getMocDatCuoi() const { return mocDatCuoi; }

    void setHang(int h) { hang = h; }
    void setTongDoanhThu(double tdt) { tongDoanhThu = tdt; }
    void setMocDatCuoi(int moc) { mocDatCuoi = moc; }
};

class Voucher {
private:
    int userId;
    double phanTramGiam;
    int ngayBatDauHieuLuc;
    int ngayKetThucHieuLuc;
    bool daSuDung;

public:
    Voucher(int uId = 0, double pt = 0.1, int bd = 0, int kt = 0, bool used = false)
        : userId(uId), phanTramGiam(pt), ngayBatDauHieuLuc(bd), ngayKetThucHieuLuc(kt), daSuDung(used) {}

    int getUserId() const { return userId; }
    double getPhanTramGiam() const { return phanTramGiam; }
    int getNgayBatDauHieuLuc() const { return ngayBatDauHieuLuc; }
    int getNgayKetThucHieuLuc() const { return ngayKetThucHieuLuc; }
    bool isDaSuDung() const { return daSuDung; }

    void setDaSuDung(bool used) { daSuDung = used; }
    bool conHieuLuc(int ngayHienTai) const {
        return !daSuDung && (ngayHienTai >= ngayBatDauHieuLuc && ngayHienTai <= ngayKetThucHieuLuc);
    }
};

#endif