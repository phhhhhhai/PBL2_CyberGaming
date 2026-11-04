#include "ClientController.h"

int ClientController::getAssignedMachine(string username, MayTram* grid[ROWS][COLS], int& loaiMay) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (grid[i][j]->getCurrentClient() != nullptr && 
                grid[i][j]->getCurrentClient()->getUsername() == username) {
                loaiMay = grid[i][j]->getLoaiMay();
                return grid[i][j]->getId();
            }
        }
    }
    return -1; // Chưa được xếp máy
}

bool ClientController::changePassword(DoublyLinkedList<User*>& dsTaiKhoan, string username, string newPass) {
    for (Node<User*>* p = dsTaiKhoan.getHead(); p != nullptr; p = p->next) {
        if (p->data->getUsername() == username) {
            if (newPass == p->data->getPassword()) return false; // Trùng MK cũ
            p->data->setPassword(newPass);
            return true;
        }
    }
    return false;
}

double ClientController::getTickRate(int loaiMay, bool isHappyHour) {
    GiaMay* boTinhTien = nullptr;
    if (loaiMay == 0) boTinhTien = new GiaStandard();
    else if (loaiMay == 1) boTinhTien = new GiaVIP();
    else if (loaiMay == 2) boTinhTien = new GiaStream();

    double rate = 0;
    if (boTinhTien != nullptr) {
        rate = boTinhTien->tinhTien1Giay(isHappyHour); 
        delete boTinhTien;
    }
    return rate;
}

bool ClientController::processTick(DoublyLinkedList<User*>& dsTaiKhoan, string username, double deductionAmount) {
    for (Node<User*>* p = dsTaiKhoan.getHead(); p != nullptr; p = p->next) {
        if (p->data->getUsername() == username) {
            Client* c = dynamic_cast<Client*>(p->data);
            if (c) {
                // Gọi hàm truTien nội tại của class Client
                return c->truTien(deductionAmount); 
            }
        }
    }
    return false;
}

void ClientController::returnMachine(string username, int idMay, MayTram* grid[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (grid[i][j]->getId() == idMay && 
                grid[i][j]->getCurrentClient() != nullptr && 
                grid[i][j]->getCurrentClient()->getUsername() == username) {
                grid[i][j]->freeMachine(); // Trả máy, ngắt liên kết con trỏ
                return;
            }
        }
    }
}