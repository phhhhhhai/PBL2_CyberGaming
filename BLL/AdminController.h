#ifndef ADMINCONTROLLER_H
#define ADMINCONTROLLER_H

#include "../Entities/User.h"
#include "../Entities/Client.h"
#include "../Entities/Admin.h"
#include "../Entities/MonAn.h"
#include "../Entities/MayTram.h"
#include "../Entities/DatLich.h"
#include "../Entities/Order.h"
#include "../Entities/Voucher.h"
#include "../BLL/DoublyLinkedList.h"
#include "../BLL/Vector.h"
#include "../BLL/Queue.h"
#include "../BLL/Pricing.h"
#include <string>

using namespace std;

class DoanhThuNgayDTO {
public:
    string ngay;
    double tongNap;
    double tongBooking;
    double tongFnB;

    DoanhThuNgayDTO(string n = "") : ngay(n), tongNap(0), tongBooking(0), tongFnB(0) {}
    double getTongDoanhThu() const { return tongNap + tongBooking + tongFnB; }
};

class HoaDonDTO {
public:
    int userId; string thoiGian; int machineId; double thanhTien; double giamGia; double tongTien; int trangThai; int thanhToan; int coVoucher;
    HoaDonDTO() : userId(0), thoiGian(""), machineId(0), thanhTien(0), giamGia(0), tongTien(0), trangThai(0), thanhToan(0), coVoucher(0) {}
    HoaDonDTO(int u, string tg, int m, double ttGoc, double gg, double t, int tt, int p, int vc = 0) 
        : userId(u), thoiGian(tg), machineId(m), thanhTien(ttGoc), giamGia(gg), tongTien(t), trangThai(tt), thanhToan(p), coVoucher(vc) {}
};

class AdminController {
private:
    DoublyLinkedList<User*>& dsTaiKhoan;
    DoublyLinkedList<DatLich*>& dsDatLich;
    DoublyLinkedList<MonAn*>& dsMon;
    MayTram* (*grid)[COLS]; 
    User* findUserByPhone(const string& phone);
    Client* findClientByPhone(const string& phone);
    MayTram* findMachineById(int id);
public:
    AdminController(DoublyLinkedList<User*>& users, DoublyLinkedList<DatLich*>& bookings, DoublyLinkedList<MonAn*>& foods, MayTram* g[ROWS][COLS]);
    
    Client* findClientById(int id);    
    MayTram* findMachineByUsername(const string& username);

    bool isValidPhoneNumber(string phone);
    bool isDuplicateUser(string phone);
    
    int addTaiKhoan(string phone, string pass, double initialBalance, string thoiGianLog = "");
    bool napTien(string username, double amount, string thoiGianLog = "");
    bool updatePassword(string username, string newPass);
    bool deleteTaiKhoan(string username, Queue<Order*>& ordersQueue);
    bool deleteBooking(int bookingId);

    int assignSeat(string username, int idMay);
    bool isMachineInBooking(int idMay);
    int changeMachine(int srcId, int destId);
    
    int validateAndAddMonAn(string tenMon, string giaStr);
    int addMonAn(string tenMon, double donGia);
    bool updateMonAn(int idSua, string tenMonMoi, double donGiaMoi);
    bool deleteMonAn(int idXoa);

    int validateAndSaveBooking(string phone, string idMayStr, string date, string hStr, string mStr, string durationStr, string pass, 
        int currentTotalDays, int selectedBookingId, string thoiGianLog = "");

    Vector<string> updateBookingLogics(int vDay, int vMonth, int vYear, int vHour, int vMinute);
    Vector<string> addVirtualMinutes(int mins, int& vDay, int& vMonth, int& vYear, int& vHour, int& vMinute, bool isHappyHourActive);

    void thanhToanBill(Queue<Order*>& q, DoublyLinkedList<MonAn*>& dsMon, int userId, string thoiGianBill, string thoiGianLog);
    void giaoHangBill(Queue<Order*>& q, int userId, string thoiGian);
    int deleteBill(Queue<Order*>& q, int userId, string thoiGian);

    int updateOrderQuantity(Queue<Order*>& q, int orderId, int newQty);
    int deleteOrder(Queue<Order*>& q, int orderId);

    Vector<HoaDonDTO> getDanhSachHoaDon(Queue<Order*>& ordersQueue);

    int tinhTongSoNgay(int d, int m, int y) const;
    void layKhoangTuanTongKet(int vDay, int vMonth, int vYear, int vHour, int& startDays, int& endDays, int& tuanTrongThang, int& thangHienThi) const;
    string layTieuDeBXH(int vDay, int vMonth, int vYear, int vHour) const;
    Vector<HangXepHang> thongKeXepHangTuan(int vDay, int vMonth, int vYear, int vHour);
    Vector<HangXepHang> layTop5BXH(int vDay, int vMonth, int vYear, int vHour);
    int layHangHienTaiCuaKhach(int userId, int vDay, int vMonth, int vYear, int vHour, double& doanhThuTuan);
    string dinhDangSDT(const string& phone, bool hienDayDu) const;
    Voucher layThongTinVoucherCuaKhach(int userId, Queue<Order*>& ordersQueue, int vDay, int vMonth, int vYear, int vHour);
    bool daDungVoucherTrongTuan(int userId, Queue<Order*>& ordersQueue, int vDay, int vMonth, int vYear, int vHour) const;
    bool coTheDungVoucher(int userId, Queue<Order*>& ordersQueue, int vDay, int vMonth, int vYear, int vHour);

    Vector<string> layDanhSachNgayCoDoanhThu();
    DoanhThuNgayDTO thongKeDoanhThuNgay(string ngayYYYYMMDD);
};

#endif