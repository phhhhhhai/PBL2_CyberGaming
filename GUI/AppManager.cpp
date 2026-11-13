#include "AppManager.h"
#include <iostream>
#include <fstream>

int soLuongCartItem = 0;

AppManager::AppManager() {
    appState = 0;
    currentTab = 0;
    currentUser = nullptr;
    popupState = 0;
    assignScroll = 0;
    isHappyHourActive = false; 
    isVoucherApplied = false;
    secondCounter = 0.0f;
    
    FileManager::loadTaiKhoan("Data/TaiKhoan.txt", dsTaiKhoan);
    FileManager::loadGrid("Data/MayTram.txt", grid, dsTaiKhoan);
    FileManager::loadMenu("Data/Menu.txt", dsMon);
    FileManager::loadOrders("Data/Orders.txt", ordersQueue);
    FileManager::loadBookings("Data/Booking.txt", dsDatLich);

    std::ifstream fTime("Data/Time.txt");
    if (fTime.is_open()) {
        fTime >> vYear >> vMonth >> vDay >> vHour >> vMinute >> vSecond;
        fTime.close();
    }

    adminCtrl = new AdminController(dsTaiKhoan, dsDatLich, dsMon, grid);
    clientCtrl = new ClientController(dsTaiKhoan, grid);
}

AppManager::~AppManager() {
    delete adminCtrl;
    delete clientCtrl;
}

void AppManager::updateTimer() {
    virtualTimer += GetFrameTime();
    if (virtualTimer >= 1.0f) { 
        virtualTimer = 0.0f;
        vSecond++;
        if (vSecond >= 60) { 
            vSecond = 0;
            Vector<string> kickedUsers = adminCtrl->addVirtualMinutes(1, vDay, vMonth, vYear, vHour, vMinute, isHappyHourActive);
            if (currentUser != nullptr && kickedUsers.contains(currentUser->getUsername())) {
                appState = 0; 
                currentUser = nullptr; 
                loginMessage = "Tài khoản bị đăng xuất (Hết tiền/Hết giờ/Đóng cửa)!";
            }
        }
    }
}

void AppManager::processLogin(Vector2 mousePoint) {
    Rectangle userBox = {750, 470, 400, 50};
    Rectangle passBox = {750, 580, 400, 50};
    Rectangle btnLogin = {850, 680, 200, 60};

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mousePoint, userBox)) activeInput = 1;
        else if (CheckCollisionPointRec(mousePoint, passBox)) activeInput = 2;
        else activeInput = 0;

        float listX = 1220; float listY = 470;
        if (CheckCollisionPointRec(mousePoint, {listX + 390, listY, 40, 40})) loginScroll--;
        if (CheckCollisionPointRec(mousePoint, {listX + 390, listY + 185, 40, 40})) loginScroll++;
        if (loginScroll < 0) loginScroll = 0;
        
        int totalAcc = 0;
        for (Node<User*>* p = dsTaiKhoan.getHead(); p != nullptr; p = p->next) {
            if (p->data->getStatus() == 1) totalAcc++;
        }
        if (loginScroll > totalAcc - 5 && totalAcc > 5) loginScroll = totalAcc - 5;
        
        int count = 0; float itemY = listY;
        for (Node<User*>* p = dsTaiKhoan.getHead(); p != nullptr; p = p->next) {
            if (p->data->getStatus() == 0) continue;
            if (count >= loginScroll && count < loginScroll + 5) {
                if (CheckCollisionPointRec(mousePoint, {listX, itemY, 380, 40})) {
                    inputUser = p->data->getUsername(); 
                    activeInput = 2; 
                    loginMessage = ""; 
                }
                itemY += 45;
            }
            count++;
        }

        if (CheckCollisionPointRec(mousePoint, btnLogin)) {
            User* tempUser = nullptr;
            int res = clientCtrl->validateLogin(inputUser, inputPass, vHour, tempUser);

            if (res == 1 || res == 2) {
                if (res == 2) FileManager::saveGrid("Data/MayTram.txt", grid); 
                
                appState = 1; 
                currentUser = tempUser;
                currentTab = 0; 
                popupState = 0;
                loginMessage = ""; 
            } 
            else if (res == -2) loginMessage = "Sai mật khẩu!";
            else if (res == -3) loginMessage = "Hết giờ hoạt động (22:00 - 07:00)!";
            else if (res == -4) loginMessage = "Tài khoản hết tiền! Vui lòng nạp thêm.";
            else if (res == -5) loginMessage = "Tài khoản chưa được xếp máy!";
            else if (res == -1) loginMessage = "Tài khoản không tồn tại!";
        }
    }

    if (activeInput == 1 || activeInput == 2) {
        int key = GetCharPressed(); 
        while (key > 0) {
            if ((key >= 32) && (key <= 125)) {
                if (activeInput == 1 && inputUser.length() < 15) inputUser += (char)key; 
                else if (activeInput == 2 && inputPass.length() < 15) inputPass += (char)key; 
            }
            key = GetCharPressed(); 
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (activeInput == 1 && inputUser.length() > 0) inputUser.pop_back();
            else if (activeInput == 2 && inputPass.length() > 0) inputPass.pop_back();
        }
    }
}

void AppManager::processWorkspace(Vector2 mousePoint, bool isAdmin) {
    if (RaylibUI::showConfirmMoveModal) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Rectangle btnYes = {800, 530, 140, 45};
            Rectangle btnNo = {980, 530, 140, 45};

            if (CheckCollisionPointRec(mousePoint, btnYes)) {
                int res = adminCtrl->changeMachine(RaylibUI::sourceMachineId, RaylibUI::targetMachineId);
                if (res == 1) {
                    FileManager::saveGrid("Data/MayTram.txt", grid);
                } else if (res == -3) {
                    RaylibUI::moveNotificationMsg = "Tài khoản không đủ tiền cho loại máy này!";
                    RaylibUI::moveNotificationTimer = 2.0f;
                } else if (res == -5) {
                    RaylibUI::moveNotificationMsg = "Máy đang chạy Booking, không thể đổi!";
                    RaylibUI::moveNotificationTimer = 2.0f;
                }                        
                RaylibUI::showConfirmMoveModal = false;
                RaylibUI::sourceMachineId = -1;
                RaylibUI::targetMachineId = -1;
            }
            else if (CheckCollisionPointRec(mousePoint, btnNo)) {
                RaylibUI::showConfirmMoveModal = false;
                RaylibUI::sourceMachineId = -1;
                RaylibUI::targetMachineId = -1;
            }
        }
    }
    else if (popupState == 0) {
        Rectangle tabSoDo     = {10, 0, 150, 50};
        Rectangle tabTaiKhoan = {168, 0, 150, 50};
        Rectangle tabFnB      = {326, 0, 150, 50};
        Rectangle tabOrder    = {484, 0, 150, 50};
        Rectangle tabBooking  = {642, 0, 150, 50};
        Rectangle tabXepHang  = {800, 0, 155, 50};

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mousePoint, {1180, 10, 130, 30})) { popupState = 15; popMsg = ""; }
            if (isAdmin) {
                if (RaylibUI::isChangingMachine) {
                    int boxW = 100, boxH = 120, padX = 30, padY = 40, gap = 120, offX = 270, offY = 180;
                    for (int i = 0; i < ROWS; i++) {
                        for (int j = 0; j < COLS; j++) {
                            int ex = (j >= 5) ? gap : 0;
                            Rectangle rec = {(float)(offX + j*(boxW+padX) + ex), (float)(offY + i*(boxH+padY)), (float)boxW, (float)boxH};
                            
                            if (CheckCollisionPointRec(mousePoint, rec)) {
                                int clickedId = grid[i][j]->getId();
                                if (clickedId == RaylibUI::sourceMachineId) {
                                    RaylibUI::isChangingMachine = false;
                                } else if (grid[i][j]->getTrangThai() == 0) { 
                                    RaylibUI::targetMachineId = clickedId;
                                    RaylibUI::showConfirmMoveModal = true;
                                    RaylibUI::isChangingMachine = false;
                                } else { 
                                    RaylibUI::moveNotificationMsg = "Da co khach, khong chon duoc!";
                                    RaylibUI::moveNotificationTimer = 2.0f;
                                    RaylibUI::isChangingMachine = false;
                                }
                                break;
                            }
                        }
                    }
                }
                else {
                    if (CheckCollisionPointRec(mousePoint, {1530, 10, 70, 30})) isHappyHourActive = !isHappyHourActive;
                    if (CheckCollisionPointRec(mousePoint, tabSoDo)) currentTab = 0;
                    else if (CheckCollisionPointRec(mousePoint, tabTaiKhoan)) currentTab = 1;
                    else if (CheckCollisionPointRec(mousePoint, tabFnB)) currentTab = 2;
                    else if (CheckCollisionPointRec(mousePoint, tabOrder)) currentTab = 3;
                    else if (CheckCollisionPointRec(mousePoint, tabBooking)) currentTab = 4;
                    else if (CheckCollisionPointRec(mousePoint, tabXepHang)) currentTab = 5;

                    if (currentTab == 0) { 
                        int boxW = 100, boxH = 120, padX = 30, padY = 40, gap = 120, offX = 270, offY = 180;
                        for (int i = 0; i < ROWS; i++) {
                            for (int j = 0; j < COLS; j++) {
                                int ex = (j >= 5) ? gap : 0;
                                Rectangle rec = {(float)(offX + j*(boxW+padX) + ex), (float)(offY + i*(boxH+padY)), (float)boxW, (float)boxH};
                                if (CheckCollisionPointRec(mousePoint, rec)) {
                                    if (grid[i][j]->getTrangThai() == 0) { 
                                        selectedMachine = grid[i][j]->getId(); 
                                        popupState = 1; popInp1 = ""; popMsg = ""; activePopInp = 1; assignScroll = 0;
                                    } else if (grid[i][j]->getTrangThai() == 2) { 
                                        grid[i][j]->freeMachine(); 
                                        FileManager::saveGrid("Data/MayTram.txt", grid);
                                    } else if (grid[i][j]->getTrangThai() == 1 || grid[i][j]->getTrangThai() == 3) {
                                        selectedMachine = grid[i][j]->getId();
                                        popupState = 10; popMsg = "";
                                    }
                                }
                            }
                        }
                    }
                    else if (currentTab == 1) { 
                        if (CheckCollisionPointRec(mousePoint, {1560, 120, 150, 40})) {
                            popupState = 2; popInp1 = ""; popInp2 = ""; popInp3 = ""; popMsg = ""; activePopInp = 1;
                        }
                        float startY = 250;
                        for (Node<User*>* p = dsTaiKhoan.getHead(); p != nullptr; p = p->next) {
                            if (p->data->getStatus() == 0) continue;
                            if (CheckCollisionPointRec(mousePoint, {1440, startY - 5, 80, 30})) {
                                selectedAccountUsername = p->data->getUsername();
                                popupState = 11; selectedBillTime = ""; 
                            } 
                            else if (CheckCollisionPointRec(mousePoint, {1540, startY - 5, 80, 30})) {
                                selectedAccountUsername = p->data->getUsername();
                                popupState = 5; popInp1 = p->data->getPassword(); popInp2 = "0"; popMsg = ""; activePopInp = 1;
                            } 
                            else if (CheckCollisionPointRec(mousePoint, {1640, startY - 5, 80, 30})) {
                                selectedAccountUsername = p->data->getUsername();
                                popupState = 6; popMsg = "";
                            }
                            startY += 45;
                        }
                    }
                    else if (currentTab == 2) { 
                        if (CheckCollisionPointRec(mousePoint, {1560, 120, 150, 40})) {
                            popupState = 7; popInp1 = ""; popInp2 = ""; popMsg = ""; activePopInp = 1;
                        }
                        float startY = 250;
                        for (Node<MonAn*>* p = dsMon.getHead(); p != nullptr; p = p->next) {
                            if (CheckCollisionPointRec(mousePoint, {1540, startY - 5, 80, 30})) {
                                selectedFoodId = p->data->getId();
                                popupState = 8; popInp1 = p->data->getTenMon(); popInp2 = std::to_string((long long)p->data->getDonGia()); popMsg = ""; activePopInp = 1;
                            } else if (CheckCollisionPointRec(mousePoint, {1640, startY - 5, 80, 30})) {
                                selectedFoodId = p->data->getId();
                                popupState = 9; popMsg = "";
                            }
                            startY += 45;
                        }
                    }
                    else if (currentTab == 3) {
                        Vector<HoaDonDTO> bills = adminCtrl->getDanhSachHoaDon(ordersQueue);
                        float startY = 250; float tableX = 160;
                        for (int i = 0; i < bills.size(); i++) {
                            Rectangle btnStatus = {tableX + 1140, startY - 5, 110, 30};
                            Rectangle btnDetail = {tableX + 1380, startY - 5, 100, 30};
                            Rectangle btnDel = {tableX + 1500, startY - 5, 75, 30};

                            if (CheckCollisionPointRec(mousePoint, btnStatus)) {
                                adminCtrl->giaoHangBill(ordersQueue, bills[i].userId, bills[i].thoiGian);
                                FileManager::saveOrders("Data/Orders.txt", ordersQueue);
                                break;
                            }
                            else if (CheckCollisionPointRec(mousePoint, btnDetail)) {
                                string uName = "";
                                for (Node<User*>* u = dsTaiKhoan.getHead(); u != nullptr; u = u->next) {
                                    if (u->data->getId() == bills[i].userId) { uName = u->data->getUsername(); break; }
                                }
                                selectedBillTime = bills[i].thoiGian; 
                                selectedAccountUsername = uName;
                                popupState = 12;
                                break;
                            }
                            else if (CheckCollisionPointRec(mousePoint, btnDel)) {
                                int res = adminCtrl->deleteBill(ordersQueue, bills[i].userId, bills[i].thoiGian);
                                if (res == -1) popMsg = "Bill đã thanh toán, không thể xóa!";
                                else if (res == -2) popMsg = "Bill đã được giao, không thể xóa!";
                                else FileManager::saveOrders("Data/Orders.txt", ordersQueue);
                                break;
                            }
                            startY += 45;
                        }
                    }
                    else if (currentTab == 4) {
                        if (CheckCollisionPointRec(mousePoint, {1400, 100, 200, 45})) {
                            selectedBookingId = -1; 
                            popupState = 14; 
                            popInp1 = ""; popInp2 = ""; popInp3 = ""; 
                            popInp4 = ""; popInp5 = ""; popInp6 = ""; popInp7 = "";
                            popMsg = ""; activePopInp = 1;
                        }
                        float startY = 250;
                        for (Node<DatLich*>* node = dsDatLich.getHead(); node != nullptr; node = node->next) {
                            DatLich* b = node->data;
                            if (CheckCollisionPointRec(mousePoint, {160 + 1300, startY - 5, 75, 30})) {
                                selectedBookingId = b->getId();
                                popupState = 14; activePopInp = 1; popMsg = "";
                                string sdtKhach = "";
                                for (Node<User*>* u = dsTaiKhoan.getHead(); u != nullptr; u = u->next) {
                                    if (u->data->getId() == b->getUserId()) { sdtKhach = u->data->getUsername(); break; }
                                }
                                popInp1 = sdtKhach;
                                popInp2 = std::to_string(b->getIdMay()); popInp3 = b->getNgayDat();
                                popInp4 = std::to_string(b->getGioBatDau()); popInp5 = std::to_string(b->getPhutBatDau());
                                popInp6 = std::to_string(b->getThoiLuong()); popInp7 = ""; 
                            }
                            if (CheckCollisionPointRec(mousePoint, {160 + 1385, startY - 5, 75, 30})) {
                                adminCtrl->deleteBooking(b->getId());
                                FileManager::saveGrid("Data/MayTram.txt", grid);
                                FileManager::saveBookings("Data/Booking.txt", dsDatLich);
                                break; 
                            }
                            startY += 45;
                        }   
                    }
                }
            } else { 
                Rectangle tabTongQuan = {10, 0, 220, 50};
                Rectangle tabFnB = {240, 0, 220, 50};
                Rectangle tabCaiDat = {470, 0, 220, 50};
                Rectangle tabBXHClient = {700, 0, 240, 50};

                if (CheckCollisionPointRec(mousePoint, tabTongQuan)) currentTab = 0;
                else if (CheckCollisionPointRec(mousePoint, tabFnB)) currentTab = 1;
                else if (CheckCollisionPointRec(mousePoint, tabCaiDat)) currentTab = 2;
                else if (CheckCollisionPointRec(mousePoint, tabBXHClient)) currentTab = 3;

                if (currentTab == 1) {
                    Rectangle btnMenuClient = {100, 200, 400, 80}; 
                    Rectangle btnMonDaGoi = {550, 200, 400, 80}; 
                    if (CheckCollisionPointRec(mousePoint, btnMenuClient)) {
                        popupState = 20; popMsg = ""; soLuongCartItem = 0; isVoucherApplied = false;
                    } else if (CheckCollisionPointRec(mousePoint, btnMonDaGoi)) {
                        popupState = 21; selectedBillTime = ""; 
                    }
                }
                else if (currentTab == 2) { 
                    Rectangle btnDoiPass = {100, 200, 400, 80}; 
                    Rectangle btnTraMay = {100, 300, 400, 80}; 
                    if (CheckCollisionPointRec(mousePoint, btnDoiPass)) {
                        popupState = 3; popInp1 = ""; popInp2 = ""; popMsg = ""; activePopInp = 1;
                    } else if (CheckCollisionPointRec(mousePoint, btnTraMay)) {
                        popupState = 4; popMsg = "";
                    }
                }
            }
            if (CheckCollisionPointRec(mousePoint, {1750, 5, 150, 40})) {
                appState = 0; 
                currentUser = nullptr; 
                inputUser = ""; 
                inputPass = ""; 
                activeInput = 0; 
                loginMessage = "";
            }
        }
    }
}

void AppManager::processPopup(Vector2 mousePoint) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (popupState == 20) {
            float leftY = 300;
            for (Node<MonAn*>* p = dsMon.getHead(); p != nullptr; p = p->next) {
                Rectangle btnThem = {810, leftY, 90, 32};
                if (CheckCollisionPointRec(mousePoint, btnThem)) {
                    bool found = false;
                    for (Node<CartItem*>* c = cartList.getHead(); c != nullptr; c = c->next) {
                        if (c->data->getFoodId() == p->data->getId()) {
                            c->data->addSoLuong(1);
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        cartList.addTail(new CartItem(p->data->getId(), p->data->getTenMon(), p->data->getDonGia(), 1));
                    }
                    popMsg = "";
                }
                leftY += 50;
            }

            float rightY = 335;
            for (Node<CartItem*>* c = cartList.getHead(); c != nullptr; c = c->next) {
                Rectangle btnSub = {1170, rightY, 28, 28};
                Rectangle btnAdd = {1245, rightY, 28, 28};
                Rectangle btnXoa = {1505, rightY, 35, 28};

                if (CheckCollisionPointRec(mousePoint, btnSub)) {
                    if (c->data->getSoLuong() > 1) {
                        c->data->addSoLuong(-1);
                    } else {
                        delete c->data; cartList.remove(c);
                    }
                    break; 
                }
                if (CheckCollisionPointRec(mousePoint, btnAdd)) {
                    c->data->addSoLuong(1);
                    break;
                }
                if (CheckCollisionPointRec(mousePoint, btnXoa)) {
                    delete c->data; cartList.remove(c);
                    break;
                }
                rightY += 45;
            }

            bool canUseVoucher = adminCtrl->coTheDungVoucher(currentUser->getId(), ordersQueue, vDay, vMonth, vYear, vHour);
            if (!canUseVoucher) isVoucherApplied = false;

            if (canUseVoucher && !isVoucherApplied && cartList.getSize() > 0) {
                Rectangle btnApDungVoucher = {970, 745, 250, 40};
                if (CheckCollisionPointRec(mousePoint, btnApDungVoucher)) {
                    isVoucherApplied = true;
                }
            }

            Rectangle btnXacNhanDat = {1280, 830, 280, 50};
            if (CheckCollisionPointRec(mousePoint, btnXacNhanDat) && cartList.getSize() > 0) {
                string currentTime = TextFormat("%02d:%02d - %02d/%02d/%d", vHour, vMinute, vDay, vMonth, vYear);
                bool applyNow = (canUseVoucher && isVoucherApplied);
                int res = clientCtrl->datMon(currentUser->getUsername(), cartList, ordersQueue, currentTime, applyNow);
                if (res == 1) {
                    isVoucherApplied = false;
                    popMsg = "Dat mon thanh cong! Yeu cau da gui toi Admin.";
                    FileManager::saveOrders("Data/Orders.txt", ordersQueue);
                } else popMsg = "Loi: Khong the dat mon!";
            }

            Rectangle btnDongMenu = {350, 830, 180, 50};
            if (CheckCollisionPointRec(mousePoint, btnDongMenu)) {
                popupState = 0; popMsg = ""; isVoucherApplied = false;
            }
        }
        else if (popupState == 13) {
            if (CheckCollisionPointRec(mousePoint, {750, 460, 420, 40})) activePopInp = 1;
        }
        else if (popupState == 1) { 
            if (CheckCollisionPointRec(mousePoint, {1130, 400, 40, 40})) assignScroll--;
            if (CheckCollisionPointRec(mousePoint, {1130, 580, 40, 40})) assignScroll++;
            if (assignScroll < 0) assignScroll = 0;
            
            int countUnassigned = 0; float listY = 400;
            for (Node<User*>* p = dsTaiKhoan.getHead(); p != nullptr; p = p->next) {
                if (p->data->getRoleId() != 1) {
                    bool isAssigned = false;
                    for (int r=0; r<ROWS; r++) for(int c=0; c<COLS; c++) {
                        if (grid[r][c]->getCurrentClient() != nullptr && 
                            grid[r][c]->getCurrentClient()->getUsername() == p->data->getUsername()) {
                            isAssigned = true;
                        }
                    }
                    if (!isAssigned) {
                        if (countUnassigned >= assignScroll && countUnassigned < assignScroll + 5) {
                            if (CheckCollisionPointRec(mousePoint, {750, listY, 370, 40})) popInp1 = p->data->getUsername();
                            listY += 45;
                        }
                        countUnassigned++;
                    }
                }
            }
            if (assignScroll > countUnassigned - 5 && countUnassigned > 5) assignScroll = countUnassigned - 5;
        } 
        else if (popupState == 10) {
            if (CheckCollisionPointRec(mousePoint, {750, 480, 200, 45})) {
                if (adminCtrl->isMachineInBooking(selectedMachine)) {
                    popMsg = "Máy Booking, không thể đổi!";
                } else {
                    RaylibUI::sourceMachineId = selectedMachine;
                    RaylibUI::isChangingMachine = true;
                    popupState = 0;
                    selectedMachine = -1;
                }
            }
            else if (CheckCollisionPointRec(mousePoint, {980, 480, 150, 45})) {
                popupState = 0; selectedMachine = -1;
            }
        }
        else if (popupState == 2) { 
            if (CheckCollisionPointRec(mousePoint, {750, 380, 420, 40})) activePopInp = 1;
            else if (CheckCollisionPointRec(mousePoint, {750, 460, 420, 40})) activePopInp = 2;
            else if (CheckCollisionPointRec(mousePoint, {750, 540, 420, 40})) activePopInp = 3;
        } 
        else if (popupState == 3 || popupState == 5 || popupState == 7 || popupState == 8) { 
            if (CheckCollisionPointRec(mousePoint, {750, 450, 420, 40})) activePopInp = 1;
            else if (CheckCollisionPointRec(mousePoint, {750, 540, 420, 40})) activePopInp = 2;
        } 
        else if (popupState == 14) { 
            if (CheckCollisionPointRec(mousePoint, {500, 340, 420, 40})) activePopInp = 1;
            else if (CheckCollisionPointRec(mousePoint, {980, 340, 400, 40})) activePopInp = 2;
            else if (CheckCollisionPointRec(mousePoint, {980, 440, 400, 40})) activePopInp = 3;
            else if (CheckCollisionPointRec(mousePoint, {980, 540, 180, 40})) activePopInp = 4;
            else if (CheckCollisionPointRec(mousePoint, {1200, 540, 180, 40})) activePopInp = 5;
            else if (CheckCollisionPointRec(mousePoint, {980, 640, 400, 40})) activePopInp = 6;
            else if (CheckCollisionPointRec(mousePoint, {500, 440, 420, 40})) activePopInp = 7;
        }
        else if (popupState == 15) {
            int skipMins = 0;
            if (CheckCollisionPointRec(mousePoint, {770, 440, 100, 40})) skipMins = 15;
            if (CheckCollisionPointRec(mousePoint, {890, 440, 100, 40})) skipMins = 60;
            if (CheckCollisionPointRec(mousePoint, {1010, 440, 100, 40})) skipMins = 1440;
            if (skipMins > 0) {
                Vector<string> kickedUsers = adminCtrl->addVirtualMinutes(skipMins, vDay, vMonth, vYear, vHour, vMinute, isHappyHourActive);
                if (currentUser != nullptr && kickedUsers.contains(currentUser->getUsername())) {
                    appState = 0; currentUser = nullptr; loginMessage = "Tài khoản bị đăng xuất!";
                }
            }
            if (CheckCollisionPointRec(mousePoint, {880, 520, 150, 45})) popupState = 0; 
        }

        Rectangle btnHuyChung = {800, 650, 150, 50};
        Rectangle btnXacNhanChung = {970, 650, 150, 50};
        
        if (popupState == 11 || popupState == 21) {
            if (CheckCollisionPointRec(mousePoint, {900, 700, 150, 45})) { popupState = 0; }
            else {
                int tId = (popupState == 21) ? currentUser->getId() : -1;
                if (tId == -1) {
                    for (Node<User*>* u = dsTaiKhoan.getHead(); u != nullptr; u = u->next) {
                        if (u->data->getUsername() == selectedAccountUsername) { tId = u->data->getId(); break; }
                    }
                }
                Vector<string> bills;
                for (Node<Order*>* o = ordersQueue.getFront(); o != nullptr; o = o->next) {
                    if (o->data->getUserId() == tId) {
                        string tg = o->data->getThoiGian();
                        if (!bills.contains(tg)) bills.push_back(tg);
                    }
                }
                float bY = 330;
                for (int i = 0; i < bills.size(); i++) {
                    if (CheckCollisionPointRec(mousePoint, {1150, bY, 80, 30})) { 
                        selectedBillTime = bills[i];
                        popupState = (popupState == 11) ? 23 : 22; 
                        break;
                    }
                    bY += 45;
                }
            }
        }
        else if (popupState == 12) { 
            if (CheckCollisionPointRec(mousePoint, {600, 780, 150, 50})) { 
                popupState = 0; 
                selectedOrderId = -1; 
                popMsg = ""; 
            }
            else if (CheckCollisionPointRec(mousePoint, {800, 780, 200, 50})) {
                int tId = -1;
                for (Node<User*>* u = dsTaiKhoan.getHead(); u != nullptr; u = u->next) {
                    if (u->data->getUsername() == selectedAccountUsername) { tId = u->data->getId(); break; }
                }
                adminCtrl->giaoHangBill(ordersQueue, tId, selectedBillTime);
                FileManager::saveOrders("Data/Orders.txt", ordersQueue);
            }
            else if (CheckCollisionPointRec(mousePoint, {1050, 780, 200, 50})) { 
                int tId = -1;
                for (Node<User*>* u = dsTaiKhoan.getHead(); u != nullptr; u = u->next) {
                    if (u->data->getUsername() == selectedAccountUsername) { tId = u->data->getId(); break; }
                }
                string thoiGianLog = TextFormat("%04d%02d%02d_%02d%02d", vYear, vMonth, vDay, vHour, vMinute);
                
                adminCtrl->thanhToanBill(ordersQueue, dsMon, tId, selectedBillTime, thoiGianLog);
                FileManager::saveOrders("Data/Orders.txt", ordersQueue);
            }
            else {
                int tId = -1;
                for (Node<User*>* u = dsTaiKhoan.getHead(); u != nullptr; u = u->next) {
                    if (u->data->getUsername() == selectedAccountUsername) { tId = u->data->getId(); break; }
                }
                float cY = 340;
                for (Node<Order*>* o = ordersQueue.getFront(); o != nullptr; o = o->next) {
                    if (o->data->getUserId() == tId && o->data->getThoiGian() == selectedBillTime) {
                        Rectangle btnEdit = {1180, cY - 5, 60, 30};
                        Rectangle btnDel = {1250, cY - 5, 60, 30};
                        
                        if (CheckCollisionPointRec(mousePoint, btnEdit)) {
                            selectedOrderId = o->data->getId();
                            popupState = 13; 
                            popInp1 = std::to_string(o->data->getSoLuong()); 
                            popMsg = ""; activePopInp = 1;
                            break;
                        }
                        if (CheckCollisionPointRec(mousePoint, btnDel)) {
                            int res = adminCtrl->deleteOrder(ordersQueue, o->data->getId());
                            if (res == -1) popMsg = "Món đã thanh toán, không thể xóa!";
                            else if (res == -2) popMsg = "Món đã giao, không thể xóa!";
                            else FileManager::saveOrders("Data/Orders.txt", ordersQueue);
                            break;
                        }
                        cY += 45;
                    }
                }
            }
        }
        else if (popupState == 22 || popupState == 23) { 
            if (CheckCollisionPointRec(mousePoint, {870, 780, 150, 50})) {
                popupState = (popupState == 23) ? 11 : 21; 
            }
        }
        else if (popupState == 13) {
            Rectangle btnHuy13 = {800, 600, 150, 45};
            Rectangle btnXN13 = {970, 600, 150, 45};
            
            if (CheckCollisionPointRec(mousePoint, btnHuy13)) {
                popupState = 0; selectedOrderId = -1;
            }
            else if (CheckCollisionPointRec(mousePoint, btnXN13)) {
                if (popInp1.empty()) popMsg = "Vui lòng nhập số lượng!";
                else {
                    int newQty = 1; 
                    try { newQty = std::stoi(popInp1); } catch(...) { newQty = 1; }
                    if (newQty <= 0) newQty = 1;
                    int res = adminCtrl->updateOrderQuantity(ordersQueue, selectedOrderId, newQty);
                    if (res == -1) popMsg = "Đã thanh toán, không thể sửa!";
                    else if (res == -2) popMsg = "Đã giao hàng, không thể sửa!";
                    else {
                        FileManager::saveOrders("Data/Orders.txt", ordersQueue);
                        popupState = 12; 
                        selectedOrderId = -1;
                    }
                }
            }
        }
        else if (popupState == 14) { 
            Rectangle btnHuy14 = {800, 700, 150, 45};
            Rectangle btnXN14 = {970, 700, 150, 45};
            
            if (CheckCollisionPointRec(mousePoint, btnHuy14)) {
                popupState = 0; selectedBookingId = -1;
            }
            else if (CheckCollisionPointRec(mousePoint, btnXN14)) {
                int currentTotalDays = vYear * 365 + vDay;
                int mDays[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
                if ((vYear % 4 == 0 && vYear % 100 != 0) || vYear % 400 == 0) mDays[2] = 29;
                for(int i = 1; i < vMonth; i++) currentTotalDays += mDays[i];

                string thoiGianLog = TextFormat("%04d%02d%02d_%02d%02d", vYear, vMonth, vDay, vHour, vMinute);
                int res = adminCtrl->validateAndSaveBooking(popInp1, popInp2, popInp3, popInp4, popInp5, popInp6, popInp7, currentTotalDays, selectedBookingId, thoiGianLog);
                
                if (res == 1) {
                    FileManager::saveTaiKhoan("Data/TaiKhoan.txt", dsTaiKhoan); FileManager::saveBookings("Data/Booking.txt", dsDatLich);
                    popupState = 0; selectedBookingId = -1;
                }
                else if (res == -4) popMsg = "Chi duoc dat may Stream (31 - 40)!";
                else if (res == -5) popMsg = "Ngay/thang khong ton tai!";
                else if (res == -6) popMsg = "Chi duoc dat lich tu 1 -> 7 ngay sau ke tu hom nay!";
                else if (res == -8) popMsg = "Tai khoan da ton tai. Sai mat khau!";
                else if (res == -9) popMsg = "Chi duoc dat trong khung gio 07:00 - 22:00!";
                else if (res == -10) popMsg = "Dang choi: Chi duoc phep sua Thoi Luong!";
                else if (res == -11) popMsg = "Don da ket thuc, khong the sua!";
                else if (res == -12) popMsg = "Tien nap tu Booking vuot muc so du 10tr!";
                else if (res == -13) popMsg = "Thoi gian ket thuc vuot qua gio dong cua (22:00)!";
                else if (res == -14) popMsg = "Thời lượng chơi tối thiểu 60 phút!"; 
                else if (res == -15) popMsg = "Trùng lịch! Khung giờ này máy đã được đặt.";
                else popMsg = "Thong tin loi hoac khong hop le!";
            }
        }
        else if (CheckCollisionPointRec(mousePoint, btnHuyChung) && popupState != 10 && popupState != 20) {
            popupState = 0; selectedMachine = -1; selectedAccountUsername = ""; selectedFoodId = -1; selectedOrderId = -1;
        } 
        else if (popupState == 10 && CheckCollisionPointRec(mousePoint, {860, 650, 200, 50})) {
            popupState = 0; selectedMachine = -1;
        }
        else if (CheckCollisionPointRec(mousePoint, btnXacNhanChung)) {
            if (popupState == 1) { 
                if (popInp1.empty()) popMsg = "Vui long chon 1 khach hang!";
                else {
                    int res = adminCtrl->assignSeat(popInp1, selectedMachine);
                    if (res == 1) { 
                        for (int r = 0; r < ROWS; r++) {
                            for (int c = 0; c < COLS; c++) {
                                if (grid[r][c]->getId() == selectedMachine) {
                                    grid[r][c]->setTrangThai(3);
                                }
                            }
                        }
                        FileManager::saveGrid("Data/MayTram.txt", grid);
                        popupState = 0; selectedMachine = -1; 
                    }
                    else if (res == -4) popMsg = "Khong tim thay SĐT trong he thong!";
                    else if (res == -3) popMsg = "Khach hang khong du so du!";
                    else popMsg = "Loi may khong trong!";
                }
            } 
            else if (popupState == 2) { 
                double soDuGoc = 0; try { soDuGoc = std::stod(popInp3); } catch(...) { soDuGoc = 0; }
                string thoiGianLog = TextFormat("%04d%02d%02d_%02d%02d", vYear, vMonth, vDay, vHour, vMinute);
                int res = adminCtrl->addTaiKhoan(popInp1, popInp2, soDuGoc, thoiGianLog);
                if (res == 1) {
                    FileManager::saveTaiKhoan("Data/TaiKhoan.txt", dsTaiKhoan);
                    popupState = 0;
                }
                else if (res == 0) popMsg = "SĐT da ton tai!";
                else if (res == -3) popMsg = "So du ban dau phai >= 10000 VND!"; 
                else popMsg = "SĐT phai la so & bat dau bang 0!";
            }
            else if (popupState == 5) {
                double napThem = 0;
                if (!popInp2.empty()) try { napThem = std::stod(popInp2); } catch(...) {}
                bool napOk = true;
                
                if (napThem > 0) {
                    string thoiGianLog = TextFormat("%04d%02d%02d_%02d%02d", vYear, vMonth, vDay, vHour, vMinute);
                    napOk = adminCtrl->napTien(selectedAccountUsername, napThem, thoiGianLog);
                }

                if (!napOk) { popMsg = "Nap that bai (Toi da 5tr/lan, tong Du < 10tr)"; }
                else {
                    adminCtrl->updatePassword(selectedAccountUsername, popInp1);
                    FileManager::saveTaiKhoan("Data/TaiKhoan.txt", dsTaiKhoan);
                    popupState = 0; 
                    popMsg = ""; 
                    selectedAccountUsername = "";
                }
            }
            else if (popupState == 6) { 
                bool success = adminCtrl->deleteTaiKhoan(selectedAccountUsername, ordersQueue);
                if (success) {
                    FileManager::saveBookings("Data/Booking.txt", dsDatLich);
                    FileManager::saveGrid("Data/MayTram.txt", grid);
                    FileManager::saveTaiKhoan("Data/TaiKhoan.txt", dsTaiKhoan);
                    FileManager::saveOrders("Data/Orders.txt", ordersQueue); 
                    popupState = 0; selectedAccountUsername = "";
                } else {
                    popMsg = "Không thể xóa tài khoản quản trị (ADMIN)!";
                }
            }
            else if (popupState == 7) { 
                int res = adminCtrl->validateAndAddMonAn(popInp1, popInp2);
                if (res == 1) popupState = 0;
                else if (res == -1) popMsg = "Vui long nhap du thong tin!";
                else if (res == -2) popMsg = "Don gia phai co duoi 000!";
                else if (res == -3) popMsg = "Gia tien phai lon hon 0!";
                else if (res == 0) popMsg = "Don gia phai la so!";
            }
            else if (popupState == 8) { 
                if (popInp1.empty() || popInp2.empty()) popMsg = "Vui long nhap du thong tin!";
                else if (popInp2.length() < 4 || popInp2.substr(popInp2.length() - 3) != "000") popMsg = "Don gia phai co duoi 000!";
                else {
                    double gia = 0; try { gia = std::stod(popInp2); } catch(...) { gia = 0; }
                    bool ok = adminCtrl->updateMonAn(selectedFoodId, popInp1, gia);
                    if (ok) { popupState = 0; selectedFoodId = -1; } else popMsg = "Gia tien khong hop le!";
                }
            }
            else if (popupState == 9) { 
                adminCtrl->deleteMonAn(selectedFoodId);
                popupState = 0; selectedFoodId = -1;
            }
            else if (popupState == 3) { 
                if (popInp1 != currentUser->getPassword()) popMsg = "Mat khau cu khong dung!";
                else {
                    bool ok = clientCtrl->changePassword(currentUser->getUsername(), popInp2);
                    if (ok) popupState = 0; else popMsg = "Mat khau moi phai khac mat khau cu!";
                }
            }
            else if (popupState == 4) { 
                int loaiMay = 0;
                int idMay = clientCtrl->getAssignedMachine(currentUser->getUsername(), loaiMay);
                if (idMay != -1){
                    clientCtrl->returnMachine(currentUser->getUsername());
                    FileManager::saveGrid("Data/MayTram.txt", grid);
                    popupState = 0;
                    appState = 0; 
                    currentUser = nullptr; 
                    inputUser = ""; 
                    inputPass = ""; 
                    activeInput = 0; 
                    loginMessage = "Da tra may thanh cong!";
                }
            }
        }
    }
}

void AppManager::processKeyboard() {
    if (popupState > 0 && popupState != 20 && popupState != 10 && popupState != 15 && popupState != 11 && popupState != 12 && popupState != 21 && popupState != 22) {
        int key = GetCharPressed(); 
        while (key > 0) {
            if ((key >= 32 && key <= 125)) {
                bool allow = true;

                if (popupState == 13) {
                    if (key < '0' || key > '9') allow = false;
                }
                else if (popupState == 7 || popupState == 8) {
                    if (activePopInp == 1 && key == 32) key = '_'; 
                    if (activePopInp == 2 && (key < '0' || key > '9')) allow = false;   
                }
                else if (popupState == 2 || popupState == 11) { 
                    if (activePopInp == 1 && (key < '0' || key > '9')) allow = false; 
                    if (activePopInp == 3 && (key < '0' || key > '9')) allow = false; 
                }
                else if (popupState == 5) {
                    if (activePopInp == 2 && (key < '0' || key > '9')) allow = false; 
                }

                if (allow) {
                    if (popupState != 1 && activePopInp == 1 && popInp1.length() < 20) popInp1 += (char)key; 
                    else if (activePopInp == 2 && popInp2.length() < 20) popInp2 += (char)key; 
                    else if (activePopInp == 3 && popInp3.length() < 15) popInp3 += (char)key; 
                    else if (activePopInp == 4 && popInp4.length() < 10) popInp4 += (char)key; 
                    else if (activePopInp == 5 && popInp5.length() < 10) popInp5 += (char)key; 
                    else if (activePopInp == 6 && popInp6.length() < 10) popInp6 += (char)key; 
                    else if (activePopInp == 7 && popInp7.length() < 20) popInp7 += (char)key; 
                }
            }
            key = GetCharPressed(); 
        }
        
        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (popupState != 1 && activePopInp == 1 && popInp1.length() > 0) popInp1.pop_back();
            else if (activePopInp == 2 && popInp2.length() > 0) popInp2.pop_back();
            else if (activePopInp == 3 && popInp3.length() > 0) popInp3.pop_back();
            else if (activePopInp == 4 && popInp4.length() > 0) popInp4.pop_back();
            else if (activePopInp == 5 && popInp5.length() > 0) popInp5.pop_back();
            else if (activePopInp == 6 && popInp6.length() > 0) popInp6.pop_back();
            else if (activePopInp == 7 && popInp7.length() > 0) popInp7.pop_back();
        }
    }
}

void AppManager::drawScreen(Font appFont, RenderTexture2D target) {
    BeginTextureMode(target);
    ClearBackground(RAYWHITE); 
    if (appState == 0) {
        RaylibUI::drawLoginScreen(appFont, inputUser, inputPass, activeInput, loginMessage, dsTaiKhoan, loginScroll);
    } else if (appState == 1) {
        string clockStr = TextFormat("%02d:%02d - %02d/%02d/%d", vHour, vMinute, vDay, vMonth, vYear);
        RaylibUI::drawWorkspace(appFont, clockStr, currentTab, grid, dsTaiKhoan, dsMon, ordersQueue, cartList, dsDatLich,
            currentUser, popupState, activePopInp, 
            (popupState == 12 || popupState == 22 || popupState == 23) ? selectedBillTime : popInp1, 
            (popupState == 11 || popupState == 12 || popupState == 23) ? selectedAccountUsername : popInp2, 
            popInp3, popInp4, popInp5, popInp6, popInp7, popMsg, assignScroll, selectedMachine, isHappyHourActive, adminCtrl,
            vDay, vMonth, vYear, vHour, isVoucherApplied);
    }
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK); 
    DrawTexturePro(target.texture, 
        { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height }, 
        { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() }, 
        { 0.0f, 0.0f }, 0.0f, WHITE);
    EndDrawing();
}

void AppManager::run() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "He Thong Quan Ly Cyber Gaming"); 
    
    int codepoints[8000];
    for (int i = 0; i < 8000; i++) codepoints[i] = i + 32; 
    
    Font appFont = LoadFontEx("GUI/Roboto-SemiBold.ttf", 96, codepoints, 8000);
    SetTextureFilter(appFont.texture, TEXTURE_FILTER_BILINEAR);
    
    RenderTexture2D target = LoadRenderTexture(1920, 1080);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float scaleX = (float)GetScreenWidth() / 1920.0f;
        float scaleY = (float)GetScreenHeight() / 1080.0f;
        
        Vector2 mousePoint = GetMousePosition();
        mousePoint.x /= scaleX; 
        mousePoint.y /= scaleY; 

        updateTimer();

        if (appState == 0) { 
            processLogin(mousePoint);
        } 
        else if (appState == 1) { 
            bool isAdmin = (currentUser != nullptr && currentUser->getRoleId() == 1);
            processWorkspace(mousePoint, isAdmin);
            processPopup(mousePoint);
        }

        processKeyboard();
        drawScreen(appFont, target);
    }

    std::ofstream outTime("Data/Time.txt");
    outTime << vYear << " " << vMonth << " " << vDay << " " << vHour << " " << vMinute << " " << vSecond;
    outTime.close();
    UnloadRenderTexture(target);
    UnloadFont(appFont);
    CloseWindow(); 
}