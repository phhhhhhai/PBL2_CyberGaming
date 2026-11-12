#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

void FileManager::loadTaiKhoan(const string& filename, DoublyLinkedList<User*>& dsTaiKhoan) {
    ifstream file(filename);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);

        int id, roleId, status;
        string u, p; 
        double soDu;
        int expDate = 0, used = 0;

        // Đọc 6 cột cơ bản (có ở cả Admin và Client)
        if (ss >> id >> u >> p >> roleId >> soDu >> status) {
            // Thử đọc thêm 2 cột Voucher (chỉ có ở Client mới)
            if (!(ss >> expDate >> used)) {
                expDate = 0; used = 0;
            }

            if (roleId == 1) {
                dsTaiKhoan.addTail(new Admin(id, u, p, status));
            } else {
                Client* c = new Client(id, u, p, soDu, status);
                if (expDate > 0) {
                    c->setVoucher(new Voucher(10, expDate, used == 1));
                }
                dsTaiKhoan.addTail(c);
            }
        }
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
        if (c != nullptr) {
            file << c->getSoDu() << " " << p->data->getStatus() << " ";
            if (c->getVoucher() != nullptr) {
                file << c->getVoucher()->getNgayHetHan() << " " << (c->getVoucher()->isDaSuDung() ? 1 : 0) << "\n";
            } else {
                file << "0 0\n";
            }
        } else {
            file << "0 " << p->data->getStatus() << " 0 0\n";
        }
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
        ordersQueue.enqueue(new Order(id, mId, uId, idMon, sl, status, paid, tg));
    }
    file.close();
}

void FileManager::saveOrders(const string& filename, Queue<Order*>& ordersQueue) {
    ofstream file(filename);
    for (Node<Order*>* p = ordersQueue.getFront(); p != nullptr; p = p->next) {
        Order* o = p->data;
        file << o->getId() << " " << o->getMachineId() << " " << o->getUserId() << " "
             << o->getIdMon() << " " << o->getSoLuong() << " " << o->getStatus() << " " 
             << o->getPaid() << " " << (o->getThoiGian().empty() ? "Unknown" : o->getThoiGian()) << "\n";
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

Vector<RankItem> FileManager::calculateTop5FromLog(const string& filename, int currentTotalDays) {
    Vector<RankItem> list;
    ifstream file(filename);
    if (!file.is_open()) return list;

    string tg, loai, chiTiet;
    int uId, sl;
    double tien;

    while (file >> tg >> loai >> uId >> chiTiet >> sl >> tien) {
        if (uId <= 0 || tg.length() < 8) continue;
        
        int y = stoi(tg.substr(0, 4));
        int m = stoi(tg.substr(4, 2));
        int d = stoi(tg.substr(6, 2));
        
        int logDays = y * 365 + d;
        int mDays[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0) mDays[2] = 29;
        for (int i = 1; i < m; i++) logDays += mDays[i];

        if (currentTotalDays - logDays <= 7 && currentTotalDays - logDays >= 0) {
            bool found = false;
            for (int i = 0; i < list.size(); i++) {
                if (list[i].userId == uId) {
                    list[i].tongChiTieu += tien;
                    found = true;
                    break;
                }
            }
            if (!found) {
                RankItem item;
                item.userId = uId;
                item.tongChiTieu = tien;
                list.push_back(item);
            }
        }
    }
    file.close();

    for (int i = 0; i < list.size() - 1; i++) {
        for (int j = i + 1; j < list.size(); j++) {
            if (list[j].tongChiTieu > list[i].tongChiTieu) {
                RankItem temp = list[i];
                list[i] = list[j];
                list[j] = temp;
            }
        }
    }

    Vector<RankItem> top5;
    int limit = (list.size() < 5) ? list.size() : 5;
    for (int i = 0; i < limit; i++) {
        top5.push_back(list[i]);
    }
    
    return top5;
}

void FileManager::saveBXHTuan(const string& filename, Vector<RankItem>& top5) {
    ofstream file(filename);
    for (int i = 0; i < top5.size(); i++) {
        file << top5[i].userId << " " << (long long)top5[i].tongChiTieu << "\n";
    }
    file.close();
}

Vector<RankItem> FileManager::loadBXHTuan(const string& filename) {
    Vector<RankItem> list;
    ifstream file(filename);
    if (!file.is_open()) return list;

    int uId;
    double tien;
    while (file >> uId >> tien) {
        RankItem item;
        item.userId = uId;
        item.tongChiTieu = tien;
        list.push_back(item);
    }
    file.close();
    return list;
}