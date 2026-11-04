#ifndef CARTITEM_H
#define CARTITEM_H

#include <string>
using namespace std;

class CartItem {
private:
    int foodId;     // Lưu Khóa ngoại thay vì tên món
    string tenMon;  // Lưu tạm tên và giá để in ra giao diện cho nhanh
    double donGia;
    int soLuong;

public:
    CartItem(int fId = 0, string ten = "", double gia = 0, int sl = 1)
        : foodId(fId), tenMon(ten), donGia(gia), soLuong(sl) {}
    
    int getFoodId() const { return foodId; }
    string getTenMon() const { return tenMon; }
    double getDonGia() const { return donGia; }
    int getSoLuong() const { return soLuong; }

    void addSoLuong(int sl) { soLuong += sl; }
};

#endif