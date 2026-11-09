#include "ClientController.h"

ClientController::ClientController(DoublyLinkedList<User*>& users, MayTram* g[ROWS][COLS]) 
    : dsTaiKhoan(users) {
    this->grid = g;
}

Client* ClientController::findClientByPhone(const string& username) {
    for (Node<User*>* p = dsTaiKhoan.getHead(); p != nullptr; p = p->next) {
        if (p->data->getUsername() == username) return dynamic_cast<Client*>(p->data);
    }
    return nullptr;
}

MayTram* ClientController::findMachineByUsername(const string& username) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (grid[i][j]->getCurrentClient() != nullptr && 
                grid[i][j]->getCurrentClient()->getUsername() == username) {
                return grid[i][j];
            }
        }
    }
    return nullptr;
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

int ClientController::getAssignedMachine(string username, int& loaiMay) {
    MayTram* m = findMachineByUsername(username);
    if (m) {
        loaiMay = m->getLoaiMay();
        return m->getId();
    }
    return -1;
}

bool ClientController::changePassword(string username, string newPass) {
    Client* c = findClientByPhone(username);
    if (c) {
        if (newPass == c->getPassword()) return false; 
        c->setPassword(newPass);
        return true;
    }
    return false;
}

bool ClientController::processTick(string username, double deductionAmount) {
    Client* c = findClientByPhone(username);
    if (c) return c->truTien(deductionAmount);
    return false;
}

void ClientController::returnMachine(string username) {
    MayTram* m = findMachineByUsername(username);
    if (m) m->freeMachine();
}