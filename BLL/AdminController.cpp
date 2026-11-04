#include "AdminController.h"

bool AdminController::isValidPhoneNumber(string phone) {
    if (phone.length() != 10 || phone[0] != '0' || phone[1] == '0') return false;
    for (char c : phone) if (c < '0' || c > '9') return false;
    return true;
}

bool AdminController::isDuplicateUser(string phone, DoublyLinkedList<User*>& dsTaiKhoan) {
    for (Node<User*>* p = dsTaiKhoan.getHead(); p != nullptr; p = p->next) {
        if (p->data->getUsername() == phone) return true;
    }
    return false;
}

int AdminController::addTaiKhoan(DoublyLinkedList<User*>& dsTaiKhoan, string phone, string pass, double initialBalance) {
    if (!isValidPhoneNumber(phone)) return -1; 
    if (initialBalance < 10000) return -3; 
    if (isDuplicateUser(phone, dsTaiKhoan)) return 0; 

    int newId = 1;
    if (dsTaiKhoan.getSize() > 0) {
        newId = dsTaiKhoan.getTail()->data->getId() + 1;
    }
    dsTaiKhoan.addTail(new Client(newId, phone, pass, initialBalance));
    return 1;
}

bool AdminController::napTien(DoublyLinkedList<User*>& dsTaiKhoan, string username, double amount) {
    if (amount <= 0) return false;
    for (Node<User*>* p = dsTaiKhoan.getHead(); p != nullptr; p = p->next) {
        if (p->data->getUsername() == username) {
            // Ép kiểu động sang Client và gọi hàm nghiệp vụ
            Client* c = dynamic_cast<Client*>(p->data);
            if (c) {
                c->napTien(amount);
                return true;
            }
        }
    }
    return false;
}

bool AdminController::updatePassword(DoublyLinkedList<User*>& dsTaiKhoan, string username, string newPass) {
    for (Node<User*>* p = dsTaiKhoan.getHead(); p != nullptr; p = p->next) {
        if (p->data->getUsername() == username) {
            p->data->setPassword(newPass); // Đóng gói setter
            return true;
        }
    }
    return false;
}

bool AdminController::deleteTaiKhoan(DoublyLinkedList<User*>& dsTaiKhoan, string username, MayTram* grid[ROWS][COLS]) {
    for (Node<User*>* p = dsTaiKhoan.getHead(); p != nullptr; p = p->next) {
        if (p->data->getUsername() == username) {
            // 1. Giải phóng máy nếu đang ngồi
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (grid[r][c]->getCurrentClient() != nullptr && 
                        grid[r][c]->getCurrentClient()->getUsername() == username) {
                        grid[r][c]->freeMachine(); // Đóng gói
                    }
                }
            }
            // 2. Xóa object và gỡ node khỏi DSLK
            delete p->data;
            dsTaiKhoan.remove(p);
            return true;
        }
    }
    return false;
}

int AdminController::assignSeat(string username, int idMay, DoublyLinkedList<User*>& dsTaiKhoan, MayTram* grid[ROWS][COLS]) {
    Client* targetClient = nullptr;
    for (Node<User*>* p = dsTaiKhoan.getHead(); p != nullptr; p = p->next) {
        if (p->data->getUsername() == username) {
            targetClient = dynamic_cast<Client*>(p->data);
            break;
        }
    }
    if (!targetClient) return -4; // Không tìm thấy

    // Check tài chính
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (grid[i][j]->getId() == idMay) {
                if (grid[i][j]->getTrangThai() != 0) return -2; // Máy không trống

                double gia1Gio = (grid[i][j]->getLoaiMay() == 0) ? 10000 : ((grid[i][j]->getLoaiMay() == 1) ? 14000 : 20000);
                if (targetClient->getSoDu() < gia1Gio) return -3; 

                // Gán máy (Sử dụng con trỏ object thay vì string)
                grid[i][j]->assignClient(targetClient);
                return 1; 
            }
        }
    }
    return -1; 
}

int AdminController::validateAndAddMonAn(DoublyLinkedList<MonAn*>& dsMon, string tenMon, string giaStr) {
    if (tenMon.empty() || giaStr.empty()) return -1; 
    if (giaStr.length() < 4 || giaStr.substr(giaStr.length() - 3) != "000") return -2; 
    try {
        double gia = std::stod(giaStr); 
        if (gia <= 0) return -3; 
        return addMonAn(dsMon, tenMon, gia); 
    } catch (...) {
        return 0; 
    }
}

int AdminController::addMonAn(DoublyLinkedList<MonAn*>& dsMon, string tenMon, double donGia) {
    if (donGia <= 0) return 0;
    int newId = 1;
    if (dsMon.getSize() > 0) {
        newId = dsMon.getTail()->data->getId() + 1;
    }
    dsMon.addTail(new MonAn(newId, tenMon, donGia));
    return 1;
}



bool AdminController::updateMonAn(DoublyLinkedList<MonAn*>& dsMon, int idSua, string tenMonMoi, double donGiaMoi) {
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

bool AdminController::deleteMonAn(DoublyLinkedList<MonAn*>& dsMon, int idXoa) {
    for (Node<MonAn*>* p = dsMon.getHead(); p != nullptr; p = p->next) {
        if (p->data->getId() == idXoa) {
            delete p->data;
            dsMon.remove(p);
            return true;
        }
    }
    return false;
}

int AdminController::validateAndSaveBooking(string phone, string idMayStr, string date, string hStr, string mStr, string durationStr, string pass, int currentTotalDays, int selectedBookingId, DoublyLinkedList<DatLich*>& dsDatLich, DoublyLinkedList<User*>& dsTaiKhoan) {
    // 1. Check rỗng
    if (phone.empty() || idMayStr.empty() || date.empty() || hStr.empty() || mStr.empty() || durationStr.empty() || pass.empty()) return -1;
    
    // 2. Check định dạng chuỗi
    if (!isValidPhoneNumber(phone)) return -2;
    if (date.length() != 10 || date[2] != '/' || date[5] != '/') return -3;
    
    try {
        // 3. Ép kiểu
        int d = stoi(date.substr(0, 2));
        int m = stoi(date.substr(3, 2));
        int y = stoi(date.substr(6, 4));
        int idMay = stoi(idMayStr);
        int gio = stoi(hStr);
        int phut = stoi(mStr);
        int thoiLuong = stoi(durationStr);
        
        // 4. Validate nghiệp vụ
        int bookTotalDays = y * 360 + m * 30 + d;
        if (idMay < 31 || idMay > 40) return -4; // Chỉ máy Stream
        if (d < 1 || d > 30 || m < 1 || m > 12) return -5;
        if (bookTotalDays < currentTotalDays || bookTotalDays > currentTotalDays + 14) return -6; // Quá 14 ngày
        if (gio < 0 || gio > 23 || phut < 0 || phut > 59 || thoiLuong <= 0) return -7;

        // 5. Xử lý logic tài khoản
        User* targetUser = nullptr;
        for(Node<User*>* uNode = dsTaiKhoan.getHead(); uNode != nullptr; uNode = uNode->next) {
            if (uNode->data->getUsername() == phone) { targetUser = uNode->data; break; }
        }

        if (targetUser != nullptr && targetUser->getPassword() != pass) return -8; // Sai pass
        
        // ----- ĐOẠN CODE GIỮ NGUYÊN TỪ ẢNH CỦA BẠN BẮT ĐẦU TỪ ĐÂY -----
        Client* cUser = dynamic_cast<Client*>(targetUser);
        if (cUser == nullptr) {
            // Tự động sinh ID cho Client mới
            int newClientId = 1;
            if (dsTaiKhoan.getSize() > 0) newClientId = dsTaiKhoan.getTail()->data->getId() + 1;
            
            // Cập nhật tham số theo cấu trúc Client mới: id, username, password, soDu, status
            cUser = new Client(newClientId, phone, pass, 0, 1);
            dsTaiKhoan.addTail(cUser);
        }

        if (selectedBookingId != -1) { 
            for (Node<DatLich*>* node = dsDatLich.getHead(); node != nullptr; node = node->next) {
                if (node->data->getId() == selectedBookingId) {
                    node->data->updateLich(idMay, date, gio, phut, thoiLuong); // Lưu ý: thay popInp3 bằng date
                    break;
                }
            }
        } else { 
            double tienNap = (thoiLuong / 60.0) * 20000.0;
            cUser->napTien(tienNap);

            int newId = 1;
            if (dsDatLich.getSize() > 0) newId = dsDatLich.getTail()->data->getId() + 1;
            dsDatLich.addTail(new DatLich(newId, phone, idMay, date, gio, phut, thoiLuong, 0)); // Lưu ý: thay popInp1 bằng phone, popInp3 bằng date
        }
        // ----- KẾT THÚC ĐOẠN CODE CỦA BẠN -----

        return 1; // Đặt lịch thành công
    } catch (...) {
        return 0; // Lỗi ép kiểu do nhập chữ
    }
}