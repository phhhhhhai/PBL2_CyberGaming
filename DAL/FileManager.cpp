#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

void FileManager::loadTaiKhoan(const string& filename, DoublyLinkedList<User*>& dsTaiKhoan) {
    ifstream file(filename);
    if (!file.is_open()) return;

    int id, roleId, status;
    string u, p; 
    double soDu;
    while (file >> id >> u >> p >> roleId >> soDu >> status) {
        if (roleId == 1) dsTaiKhoan.addTail(new Admin(id, u, p, status));
        else dsTaiKhoan.addTail(new Client(id, u, p, soDu, status));
    }
    file.close();
}

void FileManager::saveTaiKhoan(const string& filename, DoublyLinkedList<User*>& dsTaiKhoan) {
    ofstream file(filename);
    for (Node<User*>* p = dsTaiKhoan.getHead(); p != nullptr; p = p->next) {
        file << p->data->getId() << " " 
             << p->data->getUsername() << " " 
             << p->data->getPassword() << " " 
             << p->data->getRoleId() << " ";
        
        Client* c = dynamic_cast<Client*>(p->data);
        if (c != nullptr) file << c->getSoDu() << " ";
        else file << "0 "; 
        
        file << p->data->getStatus() << "\n";
    }
    file.close();
}

void FileManager::loadMenu(const string& filename, DoublyLinkedList<MonAn*>& dsMonAn) {
    ifstream file(filename);
    if (!file.is_open()) return;

    int id;
    string ten;
    double gia;
    while (file >> id >> ten >> gia) {
        dsMonAn.addTail(new MonAn(id, ten, gia));
    }
    file.close();
}

void FileManager::saveMenu(const string& filename, DoublyLinkedList<MonAn*>& dsMonAn) {
    ofstream file(filename);
    for (Node<MonAn*>* p = dsMonAn.getHead(); p != nullptr; p = p->next) {
        file << p->data->getId() << " " 
             << p->data->getTenMon() << " " 
             << p->data->getDonGia() << "\n";
    }
    file.close();
}

void FileManager::loadGrid(const string& filename, MayTram* grid[ROWS][COLS], DoublyLinkedList<User*>& dsTaiKhoan) {
    ifstream file(filename);
    if (!file.is_open()) return;

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int id, loaiMay, trangThai, userId; 

            file >> id >> loaiMay >> trangThai >> userId;
            
            grid[i][j] = new MayTram(id, loaiMay, trangThai);

            if (userId != 0) { 
                for (Node<User*>* p = dsTaiKhoan.getHead(); p != nullptr; p = p->next) {
                    if (p->data->getId() == userId) { 
                        // Ép kiểu để lấy đúng đối tượng Client
                        Client* c = dynamic_cast<Client*>(p->data);
                        if (c != nullptr) grid[i][j]->assignClient(c);
                        break;
                    }
                }
            }
        }
    }
    file.close();
}

void FileManager::saveGrid(const string& filename, MayTram* grid[ROWS][COLS]) {
    ofstream file(filename);
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            file << grid[i][j]->getId() << " " 
                 << grid[i][j]->getLoaiMay() << " " 
                 << grid[i][j]->getTrangThai() << " ";
            
            Client* c = grid[i][j]->getCurrentClient();
            if (c != nullptr) file << c->getId() << "\n";
            else file << "0\n";
        }
    }
    file.close();
}

void FileManager::loadOrders(const string& filename, Queue<Order*>& ordersQueue) {
    ifstream file(filename);
    if (!file.is_open()) return;
    int id, mId, uId, idMon, sl, status, paid;
    string tg;
    while (file >> id >> mId >> uId >> idMon >> sl >> status >> paid) {
        getline(file >> ws, tg);
        while (!tg.empty() && (tg.back() == '\r' || tg.back() == '\n' || tg.back() == ' ')) {
            tg.pop_back();
        }
        int vc = 0;
        string tag = " #V10";
        if (tg.length() >= tag.length() && tg.substr(tg.length() - tag.length()) == tag) {
            vc = 1;
            tg = tg.substr(0, tg.length() - tag.length());
        }
        ordersQueue.enqueue(new Order(id, mId, uId, idMon, sl, status, paid, tg, vc));
    }
    file.close();
}

void FileManager::saveOrders(const string& filename, Queue<Order*>& ordersQueue) {
    ofstream file(filename);
    for (Node<Order*>* p = ordersQueue.getFront(); p != nullptr; p = p->next) {
        Order* o = p->data;
        string tg = o->getThoiGian().empty() ? "Unknown" : o->getThoiGian();
        if (o->getVoucherUsed() == 1) tg += " #V10";
        file << o->getId() << " " << o->getMachineId() << " " << o->getUserId() << " "
             << o->getIdMon() << " " << o->getSoLuong() << " " << o->getStatus() << " " 
             << o->getPaid() << " " << tg << "\n";
    }
    file.close();
}

void FileManager::loadBookings(const string& filename, DoublyLinkedList<DatLich*>& dsDatLich) {
    ifstream file(filename);
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        int id, uId, mId, g, p, tl, tt; 
        string ngay;
        if (ss >> id >> uId >> mId >> ngay >> g >> p >> tl >> tt) {
            dsDatLich.addTail(new DatLich(id, uId, mId, ngay, g, p, tl, tt));
        }
    }
    file.close();
}

void FileManager::saveBookings(const string& filename, DoublyLinkedList<DatLich*>& dsDatLich) {
    ofstream file(filename);
    for (Node<DatLich*>* p = dsDatLich.getHead(); p != nullptr; p = p->next) {
        DatLich* b = p->data;
        file << b->getId() << " " << b->getUserId() << " " << b->getIdMay() << " "
             << b->getNgayDat() << " " << b->getGioBatDau() << " " << b->getPhutBatDau() << " "
             << b->getThoiLuong() << " " << b->getTrangThai() << "\n";
    }
    file.close();
}

void FileManager::logDoanhThu(string thoiGian, string loai, int userId, string chiTiet, int soLuong, double thanhTien) {
    ofstream file("Data/DoanhThu.txt", ios::app); 
    if (file.is_open()) {
        for (int i = 0; i < thoiGian.length(); i++) if (thoiGian[i] == ' ' || thoiGian[i] == '/') thoiGian[i] = '_';
        for (int i = 0; i < chiTiet.length(); i++) if (chiTiet[i] == ' ') chiTiet[i] = '_';
        
        file << thoiGian << " " << loai << " " << userId << " " << chiTiet << " " << soLuong << " " << (long long)thanhTien << "\n";
        file.close();
    }
}

void FileManager::loadDoanhThu(const string& filename, Vector<GiaoDichDoanhThu>& dsGiaoDich) {
    ifstream file(filename);
    if (!file.is_open()) return;
    string tg, loai, chiTiet;
    int uId, sl;
    double tt;
    int idx = 0;
    while (file >> tg >> loai >> uId >> chiTiet >> sl >> tt) {
        dsGiaoDich.push_back(GiaoDichDoanhThu(tg, loai, uId, chiTiet, sl, tt, idx++));
    }
    file.close();
}