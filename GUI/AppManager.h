#ifndef APPMANAGER_H
#define APPMANAGER_H

#include "../include/raylib.h"
#include "../Entities/User.h"
#include "../Entities/Client.h"
#include "../Entities/Admin.h"
#include "../Entities/MonAn.h"
#include "../Entities/Order.h"
#include "../Entities/DatLich.h"
#include "../Entities/MayTram.h"
#include "../Entities/CartItem.h"
#include "../Entities/Voucher.h"
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
    bool isVoucherApplied = false;
    float secondCounter;
    
    int vYear = 2026; int vMonth = 10; int vDay = 29;
    int vHour = 13; int vMinute = 0; int vSecond = 0;
    float virtualTimer = 0.0f;

    void updateTimer();
    void processLogin(Vector2 mousePoint);
    void processWorkspace(Vector2 mousePoint, bool isAdmin);
    void processPopup(Vector2 mousePoint);
    void processKeyboard();
    void drawScreen(Font appFont, RenderTexture2D target);

    string selectedDoanhThuNgay = "";
    int doanhThuScroll = 0;
public:
    AppManager();
    ~AppManager();
    void run();
};

#endif