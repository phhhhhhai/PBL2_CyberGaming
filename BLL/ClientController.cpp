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

int ClientController::validateLogin(string username, string password, int currentHour, User*& loggedInUser) {
    for (Node<User*>* p = dsTaiKhoan.getHead(); p != nullptr; p = p->next) {
        if (p->data->getUsername() == username) {
            if (p->data->getStatus() == 0) return -1;
            if (p->data->getPassword() != password) return -2; 

            loggedInUser = p->data;
            bool isAdmin = (p->data->getRoleId() == 1);
            if (isAdmin) return 1; 

            if (currentHour < 7 || currentHour >= 22) return -3; 
            
            Client* cObj = dynamic_cast<Client*>(p->data);
            if (cObj != nullptr && cObj->getSoDu() <= 0) return -4; 

            bool isAssigned = false;
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (grid[r][c]->getCurrentClient() != nullptr && 
                        grid[r][c]->getCurrentClient()->getUsername() == username) {
                        isAssigned = true; 
                        
                        if (grid[r][c]->getTrangThai() == 3) {
                            grid[r][c]->setTrangThai(1);
                        }
                        break;
                    }
                }
            }
            if (!isAssigned) return -5; 
            return 2;
        }
    }
    return -1; 
}

int ClientController::datMon(string username, DoublyLinkedList<CartItem*>& cart, Queue<Order*>& ordersQueue, string thoiGian, bool useVoucher) {
    MayTram* m = findMachineByUsername(username);
    Client* c = findClientByPhone(username);
    if (!m || !c) return -1; 

    int nextOrderId = 1;
    for (Node<Order*>* o = ordersQueue.getFront(); o != nullptr; o = o->next) {
        if (o->data->getId() >= nextOrderId) nextOrderId = o->data->getId() + 1;
    }

    int vcFlag = useVoucher ? 1 : 0;
    for (Node<CartItem*>* ci = cart.getHead(); ci != nullptr; ci = ci->next) {
        ordersQueue.enqueue(new Order(nextOrderId++, m->getId(), c->getId(), ci->data->getFoodId(), ci->data->getSoLuong(), 0, 0, thoiGian, vcFlag));
    }
    cart.clear();
    return 1;
}