#ifndef ADMINCONTROLLER_H
#define ADMINCONTROLLER_H

#include "../Entities/User.h"
#include "../Entities/Client.h"
#include "../Entities/Admin.h"
#include "../Entities/MonAn.h"
#include "../Entities/MayTram.h"
#include "../Entities/DatLich.h"
#include "../Entities/Order.h"
#include "../BLL/DoublyLinkedList.h"
#include "../BLL/Vector.h"
#include "../BLL/Queue.h"                 
#include <string>

using namespace std;

class AdminController {
private:
    DoublyLinkedList<User*>& dsTaiKhoan;
    DoublyLinkedList<DatLich*>& dsDatLich;
    DoublyLinkedList<MonAn*>& dsMon;
    MayTram* (*grid)[COLS]; 
    User* findUserByPhone(const string& phone);
    Client* findClientByPhone(const string& phone);
    Client* findClientById(int id);
    MayTram* findMachineById(int id);
    MayTram* findMachineByUsername(const string& username);
public:
    AdminController(DoublyLinkedList<User*>& users, DoublyLinkedList<DatLich*>& bookings, DoublyLinkedList<MonAn*>& foods, MayTram* g[ROWS][COLS]);

    bool isValidPhoneNumber(string phone);
    bool isDuplicateUser(string phone);
    
    int addTaiKhoan(string phone, string pass, double initialBalance, string thoiGianLog = "");
    bool napTien(string username, double amount, string thoiGianLog = "");
    bool updatePassword(string username, string newPass);
    bool deleteTaiKhoan(string username);
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

    void updateTop5Vouchers();
};

#endif