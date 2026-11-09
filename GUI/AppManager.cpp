#include "AppManager.h"
#include <iostream>

int soLuongCartItem = 0;

AppManager::AppManager() {
    appState = 0;
    currentTab = 0;
    currentUser = nullptr;
    popupState = 0;
    assignScroll = 0;
    isHappyHourActive = false; 
    secondCounter = 0.0f;
    
    FileManager::loadTaiKhoan("Data/TaiKhoan.txt", dsTaiKhoan);
    FileManager::loadGrid("Data/MayTram.txt", grid, dsTaiKhoan);
    FileManager::loadMenu("Data/Menu.txt", dsMon);
    FileManager::loadOrders("Data/Orders.txt", ordersQueue);
    FileManager::loadBookings("Data/Booking.txt", dsDatLich);
}

void AppManager::run() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MAXIMIZED);
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

        // ================= TRỪ TIỀN & ĐỒNG HỒ ẢO =================
        virtualTimer += GetFrameTime();
        if (virtualTimer >= 1.0f) { 
            virtualTimer = 0.0f;
            vSecond++;
            if (vSecond >= 60) { 
                vSecond = 0;
                addVirtualMinutes(1);
            }

            // Logic trừ tiền thời gian thực
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (grid[r][c]->getTrangThai() == 1 && grid[r][c]->getCurrentClient() != nullptr) {
                        string u = grid[r][c]->getCurrentClient()->getUsername();
                        double rRate = ClientController::getTickRate(grid[r][c]->getLoaiMay(), isHappyHourActive);
                        
                        bool ok = ClientController::processTick(dsTaiKhoan, u, rRate);
                        if (!ok) {
                            grid[r][c]->freeMachine();
                            grid[r][c]->setTrangThai(2); // Dọn dẹp
                            FileManager::saveGrid("Data/MayTram.txt", grid);
                            if (currentUser != nullptr && currentUser->getUsername() == u) {
                                appState = 0; currentUser = nullptr; loginMessage = "Tai khoan het tien!";
                            }
                        }
                    }
                }
            }

            // 2. Logic Đệm 15 Phút - Khóa máy Booking
            string todayStr = TextFormat("%02d/%02d/%d", vDay, vMonth, vYear);
            int currMinsTotal = vHour * 60 + vMinute;

            for (Node<DatLich*>* node = dsDatLich.getHead(); node != nullptr; node = node->next) {
                DatLich* b = node->data;
                if (b->getNgayDat() == todayStr) {
                    int bookMins = b->getGioBatDau() * 60 + b->getPhutBatDau();
                    if (currMinsTotal < bookMins) {
                        b->setTrangThai(0); // Chưa đến giờ
                        if (currMinsTotal >= bookMins - 15) {
                            int mId = b->getIdMay();
                            for (int r = 0; r < ROWS; r++) for (int c = 0; c < COLS; c++) {
                                if (grid[r][c]->getId() == mId) {
                                    if (grid[r][c]->getTrangThai() == 1 && (grid[r][c]->getCurrentClient() == nullptr || grid[r][c]->getCurrentClient()->getUsername() != b->getSdtKhach())) {
                                        grid[r][c]->setTrangThai(4); 
                                        grid[r][c]->freeMachine(); // Đá khách vãng lai
                                        FileManager::saveGrid("Data/MayTram.txt", grid);
                                    } else if (grid[r][c]->getTrangThai() == 0 || grid[r][c]->getTrangThai() == 2) {
                                        grid[r][c]->setTrangThai(4); 
                                        FileManager::saveGrid("Data/MayTram.txt", grid);
                                    }
                                }
                            }
                        }
                    }
                    else if (currMinsTotal >= bookMins && currMinsTotal < bookMins + b->getThoiLuong()) {
                        b->setTrangThai(1); // Đang chơi
                        int mId = b->getIdMay();
                        for (int r = 0; r < ROWS; r++) for (int c = 0; c < COLS; c++) {
                            if (grid[r][c]->getId() == mId && (grid[r][c]->getTrangThai() == 4 || grid[r][c]->getTrangThai() == 0)) {
                                Client* cUser = nullptr;
                                for(Node<User*>* uNode = dsTaiKhoan.getHead(); uNode != nullptr; uNode = uNode->next) {
                                    if (uNode->data->getUsername() == b->getSdtKhach()) {
                                        cUser = dynamic_cast<Client*>(uNode->data);
                                        break;
                                    }
                                }
                                if (cUser != nullptr) grid[r][c]->assignClient(cUser);
                                FileManager::saveGrid("Data/MayTram.txt", grid);
                            }
                        }
                    }
                    else {
                         b->setTrangThai(2); // Đã xong
                         FileManager::saveBookings("Data/Booking.txt", dsDatLich);
                    }
                }
            }
        }

        if (appState == 0) { 
            Rectangle userBox = {750, 470, 400, 50};
            Rectangle passBox = {750, 580, 400, 50};
            Rectangle btnLogin = {850, 680, 200, 60};

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mousePoint, userBox)) activeInput = 1;
                else if (CheckCollisionPointRec(mousePoint, passBox)) activeInput = 2;
                else activeInput = 0;

                if (CheckCollisionPointRec(mousePoint, btnLogin)) {
                    bool found = false;
                    for (Node<User*>* p = dsTaiKhoan.getHead(); p != nullptr; p = p->next) {
                        if (p->data->getUsername() == inputUser && p->data->getPassword() == inputPass) {
                            
                            bool isAdmin = (p->data->getRoleId() == 1); 
                            if (!isAdmin) {
                                bool isAssigned = false;
                                for (int r = 0; r < ROWS; r++) {
                                    for (int c = 0; c < COLS; c++) {
                                        if (grid[r][c]->getCurrentClient() != nullptr && 
                                            grid[r][c]->getCurrentClient()->getUsername() == inputUser) {
                                            isAssigned = true; 
                                            if (grid[r][c]->getTrangThai() == 3) {
                                                grid[r][c]->setTrangThai(1);
                                                FileManager::saveGrid("Data/MayTram.txt", grid);
                                            }
                                            break;
                                        }
                                    }
                                }
                                if (!isAssigned) {
                                    loginMessage = "Tai khoan chua duoc xep may!";
                                    found = true; 
                                    break;
                                }
                            }
                            appState = 1; 
                            currentUser = p->data;
                            currentTab = 0; 
                            popupState = 0;
                            found = true;
                            loginMessage = ""; 
                            break;
                        }
                    }
                    if (!found && loginMessage.empty()) loginMessage = "Sai tai khoan hoac mat khau!";
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
        else if (appState == 1) { 
            bool isAdmin = (currentUser != nullptr && currentUser->getRoleId() == 1);

            if (RaylibUI::showConfirmMoveModal) {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    Rectangle btnYes = {800, 530, 140, 45};
                    Rectangle btnNo = {980, 530, 140, 45};

                    if (CheckCollisionPointRec(mousePoint, btnYes)) {
                        int srcR = -1, srcC = -1, destR = -1, destC = -1;
                        for (int r = 0; r < ROWS; r++) {
                            for (int c = 0; c < COLS; c++) {
                                if (grid[r][c]->getId() == RaylibUI::sourceMachineId) { srcR = r; srcC = c; }
                                if (grid[r][c]->getId() == RaylibUI::targetMachineId) { destR = r; destC = c; }
                            }
                        }

                        if (srcR != -1 && destR != -1 && grid[srcR][srcC]->getCurrentClient() != nullptr) {
                            int loaiMoi = grid[destR][destC]->getLoaiMay();
                            double giaMoi = (loaiMoi == 0) ? 10000 : ((loaiMoi == 1) ? 14000 : 20000);
                            
                            Client* cUser = grid[srcR][srcC]->getCurrentClient();
                            if (cUser->getSoDu() < giaMoi) {
                                RaylibUI::moveNotificationMsg = "Tai khoan khong du tien cho loai may nay!";
                                RaylibUI::moveNotificationTimer = 2.0f;
                            } else {
                                grid[destR][destC]->assignClient(cUser);
                                grid[srcR][srcC]->freeMachine();

                        
                                FileManager::saveGrid("Data/MayTram.txt", grid);
                            }
                        }
                        RaylibUI::showConfirmMoveModal = false;
                        RaylibUI::sourceMachineId = -1;
                        RaylibUI::targetMachineId = -1;
                    }
                }
            }
            else if (popupState == 0) {
                Rectangle tabSoDo     = {10, 0, 180, 50};
                Rectangle tabTaiKhoan = {200, 0, 180, 50};
                Rectangle tabFnB      = {390, 0, 180, 50};
                Rectangle tabOrder    = {580, 0, 180, 50};
                Rectangle tabBooking  = {770, 0, 180, 50};

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
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
                            if (CheckCollisionPointRec(mousePoint, {1180, 10, 130, 30})) { popupState = 15; popMsg = ""; }
                            
                            if (CheckCollisionPointRec(mousePoint, tabSoDo)) currentTab = 0;
                            else if (CheckCollisionPointRec(mousePoint, tabTaiKhoan)) currentTab = 1;
                            else if (CheckCollisionPointRec(mousePoint, tabFnB)) currentTab = 2;
                            else if (CheckCollisionPointRec(mousePoint, tabOrder)) currentTab = 3;
                            else if (CheckCollisionPointRec(mousePoint, tabBooking)) currentTab = 4;
                            
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
                                float startY = 250;
                                float tableX = 160;
                                for (Node<Order*>* p = ordersQueue.getFront(); p != nullptr; p = p->next) {
                                    Rectangle btnPay = {tableX + 1070, startY - 5, 100, 30};
                                    Rectangle btnStatus = {tableX + 1240, startY - 5, 110, 30};
                                    Rectangle btnEdit = {tableX + 1410, startY - 5, 75, 30};
                                    Rectangle btnDel = {tableX + 1495, startY - 5, 75, 30};

                                    if (CheckCollisionPointRec(mousePoint, btnPay)) {
                                        p->data->setPaid(1 - p->data->getPaid());
                                        FileManager::saveOrders("Data/Orders.txt", ordersQueue);
                                    }
                                    else if (CheckCollisionPointRec(mousePoint, btnStatus)) {
                                        p->data->setStatus(1 - p->data->getStatus());
                                        FileManager::saveOrders("Data/Orders.txt", ordersQueue);
                                    }
                                    else if (CheckCollisionPointRec(mousePoint, btnEdit)) {
                                        selectedOrderId = p->data->getId();
                                        popupState = 13; popInp1 = std::to_string(p->data->getSoLuong()); popMsg = ""; activePopInp = 1;
                                    }
                                    else if (CheckCollisionPointRec(mousePoint, btnDel)) {
                                        int targetOrderId = p->data->getId();
                                        Queue<Order*> tempQueue;
                                        
                                        while (!ordersQueue.isEmpty()) {
                                            Order* ord = ordersQueue.dequeue();
                                            if (ord->getId() != targetOrderId) {
                                                tempQueue.enqueue(ord);
                                            } else {
                                                delete ord; 
                                            }
                                        }
                                        
                                        while (!tempQueue.isEmpty()) {
                                            ordersQueue.enqueue(tempQueue.dequeue());
                                        }
                                        
                                        FileManager::saveOrders("Data/Orders.txt", ordersQueue);
                                        break;
                                    }
                                    startY += 45;
                                }
                            }
                            else if (currentTab == 4) {
                                if (CheckCollisionPointRec(mousePoint, {1400, 100, 200, 45})) {
                                    selectedBookingId = -1; // -1 là Tạo mới
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
                                        popInp1 = b->getSdtKhach(); popInp2 = std::to_string(b->getIdMay()); popInp3 = b->getNgayDat();
                                        popInp4 = std::to_string(b->getGioBatDau()); popInp5 = std::to_string(b->getPhutBatDau());
                                        popInp6 = std::to_string(b->getThoiLuong()); popInp7 = ""; 
                                    }
                                    if (CheckCollisionPointRec(mousePoint, {160 + 1385, startY - 5, 75, 30})) {
                                        delete b; dsDatLich.remove(node); // Thu hồi RAM
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

                        if (CheckCollisionPointRec(mousePoint, tabTongQuan)) currentTab = 0;
                        else if (CheckCollisionPointRec(mousePoint, tabFnB)) currentTab = 1;
                        else if (CheckCollisionPointRec(mousePoint, tabCaiDat)) currentTab = 2;

                        if (currentTab == 1) {
                            Rectangle btnMenuClient = {100, 200, 400, 80}; 
                            Rectangle btnMonDaGoi = {550, 200, 400, 80}; 
                            if (CheckCollisionPointRec(mousePoint, btnMenuClient)) {
                                popupState = 20; popMsg = ""; soLuongCartItem = 0; 
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
            else {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (popupState == 20) {
                        float leftY = 300;
                        // 1. Logic nhấn nút [+ THÊM] món vào giỏ
                        for (Node<MonAn*>* p = dsMon.getHead(); p != nullptr; p = p->next) {
                            Rectangle btnThem = {810, leftY, 90, 32};
                            if (CheckCollisionPointRec(mousePoint, btnThem)) {
                                bool found = false;
                                // Dò xem món này đã có trong giỏ chưa
                                for (Node<CartItem*>* c = cartList.getHead(); c != nullptr; c = c->next) {
                                    if (c->data->getFoodId() == p->data->getId()) {
                                        c->data->addSoLuong(1); // Tăng số lượng
                                        found = true;
                                        break;
                                    }
                                }
                                if (!found) {
                                    // Nếu chưa có, cấp phát Node mới đẩy vào giỏ
                                    cartList.addTail(new CartItem(p->data->getId(), p->data->getTenMon(), p->data->getDonGia(), 1));
                                }
                                popMsg = "";
                            }
                            leftY += 50;
                        }

                        float rightY = 335;
                        // 2. Logic tương tác trong Giỏ Hàng (Cộng, Trừ, Xóa)
                        for (Node<CartItem*>* c = cartList.getHead(); c != nullptr; c = c->next) {
                            Rectangle btnSub = {1170, rightY, 28, 28};
                            Rectangle btnAdd = {1245, rightY, 28, 28};
                            Rectangle btnXoa = {1505, rightY, 35, 28};

                            if (CheckCollisionPointRec(mousePoint, btnSub)) {
                                if (c->data->getSoLuong() > 1) {
                                    c->data->addSoLuong(-1);
                                } else {
                                    delete c->data; cartList.remove(c); // Thu hồi vùng nhớ
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

                        // 3. Xác nhận Đặt Hàng (Đẩy từ Giỏ hàng sang Hàng đợi Queue)
                        Rectangle btnXacNhanDat = {1280, 830, 280, 50};
                        if (CheckCollisionPointRec(mousePoint, btnXacNhanDat) && cartList.getSize() > 0) {
                            int mId = -1;
                            for (int r = 0; r < ROWS; r++) for (int c_idx = 0; c_idx < COLS; c_idx++) {
                                if (grid[r][c_idx]->getCurrentClient() != nullptr &&
                                    grid[r][c_idx]->getCurrentClient()->getUsername() == currentUser->getUsername()) {
                                    mId = grid[r][c_idx]->getId();
                                    break;
                                }
                            }
                            
                            // Sinh ID tự động cho Order
                            int nextOrderId = 1;
                            for (Node<Order*>* o = ordersQueue.getFront(); o != nullptr; o = o->next) {
                                if (o->data->getId() >= nextOrderId) nextOrderId = o->data->getId() + 1;
                            }

                            // Tạo Order, chuẩn hóa Khóa ngoại (Food ID)
                            string currentTime = TextFormat("%02d:%02d - %02d/%02d/%d", vHour, vMinute, vDay, vMonth, vYear);
                            for (Node<CartItem*>* c = cartList.getHead(); c != nullptr; c = c->next) {
                                ordersQueue.enqueue(new Order(nextOrderId++, mId, currentUser->getId(), c->data->getFoodId(), c->data->getSoLuong(), 0, 0, currentTime));
                            }
                            cartList.clear(); 
                            popMsg = "Dat mon thanh cong! Yeu cau da gui toi Admin.";
                            FileManager::saveOrders("Data/Orders.txt", ordersQueue);
                        }

                        Rectangle btnDongMenu = {350, 830, 180, 50};
                        if (CheckCollisionPointRec(mousePoint, btnDongMenu)) {
                            popupState = 0; popMsg = "";
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
                            RaylibUI::sourceMachineId = selectedMachine;
                            RaylibUI::isChangingMachine = true;
                            popupState = 0;
                            selectedMachine = -1;
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
                        if (CheckCollisionPointRec(mousePoint, {770, 440, 100, 40})) addVirtualMinutes(15);
                        if (CheckCollisionPointRec(mousePoint, {890, 440, 100, 40})) addVirtualMinutes(60);
                        if (CheckCollisionPointRec(mousePoint, {1010, 440, 100, 40})) addVirtualMinutes(1440);
                        if (CheckCollisionPointRec(mousePoint, {880, 520, 150, 45})) popupState = 0; 
                    }

                }

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

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    Rectangle btnHuyChung = {800, 650, 150, 50};
                    Rectangle btnXacNhanChung = {970, 650, 150, 50};
                    
                    if (popupState == 11 || popupState == 21) {
                        if (CheckCollisionPointRec(mousePoint, {900, 700, 150, 45})) { popupState = 0; } // HỦY
                        else {
                            int tId = (popupState == 21) ? currentUser->getId() : -1;
                            if (tId == -1) {
                                for (Node<User*>* u = dsTaiKhoan.getHead(); u != nullptr; u = u->next) {
                                    if (u->data->getUsername() == selectedAccountUsername) { tId = u->data->getId(); break; }
                                }
                            }
                            std::vector<string> bills;
                            for (Node<Order*>* o = ordersQueue.getFront(); o != nullptr; o = o->next) {
                                if (o->data->getUserId() == tId) {
                                    string tg = o->data->getThoiGian();
                                    if (std::find(bills.begin(), bills.end(), tg) == bills.end()) bills.push_back(tg);
                                }
                            }
                            float bY = 330;
                            for (size_t i = 0; i < bills.size(); i++) {
                                if (CheckCollisionPointRec(mousePoint, {1150, bY, 80, 30})) { 
                                    selectedBillTime = bills[i];
                                    popupState = (popupState == 11) ? 12 : 22; 
                                    break;
                                }
                                bY += 45;
                            }
                        }
                    }
                    else if (popupState == 12) { 
                        if (CheckCollisionPointRec(mousePoint, {700, 780, 150, 50})) popupState = 11; 
                        else if (CheckCollisionPointRec(mousePoint, {950, 780, 200, 50})) { 
                            int tId = -1;
                            for (Node<User*>* u = dsTaiKhoan.getHead(); u != nullptr; u = u->next) {
                                if (u->data->getUsername() == selectedAccountUsername) { tId = u->data->getId(); break; }
                            }
                            for (Node<Order*>* o = ordersQueue.getFront(); o != nullptr; o = o->next) {
                                if (o->data->getUserId() == tId && o->data->getThoiGian() == selectedBillTime) {
                                    o->data->setPaid(1); 
                                }
                            }
                            FileManager::saveOrders("Data/Orders.txt", ordersQueue);
                            popupState = 11; 
                        }
                    }
                    else if (popupState == 22) { 
                        if (CheckCollisionPointRec(mousePoint, {870, 780, 150, 50})) popupState = 21; 
                    }
                    else if (popupState == 13) {
                        Rectangle btnHuy13 = {800, 600, 150, 45};
                        Rectangle btnXN13 = {970, 600, 150, 45};
                        
                        if (CheckCollisionPointRec(mousePoint, btnHuy13)) {
                            popupState = 0; selectedOrderId = -1;
                        }
                        else if (CheckCollisionPointRec(mousePoint, btnXN13)) {
                            if (popInp1.empty()) popMsg = "Vui long nhap so luong!";
                            else {
                                int newQty = 1; 
                                try { newQty = std::stoi(popInp1); } catch(...) { newQty = 1; }
                                if (newQty <= 0) newQty = 1;

                                for (Node<Order*>* p = ordersQueue.getFront(); p != nullptr; p = p->next) {
                                    if (p->data->getId() == selectedOrderId) {
                                        p->data->setSoLuong(newQty);
                                        FileManager::saveOrders("Data/Orders.txt", ordersQueue);
                                        break;
                                    }
                                }
                                popupState = 0; selectedOrderId = -1;
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
                            int currentTotalDays = vYear * 360 + vMonth * 30 + vDay;
                            int res = AdminController::validateAndSaveBooking(popInp1, popInp2, popInp3, popInp4, popInp5, popInp6, popInp7, 
                                currentTotalDays, selectedBookingId, dsDatLich, dsTaiKhoan);
                            if (res == 1) {
                                FileManager::saveTaiKhoan("Data/TaiKhoan.txt", dsTaiKhoan);
                                FileManager::saveBookings("Data/Booking.txt", dsDatLich);
                                popupState = 0; 
                                selectedBookingId = -1;
                            }
                            else if (res == -1) popMsg = "Vui long nhap day du thong tin!";
                            else if (res == -2) popMsg = "SDT khong hop le!";
                            else if (res == -3) popMsg = "Dinh dang ngay phai la DD/MM/YYYY!";
                            else if (res == -4) popMsg = "Chi duoc dat lich may Stream (31 - 40)!";
                            else if (res == -5) popMsg = "Ngay/thang khong hop le!";
                            else if (res == -6) popMsg = "Chi duoc dat trong vong 14 NGAY toi!";
                            else if (res == -7) popMsg = "Thoi gian khong hop le!";
                            else if (res == -8) popMsg = "Tai khoan da ton tai. Sai mat khau!";
                            else if (res == 0)  popMsg = "ID, Thoi gian, Thoi luong phai la so!";
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
                                int res = AdminController::assignSeat(popInp1, selectedMachine, dsTaiKhoan, grid);
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
                            int res = AdminController::addTaiKhoan(dsTaiKhoan, popInp1, popInp2, soDuGoc);
                            if (res == 1) popupState = 0;
                            else if (res == 0) popMsg = "SĐT da ton tai!";
                            else if (res == -3) popMsg = "So du ban dau phai >= 10000 VND!"; 
                            else popMsg = "SĐT phai la so & bat dau bang 0!";
                        }
                        else if (popupState == 5) { 
                            AdminController::updatePassword(dsTaiKhoan, selectedAccountUsername, popInp1);
                            double napThem = 0; try { napThem = std::stod(popInp2); } catch(...) { napThem = 0; }
                            if (napThem > 0) AdminController::napTien(dsTaiKhoan, selectedAccountUsername, napThem);
                            FileManager::saveTaiKhoan("Data/TaiKhoan.txt", dsTaiKhoan);
                            popupState = 0; selectedAccountUsername = "";
                        }
                        else if (popupState == 6) { 
                            AdminController::deleteTaiKhoan(dsTaiKhoan, selectedAccountUsername, grid);
                            popupState = 0; selectedAccountUsername = "";
                        }
                        else if (popupState == 7) { 
                            int res = AdminController::validateAndAddMonAn(dsMon, popInp1, popInp2);
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
                                bool ok = AdminController::updateMonAn(dsMon, selectedFoodId, popInp1, gia);
                                if (ok) { popupState = 0; selectedFoodId = -1; } else popMsg = "Gia tien khong hop le!";
                            }
                        }
                        else if (popupState == 9) { 
                            AdminController::deleteMonAn(dsMon, selectedFoodId);
                            popupState = 0; selectedFoodId = -1;
                        }
                        else if (popupState == 3) { 
                            if (popInp1 != currentUser->getPassword()) popMsg = "Mat khau cu khong dung!";
                            else {
                                bool ok = ClientController::changePassword(dsTaiKhoan, currentUser->getUsername(), popInp2);
                                if (ok) popupState = 0; else popMsg = "Mat khau moi phai khac mat khau cu!";
                            }
                        }
                        else if (popupState == 4) { 
                            int loaiMay = 0;
                            int idMay = ClientController::getAssignedMachine(currentUser->getUsername(), grid, loaiMay);
                            if (idMay != -1) {
                                ClientController::returnMachine(currentUser->getUsername(), idMay, grid);
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
        }
        // ================= RENDER HIỂN THỊ =================
        BeginTextureMode(target);
        ClearBackground(RAYWHITE); 
        if (appState == 0) {
            RaylibUI::drawLoginScreen(appFont, inputUser, inputPass, activeInput, loginMessage);
        } else if (appState == 1) {
            string clockStr = TextFormat("%02d:%02d - %02d/%02d/%d", vHour, vMinute, vDay, vMonth, vYear);
            RaylibUI::drawWorkspace(appFont, clockStr, currentTab, grid, dsTaiKhoan, dsMon, ordersQueue, cartList, dsDatLich,
                currentUser, popupState, activePopInp, 
                (popupState == 12 || popupState == 22) ? selectedBillTime : popInp1, 
                (popupState == 11 || popupState == 12) ? selectedAccountUsername : popInp2, 
                popInp3, popInp4, popInp5, popInp6, popInp7, popMsg, 
                assignScroll, selectedMachine, isHappyHourActive);
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

    UnloadRenderTexture(target);
    UnloadFont(appFont);
    CloseWindow(); 
}
void AppManager::addVirtualMinutes(int mins) {
    int skipSeconds = mins * 60;
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (grid[r][c]->getTrangThai() == 1 && grid[r][c]->getCurrentClient() != nullptr) {
                string u = grid[r][c]->getCurrentClient()->getUsername();
                double rRate = ClientController::getTickRate(grid[r][c]->getLoaiMay(), isHappyHourActive);
                double amount = rRate * skipSeconds;
                
                if (mins > 0) {
                    bool ok = ClientController::processTick(dsTaiKhoan, u, amount);
                    if (!ok) {
                        grid[r][c]->freeMachine();
                        grid[r][c]->setTrangThai(2); 
                        if (currentUser != nullptr && currentUser->getUsername() == u) {
                            appState = 0; currentUser = nullptr; loginMessage = "Tai khoan het tien!";
                        }
                    }
                }
            }
        }
    }
    if (mins > 0) FileManager::saveGrid("Data/MayTram.txt", grid);

    if (mins > 0) {
        vMinute += mins;
        while (vMinute >= 60) { vMinute -= 60; vHour++; }
        while (vHour >= 24) { vHour -= 24; vDay++; if(vDay>30){vDay=1; vMonth++;} if(vMonth>12){vMonth=1; vYear++;} }
    } else {
        vMinute += mins;
        while (vMinute < 0) { vMinute += 60; vHour--; }
        while (vHour < 0) { vHour += 24; vDay--; if(vDay<1){vDay=30; vMonth--;} if(vMonth<1){vMonth=12; vYear--;} }
    }
}