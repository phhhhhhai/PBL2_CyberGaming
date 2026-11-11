#ifndef RAYLIBUI_H
#define RAYLIBUI_H

#include <raylib.h>
#include "../Entities/User.h"
#include "../Entities/MayTram.h"
#include "../Entities/MonAn.h"
#include "../Entities/Order.h"
#include "../Entities/CartItem.h"
#include "../Entities/DatLich.h"
#include "../BLL/DoublyLinkedList.h"
#include "../BLL/Queue.h"
#include "../BLL/Vector.h"
#include <string>

using namespace std;

class RaylibUI {
public:
    // --- CÁC BIẾN TRẠNG THÁI CHO TÍNH NĂNG ĐỔI MÁY ---
    static bool isChangingMachine;       // Đang ở chế độ chọn máy đích
    static int sourceMachineId;             // ID máy của khách cần đổi
    static bool showConfirmMoveModal;    // Hiển thị hộp thoại xác nhận đổi máy
    static int targetMachineId;             // ID máy đích được chọn
    static string moveNotificationMsg;      // Thông báo lỗi (ví dụ: "Đã có khách, không chọn được")
    static float moveNotificationTimer;     // Thời gian hiển thị thông báo

    static void drawLoginScreen(Font font, string inputUser, string inputPass, int activeInput, string loginMessage, DoublyLinkedList<User*>& dsTK, int loginScroll);
    static void drawWorkspace(Font font, string clockStr, int currentTab, MayTram* grid[ROWS][COLS], 
        DoublyLinkedList<User*>& dsTK, DoublyLinkedList<MonAn*>& dsMon, Queue<Order*>& ordersQueue, 
        DoublyLinkedList<CartItem*>& cartList, DoublyLinkedList<DatLich*>& dsDatLich, User* currentUser, int popupState, int activePopInp, 
        string p1, string p2, string p3, string p4, string p5, string p6, string p7, string pMsg,
        int assignScroll, int selectedMachine, bool isHappyHourActive);
    static void drawGrid2D(Font font, MayTram* grid[ROWS][COLS], int offsetX, int offsetY);

    static void transferMachineData(MayTram grid[ROWS][COLS], int srcId, int destId, const string& filename);
    static void drawMoveMachineLogic(int screenWidth, int screenHeight, MayTram grid[ROWS][COLS]);

    static void drawOrderManagement(Font font, Order dsOrders[], int soLuongOrder, float contentY);
    static void drawClientMenuModal(Font font, MonAn dsMon[], int soLuongMon, int menuScroll);
};

#endif