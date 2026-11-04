#ifndef MAYTRAM_H
#define MAYTRAM_H

#include "Client.h"

const int ROWS = 4;
const int COLS = 10;

class MayTram {
private:
    int id;
    int loaiMay;   // 0: STD, 1: VIP, 2: STR
    int trangThai; // 0: Trong, 1: Dang choi, 2: Don dep...
    Client* currentClient; // Thay vi dung string, dung con tro tro den object Client

public:
    MayTram(int i = 0, int loai = 0, int tt = 0) 
        : id(i), loaiMay(loai), trangThai(tt), currentClient(nullptr) {}

    int getId() const { return id; }
    int getLoaiMay() const { return loaiMay; }
    int getTrangThai() const { return trangThai; }
    Client* getCurrentClient() const { return currentClient; }

    void setTrangThai(int tt) { trangThai = tt; }
    
    // Gán khách vào máy
    void assignClient(Client* c) {
        currentClient = c;
        trangThai = 1;
    }
    
    // Giải phóng máy
    void freeMachine() {
        currentClient = nullptr;
        trangThai = 0;
    }
};

#endif