#ifndef ADMINCONTROLLER_H
#define ADMINCONTROLLER_H

#include "../Entities/User.h"
#include "../Entities/Client.h"
#include "../Entities/Admin.h"
#include "../Entities/MonAn.h"
#include "../Entities/MayTram.h"
#include "../Entities/DatLich.h"
#include "DoublyLinkedList.h"
#include <string>

using namespace std;

class AdminController {
public:
    static bool isValidPhoneNumber(string phone);
    static bool isDuplicateUser(string phone, DoublyLinkedList<User*>& dsTaiKhoan);
    
    static int addTaiKhoan(DoublyLinkedList<User*>& dsTaiKhoan, string phone, string pass, double initialBalance);
    static bool napTien(DoublyLinkedList<User*>& dsTaiKhoan, string username, double amount);
    static bool updatePassword(DoublyLinkedList<User*>& dsTaiKhoan, string username, string newPass);
    static bool deleteTaiKhoan(DoublyLinkedList<User*>& dsTaiKhoan, string username, MayTram* grid[ROWS][COLS]);
    
    static int assignSeat(string username, int idMay, DoublyLinkedList<User*>& dsTaiKhoan, MayTram* grid[ROWS][COLS]);
    
    static int validateAndAddMonAn(DoublyLinkedList<MonAn*>& dsMon, string tenMon, string giaStr);
    static int addMonAn(DoublyLinkedList<MonAn*>& dsMon, string tenMon, double donGia);
    static bool updateMonAn(DoublyLinkedList<MonAn*>& dsMon, int idSua, string tenMonMoi, double donGiaMoi);
    static bool deleteMonAn(DoublyLinkedList<MonAn*>& dsMon, int idXoa);

    static int validateAndSaveBooking(string phone, string idMayStr, string date, string hStr, string mStr, string durationStr, string pass, 
        int currentTotalDays, int selectedBookingId, DoublyLinkedList<DatLich*>& dsDatLich, DoublyLinkedList<User*>& dsTaiKhoan);
};

#endif