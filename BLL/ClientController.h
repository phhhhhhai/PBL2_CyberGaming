#ifndef CLIENTCONTROLLER_H
#define CLIENTCONTROLLER_H

#include "../Entities/User.h"
#include "../Entities/Client.h"
#include "../Entities/MayTram.h"
#include "../Entities/CartItem.h"  
#include "../Entities/Order.h"
#include "DoublyLinkedList.h"
#include "Queue.h"
#include "Pricing.h"
#include <string>

using namespace std;

class ClientController {
private:
    DoublyLinkedList<User*>& dsTaiKhoan;
    MayTram* (*grid)[COLS];
    Client* findClientByPhone(const string& username);
    MayTram* findMachineByUsername(const string& username);

public:
    ClientController(DoublyLinkedList<User*>& users, MayTram* g[ROWS][COLS]);

    static double getTickRate(int loaiMay, bool isHappyHour);
    
    int getAssignedMachine(string username, int& loaiMay);
    bool changePassword(string username, string newPass);
    bool processTick(string username, double deductionAmount);
    void returnMachine(string username); 
    int validateLogin(string username, string password, int currentHour, User*& loggedInUser);
    int datMon(string username, DoublyLinkedList<CartItem*>& cart, Queue<Order*>& ordersQueue, string thoiGian, bool useVoucher = false);
};
#endif