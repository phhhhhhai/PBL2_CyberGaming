#ifndef CLIENTCONTROLLER_H
#define CLIENTCONTROLLER_H

#include "../Entities/User.h"
#include "../Entities/Client.h"
#include "../Entities/MayTram.h"
#include "DoublyLinkedList.h"
#include "Pricing.h" // Kế thừa đa hình từ file Pricing.h cũ
#include <string>

using namespace std;

class ClientController {
public:
    static int getAssignedMachine(string username, MayTram* grid[ROWS][COLS], int& loaiMay);
    static bool changePassword(DoublyLinkedList<User*>& dsTaiKhoan, string username, string newPass);
    
    static double getTickRate(int loaiMay, bool isHappyHour);
    
    static bool processTick(DoublyLinkedList<User*>& dsTaiKhoan, string username, double deductionAmount);
    static void returnMachine(string username, int idMay, MayTram* grid[ROWS][COLS]);
};

#endif