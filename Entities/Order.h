#ifndef ORDER_H
#define ORDER_H

#include <string>
using namespace std;

class Order {
private:
    int id;
    int machineId;
    int userId; // Khóa ngoại tới TaiKhoan
    int idMon; // Khóa ngoại tới Menu
    int soLuong;   
    int status; // 0: Chua giao, 1: Da giao
    int paid; // 0: Chua thanh toan, 1: Da thanh toan

public:
    Order(int i=0, int mId=0, int uId=0, int idM=0, int sl=0, int st=0, int pd=0)
        : id(i), machineId(mId), userId(uId), idMon(idM), soLuong(sl), status(st), paid(pd) {}

    int getId() const { return id; }
    int getMachineId() const { return machineId; }
    int getUserId() const { return userId; }
    int getIdMon() const { return idMon; }
    int getSoLuong() const { return soLuong; }
    int getStatus() const { return status; }
    int getPaid() const { return paid; }

    void setStatus(int st) { status = st; }
    void setPaid(int pd) { paid = pd; }
    void setSoLuong(int sl) { soLuong = sl; }
};

#endif