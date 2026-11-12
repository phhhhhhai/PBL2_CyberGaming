#ifndef APPMANAGER_H
#define APPMANAGER_H

#include "../Entities/User.h"
#include "../Entities/Client.h"
#include "../Entities/Admin.h"
#include "../Entities/MonAn.h"
#include "../Entities/Order.h"
#include "../Entities/DatLich.h"
#include "../Entities/MayTram.h"
#include "../Entities/CartItem.h"
#include "../BLL/DoublyLinkedList.h"
#include "../BLL/Queue.h"
#include "../BLL/AdminController.h"
#include "../BLL/ClientController.h"
#include "../DAL/FileManager.h"
#include "RaylibUI.h"
#include <string>
#include "../BLL/Vector.h"
#include <algorithm>

class AppManager {
private:
    DoublyLinkedList<User*> dsTaiKhoan;
    DoublyLinkedList<MonAn*> dsMon;
    DoublyLinkedList<DatLich*> dsDatLich;
    Queue<Order*> ordersQueue;
    DoublyLinkedList<CartItem*> cartList;
    
    AdminController* adminCtrl;
    ClientController* clientCtrl;

    MayTram* grid[ROWS][COLS];

    int appState;          
    int currentTab;        
    User* currentUser;

    string inputUser = "";
    string inputPass = "";
    int activeInput = 0; 
    string loginMessage = "";

    int popupState = 0; 
    int selectedMachine = -1;
    string selectedAccountUsername = ""; 
    int selectedFoodId = -1;       
    int selectedOrderId = -1; 
    int selectedBookingId = -1;
    string selectedBillTime = "";

    string popInp1 = ""; string popInp2 = ""; string popInp3 = ""; 
    string popInp4 = ""; string popInp5 = ""; string popInp6 = ""; string popInp7 = "";
    int activePopInp = 1;
    string popMsg = "";
    int assignScroll = 0; 
    int loginScroll = 0;
    bool isHappyHourActive = false; 
    float secondCounter;
    
    int vYear = 2026; int vMonth = 10; int vDay = 29;
    int vHour = 13; int vMinute = 0; int vSecond = 0;
    float virtualTimer = 0.0f;
    int lastUpdateDay = -1;

    void updateTimer();                               // Cập nhật giờ và trừ tiền
    void processLogin(Vector2 mousePoint);            // Xử lý click ở màn hình đăng nhập
    void processWorkspace(Vector2 mousePoint, bool isAdmin); // Xử lý click chuyển tab/chọn máy
    void processPopup(Vector2 mousePoint);            // Xử lý click nút Xác nhận/Hủy trong Popup
    void processKeyboard();                           // Xử lý gõ phím nhập liệu
    void drawScreen(Font appFont, RenderTexture2D target); // Vẽ giao diện
public:
    AppManager();
    ~AppManager();
    void run();
};

#endif