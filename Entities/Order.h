#ifndef ORDER_H
#define ORDER_H

#include <string>
using namespace std;

class Order {
private:
    int id;
    int machineId;
    int userId;
    int idMon;
    int soLuong;   
    int status;
    int paid;
    string thoiGian;
    int voucherUsed;

public:
    Order(int i=0, int mId=0, int uId=0, int idM=0, int sl=0, int st=0, int pd=0, string tg="", int vc=0)
        : id(i), machineId(mId), userId(uId), idMon(idM), soLuong(sl), status(st), paid(pd), thoiGian(tg), voucherUsed(vc) {}

    int getId() const { return id; }
    int getMachineId() const { return machineId; }
    int getUserId() const { return userId; }
    int getIdMon() const { return idMon; }
    int getSoLuong() const { return soLuong; }
    int getStatus() const { return status; }
    int getPaid() const { return paid; }
    string getThoiGian() const { return thoiGian; }
    int getVoucherUsed() const { return voucherUsed; }

    void setStatus(int st) { status = st; }
    void setPaid(int pd) { paid = pd; }
    void setSoLuong(int sl) { soLuong = sl; }
    void setThoiGian(string tg) { thoiGian = tg; }
    void setMachineId(int mId) { machineId = mId; }
    void setVoucherUsed(int vc) { voucherUsed = vc; }
};

#endif