#include "AdminController.h"
#include "../DAL/FileManager.h"
#include <cstdio> 
#include <algorithm>
#include "ClientController.h"

AdminController::AdminController(DoublyLinkedList<User*>& users, DoublyLinkedList<DatLich*>& bookings, DoublyLinkedList<MonAn*>& foods, MayTram* g[ROWS][COLS]) 
    : dsTaiKhoan(users), dsDatLich(bookings), dsMon(foods) {
    this->grid = g;
}

User* AdminController::findUserByPhone(const string& phone) {
    for (Node<User*>* p = dsTaiKhoan.getHead(); p != nullptr; p = p->next) {
        if (p->data->getUsername() == phone) return p->data;
    }
    return nullptr;
}

Client* AdminController::findClientByPhone(const string& phone) {
    return dynamic_cast<Client*>(findUserByPhone(phone)); 
}

Client* AdminController::findClientById(int id) {
    for (Node<User*>* p = dsTaiKhoan.getHead(); p != nullptr; p = p->next) {
        if (p->data->getId() == id) {
            return dynamic_cast<Client*>(p->data);
        }
    }
    return nullptr;
}

MayTram* AdminController::findMachineById(int id) {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (grid[r][c]->getId() == id) return grid[r][c];
        }
    }
    return nullptr;
}

MayTram* AdminController::findMachineByUsername(const string& username) {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (grid[r][c]->getCurrentClient() != nullptr && grid[r][c]->getCurrentClient()->getUsername() == username) {
                return grid[r][c];
            }
        }
    }
    return nullptr;
}

bool AdminController::isValidPhoneNumber(string phone) {
    if (phone.length() != 10 || phone[0] != '0' || phone[1] == '0') return false;
    for (char c : phone) if (c < '0' || c > '9') return false;
    return true;
}

bool AdminController::isDuplicateUser(string phone) {
    return findUserByPhone(phone) != nullptr;
}

int AdminController::addTaiKhoan(string phone, string pass, double initialBalance, string thoiGianLog) {
    if (!isValidPhoneNumber(phone)) return -1; 
    if (initialBalance < 10000) return -3; 
    if (initialBalance > 10000000) return -4; 
    if (isDuplicateUser(phone)) return 0; 

    int newId = 1;
    if (dsTaiKhoan.getSize() > 0) newId = dsTaiKhoan.getTail()->data->getId() + 1;
    dsTaiKhoan.addTail(new Client(newId, phone, pass, initialBalance));
    if (!thoiGianLog.empty()) {
        FileManager::logDoanhThu(thoiGianLog, "NAP", newId, "Nap_Tien_Tao_Tai_Khoan", 1, initialBalance);
    }
    return 1;
}

bool AdminController::napTien(string username, double amount, string thoiGianLog) {
    if (amount <= 0 || amount > 5000000) return false; 
    
    for (Node<User*>* u = dsTaiKhoan.getHead(); u != nullptr; u = u->next) {
        if (u->data->getUsername() == username) {
            Client* c = dynamic_cast<Client*>(u->data);
            if (c != nullptr) {
                if (c->getSoDu() + amount > 10000000) return false; 
                
                c->setSoDu(c->getSoDu() + amount);
                
                if (!thoiGianLog.empty()) {
                    FileManager::logDoanhThu(thoiGianLog, "NAP", c->getId(), "Nap_Tien_Truc_Tiep", 1, amount);
                }
                return true;
            }
        }
    }
    return false;
}

bool AdminController::updatePassword(string username, string newPass) {
    User* u = findUserByPhone(username);
    if (u) {
        u->setPassword(newPass);
        return true;
    }
    return false;
}

int AdminController::assignSeat(string username, int idMay) {
    Client* targetClient = findClientByPhone(username);
    if (!targetClient) return -4; 
    MayTram* m = findMachineById(idMay);
    if (!m) return -1; 
    if (m->getTrangThai() != 0) return -2; 
    double gia1Gio = (m->getLoaiMay() == 0) ? 10000 : ((m->getLoaiMay() == 1) ? 14000 : 20000);
    if (targetClient->getSoDu() < gia1Gio) return -3; 
    m->assignClient(targetClient);
    return 1; 
}

bool AdminController::isMachineInBooking(int idMay) {
    for (Node<DatLich*>* p = dsDatLich.getHead(); p != nullptr; p = p->next) {
        if (p->data->getIdMay() == idMay && p->data->getTrangThai() == 1) {
            return true;
        }
    }
    return false;
}

int AdminController::changeMachine(int srcId, int destId) {
    MayTram* srcM = findMachineById(srcId);
    MayTram* destM = findMachineById(destId);
    if (!srcM || !destM) return -1;
    if (destM->getTrangThai() != 0) return -2;
    Client* cUser = srcM->getCurrentClient();
    if (!cUser) return -1;
    if (isMachineInBooking(srcId)) return -5; 
    double gia1Gio = (destM->getLoaiMay() == 0) ? 10000 : ((destM->getLoaiMay() == 1) ? 14000 : 20000);
    if (cUser->getSoDu() < gia1Gio) return -3;
    int oldState = srcM->getTrangThai();
    destM->assignClient(cUser);
    destM->setTrangThai(oldState);
    srcM->freeMachine();
    srcM->setTrangThai(2); 

    return 1;
}

int AdminController::validateAndAddMonAn(string tenMon, string giaStr) {
    if (tenMon.empty() || giaStr.empty()) return -1; 
    if (giaStr.length() < 4 || giaStr.substr(giaStr.length() - 3) != "000") return -2; 
    try {
        double gia = std::stod(giaStr); 
        if (gia <= 0) return -3; 
        return addMonAn(tenMon, gia); 
    } catch (...) {
        return 0; 
    }
}

int AdminController::addMonAn(string tenMon, double donGia) {
    if (donGia <= 0) return 0;
    int newId = 1;
    if (dsMon.getSize() > 0) newId = dsMon.getTail()->data->getId() + 1;
    dsMon.addTail(new MonAn(newId, tenMon, donGia));
    return 1;
}

bool AdminController::updateMonAn(int idSua, string tenMonMoi, double donGiaMoi) {
    if (donGiaMoi <= 0) return false;
    for (Node<MonAn*>* p = dsMon.getHead(); p != nullptr; p = p->next) {
        if (p->data->getId() == idSua) {
            p->data->setTenMon(tenMonMoi);
            p->data->setDonGia(donGiaMoi);
            return true;
        }
    }
    return false;
}

bool AdminController::deleteMonAn(int idXoa) {
    for (Node<MonAn*>* p = dsMon.getHead(); p != nullptr; p = p->next) {
        if (p->data->getId() == idXoa) {
            delete p->data;
            dsMon.remove(p);
            return true;
        }
    }
    return false;
}

bool AdminController::deleteTaiKhoan(string username, Queue<Order*>& ordersQueue) {
    User* u = findUserByPhone(username);
    if (u) {
        if (u->getRoleId() == 1) return false;
        int targetId = u->getId();
        u->setStatus(0); 

        MayTram* m = findMachineByUsername(username);
        if (m) m->freeMachine();

        Node<DatLich*>* bNode = dsDatLich.getHead();
        while (bNode != nullptr) {
            Node<DatLich*>* nextNode = bNode->next;
            if (bNode->data->getUserId() == targetId) {
                if (bNode->data->getTrangThai() == 0) {
                    MayTram* bookM = findMachineById(bNode->data->getIdMay());
                    if (bookM && bookM->getTrangThai() == 4) bookM->setTrangThai(0);
                }
                delete bNode->data; 
                dsDatLich.remove(bNode);
            }
            bNode = nextNode;
        }

        Queue<Order*> tempQueue;
        while (!ordersQueue.isEmpty()) {
            Order* o = ordersQueue.dequeue();
            if (o->getUserId() == targetId) {
                delete o; 
            } else {
                tempQueue.enqueue(o); 
            }
        }
        while (!tempQueue.isEmpty()) ordersQueue.enqueue(tempQueue.dequeue());

        return true;
    }
    return false;
}

bool AdminController::deleteBooking(int bookingId) {
    for (Node<DatLich*>* node = dsDatLich.getHead(); node != nullptr; node = node->next) {
        DatLich* b = node->data;
        if (b->getId() == bookingId) {
            MayTram* m = findMachineById(b->getIdMay());
            if (m != nullptr) {
                if (b->getTrangThai() == 0 && m->getTrangThai() == 4) {
                    m->setTrangThai(0); 
                }
                else if (b->getTrangThai() == 1 && m->getCurrentClient() != nullptr && m->getCurrentClient()->getId() == b->getUserId()) {
                    m->freeMachine(); 
                    m->setTrangThai(2); 
                }
            }
            delete b; 
            dsDatLich.remove(node);
            return true;
        }
    }
    return false;
}

int AdminController::validateAndSaveBooking(string phone, string idMayStr, string date, string hStr, string mStr, string durationStr, string pass, int currentTotalDays, int selectedBookingId, string thoiGianLog) {
    if (phone.empty() || idMayStr.empty() || date.empty() || hStr.empty() || mStr.empty() || durationStr.empty() || pass.empty()) return -1;
    if (!isValidPhoneNumber(phone)) return -2;
    if (date.length() != 10 || date[2] != '/' || date[5] != '/') return -3;
    
    try {
        int d = stoi(date.substr(0, 2)), m = stoi(date.substr(3, 2)), y = stoi(date.substr(6, 4));
        int idMay = stoi(idMayStr), gio = stoi(hStr), phut = stoi(mStr), thoiLuong = stoi(durationStr);
        
        if (thoiLuong < 60) return -14; 
        if (phut < 0 || phut > 59) return -7;

        int newStart = gio * 60 + phut;
        int newEnd = newStart + thoiLuong;
        
        for (Node<DatLich*>* node = dsDatLich.getHead(); node != nullptr; node = node->next) {
            DatLich* b = node->data;
            if (b->getId() != selectedBookingId && b->getTrangThai() != 2) {
                if (b->getIdMay() == idMay && b->getNgayDat() == date) {
                    int bStart = b->getGioBatDau() * 60 + b->getPhutBatDau();
                    int bEnd = bStart + b->getThoiLuong();
                    if (newStart < bEnd && newEnd > bStart) {
                        return -15; 
                    }
                }
            }
        }

        User* targetUser = findUserByPhone(phone);
        if (targetUser != nullptr && targetUser->getPassword() != pass) return -8; 

        Client* cUser = dynamic_cast<Client*>(targetUser);
        if (cUser == nullptr) {
            int newClientId = 1;
            if (dsTaiKhoan.getSize() > 0) newClientId = dsTaiKhoan.getTail()->data->getId() + 1;
            cUser = new Client(newClientId, phone, pass, 0, 1);
            dsTaiKhoan.addTail(cUser);
        }

        DatLich* existingBooking = nullptr;
        if (selectedBookingId != -1) {
            for (Node<DatLich*>* node = dsDatLich.getHead(); node != nullptr; node = node->next) {
                if (node->data->getId() == selectedBookingId) { existingBooking = node->data; break; }
            }
        }

        if (existingBooking != nullptr) {
            if (existingBooking->getTrangThai() == 2) return -11; 
            if (existingBooking->getTrangThai() == 1) {
                if (idMay != existingBooking->getIdMay() || date != existingBooking->getNgayDat() || gio != existingBooking->getGioBatDau() || phut != existingBooking->getPhutBatDau()) return -10; 
                if (gio * 60 + phut + thoiLuong > 22 * 60) return -13;
                double extraTien = ((thoiLuong - existingBooking->getThoiLuong()) / 60.0) * 20000.0;
                if (extraTien > 0) {
                    if (cUser->getSoDu() + extraTien > 10000000) return -12;
                    cUser->napTien(extraTien);
                    if (!thoiGianLog.empty()) {
                        int phutMuaThem = thoiLuong - existingBooking->getThoiLuong();
                        string chiTiet = "Gia_Han_Booking_May_" + to_string(idMay);
                        FileManager::logDoanhThu(thoiGianLog, "BOK", cUser->getId(), chiTiet, phutMuaThem, extraTien);
                    }
                }
                existingBooking->updateLich(existingBooking->getIdMay(), existingBooking->getNgayDat(), existingBooking->getGioBatDau(), existingBooking->getPhutBatDau(), thoiLuong);
                return 1; 
            }
        }

        if (m < 1 || m > 12) return -5;
        int maxDays = 31;
        if (m == 4 || m == 6 || m == 9 || m == 11) maxDays = 30;
        else if (m == 2) maxDays = ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) ? 29 : 28;
        if (d < 1 || d > maxDays) return -5;

        if (gio < 7 || gio >= 22) return -9; 
        if (gio * 60 + phut + thoiLuong > 22 * 60) return -13;
        if (idMay < 31 || idMay > 40) return -4; 

        int bookTotalDays = y * 365 + d;
        int mDays[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
        if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0) mDays[2] = 29;
        for(int i = 1; i < m; i++) bookTotalDays += mDays[i];

        if (bookTotalDays < currentTotalDays + 1 || bookTotalDays > currentTotalDays + 7) return -6;

        if (existingBooking != nullptr) {
            double extraTien = ((thoiLuong - existingBooking->getThoiLuong()) / 60.0) * 20000.0;
            if (extraTien > 0) {
                if (cUser->getSoDu() + extraTien > 10000000) return -12;
                cUser->napTien(extraTien);
                if (!thoiGianLog.empty()) {
                    int phutMuaThem = thoiLuong - existingBooking->getThoiLuong();
                    string chiTiet = "Sua_Don_Booking_May_" + to_string(idMay);
                    FileManager::logDoanhThu(thoiGianLog, "BOK", cUser->getId(), chiTiet, phutMuaThem, extraTien);
                }
            }
            existingBooking->updateLich(idMay, date, gio, phut, thoiLuong); 
        } else { 
            double tienNap = (thoiLuong / 60.0) * 20000.0;
            if (cUser->getSoDu() + tienNap > 10000000) return -12;
            cUser->napTien(tienNap);
            int newId = 1;
            if (dsDatLich.getSize() > 0) newId = dsDatLich.getTail()->data->getId() + 1;
            int uId = -1;
            Client* c = findClientByPhone(phone);
            if (c != nullptr) uId = c->getId();
            dsDatLich.addTail(new DatLich(newId, uId, idMay, date, gio, phut, thoiLuong, 0));
            if (!thoiGianLog.empty()) {
                double donGiaPhut = 20000.0/60.0; 
                double tongTien = thoiLuong * donGiaPhut; 
                string chiTiet = "Thanh_Toan_Booking_May_" + to_string(idMay);
                FileManager::logDoanhThu(thoiGianLog, "BOK", uId, chiTiet, thoiLuong, tongTien);
            }
        }
        return 1; 
    } catch (...) { return 0; }
}

Vector<string> AdminController::updateBookingLogics(int vDay, int vMonth, int vYear, int vHour, int vMinute) {
    Vector<string> kickedUsers;
    int currMinsTotal = vHour * 60 + vMinute;
    
    int currentTotalDays = vYear * 365 + vDay;
    int mDays[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if ((vYear % 4 == 0 && vYear % 100 != 0) || vYear % 400 == 0) mDays[2] = 29;
    for(int i = 1; i < vMonth; i++) currentTotalDays += mDays[i];
    
    for (Node<DatLich*>* node = dsDatLich.getHead(); node != nullptr; node = node->next) {
        DatLich* b = node->data;
        MayTram* m = findMachineById(b->getIdMay()); 
        if (!m) continue;

        string dateStr = b->getNgayDat();
        int bD = 0, bM = 0, bY = 0;
        if (dateStr.length() == 10) {
            bD = stoi(dateStr.substr(0, 2));
            bM = stoi(dateStr.substr(3, 2));
            bY = stoi(dateStr.substr(6, 4));
        }
        int bookTotalDays = bY * 365 + bD;
        int bMDays[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
        if ((bY % 4 == 0 && bY % 100 != 0) || bY % 400 == 0) bMDays[2] = 29;
        for(int i = 1; i < bM; i++) bookTotalDays += bMDays[i];
        
        if (bookTotalDays < currentTotalDays) {
            if (b->getTrangThai() != 2) {
                if (m->getCurrentClient() != nullptr && m->getCurrentClient()->getId() == b->getUserId()) {
                    kickedUsers.push_back(m->getCurrentClient()->getUsername());
                    m->freeMachine(); 
                    m->setTrangThai(0); 
                }
                b->setTrangThai(2);
            }
        }
        else if (bookTotalDays == currentTotalDays) {
            int bookMins = b->getGioBatDau() * 60 + b->getPhutBatDau();
            
            if (currMinsTotal < bookMins) {
                b->setTrangThai(0); 
                if (currMinsTotal >= bookMins - 15) {
                    if ((m->getTrangThai() == 1 || m->getTrangThai() == 3) && 
                        (m->getCurrentClient() == nullptr || m->getCurrentClient()->getId() != b->getUserId())) {
                        
                        if (m->getCurrentClient() != nullptr) {
                            kickedUsers.push_back(m->getCurrentClient()->getUsername());
                        }
                        m->freeMachine(); 
                        m->setTrangThai(2); 
                    } 
                    else if (m->getTrangThai() == 0) {
                        m->setTrangThai(4); 
                    }
                }
            }
            else if (currMinsTotal >= bookMins && currMinsTotal < bookMins + b->getThoiLuong()) {
                b->setTrangThai(1); 
                Client* cUser = findClientById(b->getUserId());
                if (cUser != nullptr) {
                    if ((m->getTrangThai() == 1 || m->getTrangThai() == 3) && 
                        (m->getCurrentClient() == nullptr || m->getCurrentClient()->getId() != b->getUserId())) {
                        if (m->getCurrentClient() != nullptr) kickedUsers.push_back(m->getCurrentClient()->getUsername());
                        m->freeMachine();
                        m->setTrangThai(2); 
                    }
                    if (m->getTrangThai() == 0 || m->getTrangThai() == 4 || m->getTrangThai() == 2) {
                        MayTram* oldM = findMachineByUsername(cUser->getUsername()); 
                        if (oldM && oldM->getId() != m->getId()) {
                            oldM->freeMachine();
                            oldM->setTrangThai(0);
                        }
                        m->assignClient(cUser);
                        m->setTrangThai(1); 
                    }
                }
            }
            else {
                if (b->getTrangThai() == 1) { 
                    if (m->getCurrentClient() != nullptr && m->getCurrentClient()->getId() == b->getUserId()) {
                        kickedUsers.push_back(m->getCurrentClient()->getUsername());
                        m->freeMachine(); 
                        m->setTrangThai(2); 
                    }
                }
                b->setTrangThai(2); 
            }
        }
    }
    return kickedUsers;
}

Vector<string> AdminController::addVirtualMinutes(int mins, int& vDay, int& vMonth, int& vYear, int& vHour, int& vMinute, bool isHappyHourActive) {
    Vector<string> kickedUsers;
    int skipSeconds = mins * 60;    
    
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (grid[r][c]->getTrangThai() == 1 && grid[r][c]->getCurrentClient() != nullptr) {
                string u = grid[r][c]->getCurrentClient()->getUsername();
                double rRate = ClientController::getTickRate(grid[r][c]->getLoaiMay(), isHappyHourActive);
                double amount = rRate * skipSeconds;
                if (mins > 0) {
                    Client* cUser = findClientByPhone(u);
                    bool ok = (cUser != nullptr) ? cUser->truTien(amount) : false;
                    if (!ok) {
                        kickedUsers.push_back(u); 
                        grid[r][c]->freeMachine();
                        grid[r][c]->setTrangThai(2); 
                    }
                }
            }
        }
    }
    
    if (mins > 0) {
        vMinute += mins;
        while (vMinute >= 60) { vMinute -= 60; vHour++; }
        while (vHour >= 24) { 
            vHour -= 24; vDay++; 
            int maxDays = 31;
            if (vMonth == 4 || vMonth == 6 || vMonth == 9 || vMonth == 11) maxDays = 30;
            else if (vMonth == 2) maxDays = ((vYear % 4 == 0 && vYear % 100 != 0) || (vYear % 400 == 0)) ? 29 : 28;
            if(vDay > maxDays){ vDay = 1; vMonth++; } 
            if(vMonth > 12){ vMonth = 1; vYear++; } 
        }
    } else {
        vMinute += mins;
        while (vMinute < 0) { vMinute += 60; vHour--; }
        while (vHour < 0) { 
            vHour += 24; vDay--; 
            if(vDay < 1){ 
                vMonth--; 
                if(vMonth < 1){ vMonth = 12; vYear--; } 
                int maxDays = 31;
                if (vMonth == 4 || vMonth == 6 || vMonth == 9 || vMonth == 11) maxDays = 30;
                else if (vMonth == 2) maxDays = ((vYear % 4 == 0 && vYear % 100 != 0) || (vYear % 400 == 0)) ? 29 : 28;
                vDay = maxDays;
            } 
        }
    }
    
    if (vHour < 7 || vHour >= 22 || mins >= 1440) {
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                if (grid[r][c]->getTrangThai() != 0) { 
                    if (grid[r][c]->getCurrentClient() != nullptr) {
                        kickedUsers.push_back(grid[r][c]->getCurrentClient()->getUsername()); 
                        grid[r][c]->freeMachine();
                    }
                    grid[r][c]->setTrangThai(0); 
                }
            }
        }
    }
    
    Vector<string> bookingKicked = updateBookingLogics(vDay, vMonth, vYear, vHour, vMinute);
    for (int i = 0; i < bookingKicked.size(); i++) {
        if (!kickedUsers.contains(bookingKicked[i])) {
            kickedUsers.push_back(bookingKicked[i]);
        }
    }
    if (mins > 0 || (vHour < 7 || vHour >= 22)) {
        FileManager::saveGrid("Data/MayTram.txt", grid);
        FileManager::saveBookings("Data/Booking.txt", dsDatLich);
    }
    return kickedUsers;
}

void AdminController::thanhToanBill(Queue<Order*>& q, DoublyLinkedList<MonAn*>& dsMon, int userId, string thoiGianBill, string thoiGianLog) {
    bool allPaid = true;
    for (Node<Order*>* p = q.getFront(); p != nullptr; p = p->next) {
        if (p->data->getUserId() == userId && p->data->getThoiGian() == thoiGianBill) {
            if (p->data->getPaid() == 0) allPaid = false;
        }
    }
    
    int newState = allPaid ? 0 : 1; 

    if (newState == 1) {
        for (Node<Order*>* p = q.getFront(); p != nullptr; p = p->next) {
            if (p->data->getUserId() == userId && p->data->getThoiGian() == thoiGianBill && p->data->getPaid() == 0) {
                p->data->setPaid(1); 
                
                string tenMon = "Unknown";
                double thanhTienGoc = 0;
                
                for (Node<MonAn*>* m = dsMon.getHead(); m != nullptr; m = m->next) {
                    if (m->data->getId() == p->data->getIdMon()) {
                        tenMon = m->data->getTenMon();
                        thanhTienGoc = m->data->getDonGia() * p->data->getSoLuong();
                        break;
                    }
                }
                GiaHoaDon* boTinh = (p->data->getVoucherUsed() == 1)
                    ? static_cast<GiaHoaDon*>(new GiaHoaDonVoucher10())
                    : static_cast<GiaHoaDon*>(new GiaHoaDonGoc());
                double thucTra = boTinh->tinhTongCong(thanhTienGoc);
                delete boTinh;

                FileManager::logDoanhThu(thoiGianLog, "FNB", userId, tenMon, p->data->getSoLuong(), thucTra);
            }
        }
    } 
    else {
        for (Node<Order*>* p = q.getFront(); p != nullptr; p = p->next) {
            if (p->data->getUserId() == userId && p->data->getThoiGian() == thoiGianBill) {
                p->data->setPaid(0);
            }
        }
    }
}

void AdminController::giaoHangBill(Queue<Order*>& q, int userId, string thoiGian) {
    bool allDelivered = true;
    for (Node<Order*>* p = q.getFront(); p != nullptr; p = p->next) {
        if (p->data->getUserId() == userId && p->data->getThoiGian() == thoiGian) {
            if (p->data->getStatus() == 0) allDelivered = false;
        }
    }
    int newState = allDelivered ? 0 : 1; 
    for (Node<Order*>* p = q.getFront(); p != nullptr; p = p->next) {
        if (p->data->getUserId() == userId && p->data->getThoiGian() == thoiGian) {
            p->data->setStatus(newState);
        }
    }
}

int AdminController::deleteBill(Queue<Order*>& q, int userId, string thoiGian) {
    for (Node<Order*>* p = q.getFront(); p != nullptr; p = p->next) {
        if (p->data->getUserId() == userId && p->data->getThoiGian() == thoiGian) {
            if (p->data->getPaid() == 1) return -1; 
            if (p->data->getStatus() == 1) return -2;
        }
    }
    Queue<Order*> temp;
    while (!q.isEmpty()) {
        Order* o = q.dequeue();
        if (o->getUserId() == userId && o->getThoiGian() == thoiGian) { delete o; } 
        else { temp.enqueue(o); }
    }
    while (!temp.isEmpty()) q.enqueue(temp.dequeue());
    return 1;
}

int AdminController::updateOrderQuantity(Queue<Order*>& q, int orderId, int newQty) {
    for (Node<Order*>* p = q.getFront(); p != nullptr; p = p->next) {
        if (p->data->getId() == orderId) {
            if (p->data->getPaid() == 1) return -1; 
            if (p->data->getStatus() == 1) return -2;
            p->data->setSoLuong(newQty);
            return 1;
        }
    }
    return 0; 
}

int AdminController::deleteOrder(Queue<Order*>& q, int orderId) {
    for (Node<Order*>* p = q.getFront(); p != nullptr; p = p->next) {
        if (p->data->getId() == orderId) {
            if (p->data->getPaid() == 1) return -1;
            if (p->data->getStatus() == 1) return -2;
        }
    }
    Queue<Order*> temp;
    bool found = false;
    while (!q.isEmpty()) {
        Order* o = q.dequeue();
        if (o->getId() == orderId) { delete o; found = true; }
        else { temp.enqueue(o); }
    }
    while (!temp.isEmpty()) q.enqueue(temp.dequeue());
    return found ? 1 : 0;
}

Vector<HoaDonDTO> AdminController::getDanhSachHoaDon(Queue<Order*>& ordersQueue) {
    Vector<HoaDonDTO> bills;
    for (Node<Order*>* p = ordersQueue.getFront(); p != nullptr; p = p->next) {
        Order* o = p->data;
        bool found = false;
        for (int k = 0; k < bills.size(); k++) {
            if (bills[k].userId == o->getUserId() && bills[k].thoiGian == o->getThoiGian()) {
                double gia = 0;
                for (Node<MonAn*>* m = dsMon.getHead(); m != nullptr; m = m->next) {
                    if (m->data->getId() == o->getIdMon()) { gia = m->data->getDonGia(); break; }
                }
                bills[k].thanhTien += gia * o->getSoLuong();
                if (o->getVoucherUsed() == 1) bills[k].coVoucher = 1;
                if (o->getStatus() == 0) bills[k].trangThai = 0; 
                if (o->getPaid() == 0) bills[k].thanhToan = 0;   
                found = true; break;
            }
        }
        if (!found) {
            double giaBanDau = 0;
            for (Node<MonAn*>* m = dsMon.getHead(); m != nullptr; m = m->next) {
                if (m->data->getId() == o->getIdMon()) { giaBanDau = m->data->getDonGia() * o->getSoLuong(); break; }
            }
            HoaDonDTO b(o->getUserId(), o->getThoiGian(), o->getMachineId(), giaBanDau, 0, giaBanDau, o->getStatus(), o->getPaid(), o->getVoucherUsed());
            bills.push_back(b);
        }
    }
    for (int k = 0; k < bills.size(); k++) {
        GiaHoaDon* boTinh = (bills[k].coVoucher == 1)
            ? static_cast<GiaHoaDon*>(new GiaHoaDonVoucher10())
            : static_cast<GiaHoaDon*>(new GiaHoaDonGoc());
        bills[k].giamGia = boTinh->tinhGiamGia(bills[k].thanhTien);
        bills[k].tongTien = boTinh->tinhTongCong(bills[k].thanhTien);
        delete boTinh;
    }
    return bills;
}

int AdminController::tinhTongSoNgay(int d, int m, int y) const {
    int prevY = y - 1;
    int leapDays = prevY / 4 - prevY / 100 + prevY / 400;
    int total = y * 365 + leapDays + d;
    int mDays[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) mDays[2] = 29;
    for (int i = 1; i < m; i++) total += mDays[i];
    return total;
}

void AdminController::layKhoangTuanTongKet(int vDay, int vMonth, int vYear, int vHour, int& startDays, int& endDays, int& tuanTrongThang, int& thangHienThi) const {
    int mocThuHaiGoc = tinhTongSoNgay(16, 11, 2026);
    int ngayHienTai = tinhTongSoNgay(vDay, vMonth, vYear);
    int diff = ngayHienTai - mocThuHaiGoc;

    int tuanTongKet = -1;
    if (diff >= 7) {
        tuanTongKet = (diff / 7) - 1;
        if (diff % 7 == 6 && vHour >= 22) {
            tuanTongKet = diff / 7;
        }
    } else if (diff == 6 && vHour >= 22) {
        tuanTongKet = 0;
    }

    if (tuanTongKet < 0) {
        startDays = -1;
        endDays = -1;
        tuanTrongThang = 3;
        thangHienThi = 11;
        return;
    }

    startDays = mocThuHaiGoc + tuanTongKet * 7;
    endDays = startDays + 6;

    int d = 16, m = 11, y = 2026;
    int addDays = tuanTongKet * 7;
    while (addDays > 0) {
        int maxDays = 31;
        if (m == 4 || m == 6 || m == 9 || m == 11) maxDays = 30;
        else if (m == 2) maxDays = ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) ? 29 : 28;
        d++;
        if (d > maxDays) { d = 1; m++; if (m > 12) { m = 1; y++; } }
        addDays--;
    }

    tuanTrongThang = (d - 1) / 7 + 1;
    if (tuanTrongThang > 4) tuanTrongThang = 4;
    thangHienThi = m;
}

string AdminController::layTieuDeBXH(int vDay, int vMonth, int vYear, int vHour) const {
    int startDays = -1, endDays = -1, tuan = 3, thang = 11;
    layKhoangTuanTongKet(vDay, vMonth, vYear, vHour, startDays, endDays, tuan, thang);
    return "BẢNG XẾP HẠNG TUẦN " + to_string(tuan) + " THÁNG " + to_string(thang);
}

Vector<HangXepHang> AdminController::thongKeXepHangTuan(int vDay, int vMonth, int vYear, int vHour) {
    Vector<HangXepHang> danhSach;
    int startDays = -1, endDays = -1, tuan = 0, thang = 0;
    layKhoangTuanTongKet(vDay, vMonth, vYear, vHour, startDays, endDays, tuan, thang);
    if (startDays == -1) return danhSach;

    Vector<GiaoDichDoanhThu> dsLog;
    FileManager::loadDoanhThu("Data/DoanhThu.txt", dsLog);

    for (Node<User*>* p = dsTaiKhoan.getHead(); p != nullptr; p = p->next) {
        if (p->data->getRoleId() == 2 && p->data->getStatus() == 1) {
            int uId = p->data->getId();
            double tong = 0;
            int mocCuoi = -1;

            for (int i = 0; i < dsLog.size(); i++) {
                if (dsLog[i].getUserId() == uId && dsLog[i].getThanhTien() > 0) {
                    string tg = dsLog[i].getThoiGian();
                    if (tg.length() >= 8) {
                        try {
                            int ly = stoi(tg.substr(0, 4));
                            int lm = stoi(tg.substr(4, 2));
                            int ld = stoi(tg.substr(6, 2));
                            int logDays = tinhTongSoNgay(ld, lm, ly);
                            if (logDays >= startDays && logDays <= endDays) {
                                tong += dsLog[i].getThanhTien();
                                mocCuoi = dsLog[i].getThuTuLog();
                            }
                        } catch (...) {}
                    }
                }
            }

            if (tong > 0) {
                danhSach.push_back(HangXepHang(0, uId, p->data->getUsername(), tong, mocCuoi));
            }
        }
    }

    for (int i = 0; i < danhSach.size() - 1; i++) {
        for (int j = i + 1; j < danhSach.size(); j++) {
            bool doiCho = false;
            if (danhSach[j].getTongDoanhThu() > danhSach[i].getTongDoanhThu()) {
                doiCho = true;
            } else if (danhSach[j].getTongDoanhThu() == danhSach[i].getTongDoanhThu()) {
                if (danhSach[j].getMocDatCuoi() < danhSach[i].getMocDatCuoi()) {
                    doiCho = true;
                }
            }
            if (doiCho) {
                HangXepHang tmp = danhSach[i];
                danhSach[i] = danhSach[j];
                danhSach[j] = tmp;
            }
        }
    }

    for (int i = 0; i < danhSach.size(); i++) {
        danhSach[i].setHang(i + 1);
    }

    return danhSach;
}

Vector<HangXepHang> AdminController::layTop5BXH(int vDay, int vMonth, int vYear, int vHour) {
    Vector<HangXepHang> tatCa = thongKeXepHangTuan(vDay, vMonth, vYear, vHour);
    Vector<HangXepHang> top5;
    for (int i = 0; i < tatCa.size() && i < 5; i++) {
        top5.push_back(tatCa[i]);
    }
    return top5;
}

int AdminController::layHangHienTaiCuaKhach(int userId, int vDay, int vMonth, int vYear, int vHour, double& doanhThuTuan) {
    doanhThuTuan = 0;
    Vector<HangXepHang> tatCa = thongKeXepHangTuan(vDay, vMonth, vYear, vHour);
    for (int i = 0; i < tatCa.size(); i++) {
        if (tatCa[i].getUserId() == userId) {
            doanhThuTuan = tatCa[i].getTongDoanhThu();
            return tatCa[i].getHang();
        }
    }
    return -1;
}

string AdminController::dinhDangSDT(const string& phone, bool hienDayDu) const {
    if (hienDayDu) return phone;
    if (phone.length() >= 7) {
        return phone.substr(0, 4) + "***" + phone.substr(phone.length() - 3);
    }
    return phone;
}

bool AdminController::daDungVoucherTrongTuan(int userId, Queue<Order*>& ordersQueue, int vDay, int vMonth, int vYear, int vHour) const {
    int startDays = -1, endDays = -1, tuan = 0, thang = 0;
    layKhoangTuanTongKet(vDay, vMonth, vYear, vHour, startDays, endDays, tuan, thang);
    if (startDays == -1) return false;

    int hieuLucTu = endDays;
    int hieuLucDen = endDays + 7;

    for (Node<Order*>* p = ordersQueue.getFront(); p != nullptr; p = p->next) {
        Order* o = p->data;
        if (o->getUserId() == userId && o->getVoucherUsed() == 1) {
            string tg = o->getThoiGian();
            if (tg.length() >= 18) {
                try {
                    string datePart = tg.substr(tg.length() - 10);
                    int od = stoi(datePart.substr(0, 2));
                    int om = stoi(datePart.substr(3, 2));
                    int oy = stoi(datePart.substr(6, 4));
                    int orderDays = tinhTongSoNgay(od, om, oy);
                    if (orderDays >= hieuLucTu && orderDays <= hieuLucDen) {
                        return true;
                    }
                } catch (...) {}
            }
        }
    }
    return false;
}

Voucher AdminController::layThongTinVoucherCuaKhach(int userId, Queue<Order*>& ordersQueue, int vDay, int vMonth, int vYear, int vHour) {
    int startDays = -1, endDays = -1, tuan = 0, thang = 0;
    layKhoangTuanTongKet(vDay, vMonth, vYear, vHour, startDays, endDays, tuan, thang);
    if (startDays == -1) return Voucher(userId, 0.1, -1, -1, true);

    bool inTop5 = false;
    Vector<HangXepHang> top5 = layTop5BXH(vDay, vMonth, vYear, vHour);
    for (int i = 0; i < top5.size(); i++) {
        if (top5[i].getUserId() == userId) {
            inTop5 = true;
            break;
        }
    }
    if (!inTop5) return Voucher(userId, 0.1, -1, -1, true);

    bool daDung = daDungVoucherTrongTuan(userId, ordersQueue, vDay, vMonth, vYear, vHour);
    return Voucher(userId, 0.1, endDays, endDays + 7, daDung);
}

bool AdminController::coTheDungVoucher(int userId, Queue<Order*>& ordersQueue, int vDay, int vMonth, int vYear, int vHour) {
    Voucher vc = layThongTinVoucherCuaKhach(userId, ordersQueue, vDay, vMonth, vYear, vHour);
    int ngayHienTai = tinhTongSoNgay(vDay, vMonth, vYear);
    return vc.conHieuLuc(ngayHienTai);
}