#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "../Entities/User.h"
#include "../Entities/Client.h"
#include "../Entities/Admin.h"
#include "../Entities/MonAn.h"
#include "../Entities/Order.h"
#include "../Entities/DatLich.h"
#include "../Entities/MayTram.h"
#include "../Entities/Voucher.h"
#include "../BLL/DoublyLinkedList.h"
#include "../BLL/Queue.h"
#include "../BLL/Vector.h"
#include <string>

using namespace std;

class FileManager {
public:
    static void loadTaiKhoan(const string& filename, DoublyLinkedList<User*>& dsTaiKhoan);
    static void saveTaiKhoan(const string& filename, DoublyLinkedList<User*>& dsTaiKhoan);
    
    static void loadMenu(const string& filename, DoublyLinkedList<MonAn*>& dsMonAn);
    static void saveMenu(const string& filename, DoublyLinkedList<MonAn*>& dsMonAn);

    static void loadOrders(const string& filename, Queue<Order*>& ordersQueue);
    static void saveOrders(const string& filename, Queue<Order*>& ordersQueue);

    static void loadGrid(const string& filename, MayTram* grid[ROWS][COLS], DoublyLinkedList<User*>& dsTaiKhoan);
    static void saveGrid(const string& filename, MayTram* grid[ROWS][COLS]);

    static void loadBookings(const string& filename, DoublyLinkedList<DatLich*>& dsDatLich);
    static void saveBookings(const string& filename, DoublyLinkedList<DatLich*>& dsDatLich);

    static void logDoanhThu(string thoiGian, string loai, int userId, string chiTiet, int soLuong, double thanhTien);
    static void loadDoanhThu(const string& filename, Vector<GiaoDichDoanhThu>& dsGiaoDich);
};

#endif