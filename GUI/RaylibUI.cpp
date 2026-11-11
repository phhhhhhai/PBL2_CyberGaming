#include "RaylibUI.h"
#include "../BLL/ClientController.h"
#include <vector>

bool RaylibUI::isChangingMachine = false;
int RaylibUI::sourceMachineId = -1;
bool RaylibUI::showConfirmMoveModal = false;
int RaylibUI::targetMachineId = -1;
string RaylibUI::moveNotificationMsg = "";
float RaylibUI::moveNotificationTimer = 0.0f;

void RaylibUI::drawLoginScreen(Font font, string inputUser, string inputPass, int activeInput, string loginMessage, DoublyLinkedList<User*>& dsTK, int loginScroll) {
    DrawTextEx(font, "CYBER GAMING - HỆ THỐNG QUẢN LÝ", {680, 350}, 35, 1, DARKBLUE);
    
    DrawTextEx(font, "Tên đăng nhập (SĐT):", {750, 430}, 25, 1, DARKGRAY);
    DrawRectangleLines(750, 470, 400, 50, (activeInput == 1) ? BLUE : DARKGRAY); 
    if (inputUser.empty()) {
        if (activeInput != 1) DrawTextEx(font, "Nhấp để nhập...", {760, 480}, 25, 1, LIGHTGRAY);
    } else {
        DrawTextEx(font, inputUser.c_str(), {760, 480}, 25, 1, BLACK);
    }
    
    DrawTextEx(font, "Mật khẩu:", {750, 540}, 25, 1, DARKGRAY);
    DrawRectangleLines(750, 580, 400, 50, (activeInput == 2) ? BLUE : DARKGRAY);
    if (inputPass.empty()) {
        if (activeInput != 2) DrawTextEx(font, "Nhấp để nhập...", {760, 590}, 25, 1, LIGHTGRAY);
    } else {
        string hiddenPass(inputPass.length(), '*');
        DrawTextEx(font, hiddenPass.c_str(), {760, 590}, 25, 1, BLACK);
    }

    if ((activeInput == 1 || activeInput == 2) && ((int)(GetTime() * 2) % 2 == 0)) {
        float textWidth = MeasureTextEx(font, (activeInput == 1) ? inputUser.c_str() : string(inputPass.length(), '*').c_str(), 25, 1).x;
        float cursorY = (activeInput == 1) ? 480 : 590;
        DrawTextEx(font, "_", {765 + textWidth, cursorY}, 25, 1, BLACK);
    }
    
    Rectangle btnLogin = {850, 680, 200, 60};
    DrawRectangleRec(btnLogin, CheckCollisionPointRec(GetMousePosition(), btnLogin) ? DARKBLUE : BLUE);
    DrawTextEx(font, "ĐĂNG NHẬP", {875, 695}, 25, 1, WHITE);

    if (!loginMessage.empty()) {
        DrawTextEx(font, loginMessage.c_str(), {750, 640}, 22, 1, RED);
    }

    float listX = 1220; float listY = 470;
    DrawTextEx(font, "CHỌN NHANH TÀI KHOẢN", {listX, listY - 40}, 25, 1, DARKGREEN);
    DrawRectangle(listX + 390, listY, 40, 40, GRAY); DrawTextEx(font, "/\\", {listX + 400, listY + 10}, 20, 1, WHITE);
    DrawRectangle(listX + 390, listY + 185, 40, 40, GRAY); DrawTextEx(font, "\\/", {listX + 400, listY + 195}, 20, 1, WHITE);
    
    int count = 0; float itemY = listY;
    for (Node<User*>* p = dsTK.getHead(); p != nullptr; p = p->next) {
        if (count >= loginScroll && count < loginScroll + 5) {
            Rectangle itemRec = {listX, itemY, 380, 40};
            bool isSelected = (inputUser == p->data->getUsername());
            DrawRectangleRec(itemRec, isSelected ? SKYBLUE : LIGHTGRAY);
            DrawRectangleLinesEx(itemRec, 1, DARKGRAY);
            
            string roleStr = (p->data->getRoleId() == 1) ? "ADMIN" : "CLIENT";
            string displayStr = p->data->getUsername() + " - " + roleStr;
            DrawTextEx(font, displayStr.c_str(), {listX + 20, itemY + 10}, 22, 1, BLACK);
            itemY += 45;
        }
        count++;
    }
}

void RaylibUI::drawWorkspace(Font font, string clockStr, int currentTab, MayTram* grid[ROWS][COLS], 
    DoublyLinkedList<User*>& dsTK, DoublyLinkedList<MonAn*>& dsMon, Queue<Order*>& ordersQueue, 
    DoublyLinkedList<CartItem*>& cartList, DoublyLinkedList<DatLich*>& dsDatLich, User* currentUser, int popupState, int activePopInp, 
    string p1, string p2, string p3, string p4, string p5, string p6, string p7, string pMsg, 
    int assignScroll, int selectedMachine, bool isHappyHourActive) 
{
    DrawRectangle(0, 0, 1920, 50, DARKGRAY); 
    
    bool isAdmin = false;
    if (currentUser != nullptr) {
        isAdmin = (currentUser->getRoleId() == 1);
    }

    if (isAdmin) {
        auto drawTab = [&](Rectangle rect, const char* text, bool isActive) {
            if (isActive) {
                DrawRectangleRec(rect, RAYWHITE);
                DrawTextEx(font, text, {rect.x + 15, rect.y + 15}, 19, 1, BLACK);
            } else {
                DrawRectangleRec(rect, GRAY);
                DrawTextEx(font, text, {rect.x + 15, rect.y + 15}, 19, 1, LIGHTGRAY);
            }
        };
        drawTab({10, 0, 180, 50}, "1. SƠ ĐỒ", currentTab == 0);
        drawTab({200, 0, 180, 50}, "2. TÀI KHOẢN", currentTab == 1);
        drawTab({390, 0, 180, 50}, "3. F&B", currentTab == 2);
        drawTab({580, 0, 180, 50}, "4. ORDER", currentTab == 3);
        drawTab({770, 0, 180, 50}, "5. BOOKING", currentTab == 4); 

        DrawTextEx(font, "HAPPY HOUR (-30%):", {1340, 15}, 18, 1, WHITE);
        if (isHappyHourActive) {
            DrawRectangle(1530, 10, 70, 30, GREEN); DrawTextEx(font, "BẬT", {1545, 15}, 18, 1, WHITE);
        } else {
            DrawRectangle(1530, 10, 70, 30, GRAY);  DrawTextEx(font, "TẮT", {1545, 15}, 18, 1, WHITE);
        }
    } else {
        auto drawTab = [&](Rectangle rect, const char* text, bool isActive) {
            if (isActive) {
                DrawRectangleRec(rect, RAYWHITE);
                DrawTextEx(font, text, {rect.x + 15, rect.y + 15}, 19, 1, BLACK);
            } else {
                DrawRectangleRec(rect, GRAY);
                DrawTextEx(font, text, {rect.x + 15, rect.y + 15}, 19, 1, LIGHTGRAY);
            }
        };

        drawTab({10, 0, 220, 50}, "1. TỔNG QUAN", currentTab == 0);
        drawTab({240, 0, 220, 50}, "2. DỊCH VỤ F&B", currentTab == 1);
        drawTab({470, 0, 220, 50}, "3. CÀI ĐẶT", currentTab == 2);
    }
    DrawTextEx(font, clockStr.c_str(), {970, 15}, 20, 1, WHITE);
    DrawRectangle(1180, 10, 130, 30, ORANGE); 
    DrawTextEx(font, "CHỈNH GIỜ", {1195, 15}, 18, 1, WHITE);

    DrawRectangle(1750, 5, 150, 40, RED);
    DrawTextEx(font, "ĐĂNG XUẤT", {1765, 15}, 20, 1, WHITE);

    float contentY = 100;
    float tableX = 160;

    if (isAdmin) {
        if (currentTab == 0) {
            DrawTextEx(font, "SƠ ĐỒ PHÒNG MÁY", {100, contentY}, 35, 1, DARKBLUE);
            drawGrid2D(font, grid, 270, contentY + 80);

            DrawTextEx(font, "Trạng thái:", {170, 950}, 25, 1, BLACK);
            DrawRectangle(320, 950, 30, 30, GREEN); DrawTextEx(font, "Trống", {360, 952}, 25, 1, DARKGRAY);
            DrawRectangle(460, 950, 30, 30, BLUE);  DrawTextEx(font, "Chờ bật máy", {500, 952}, 25, 1, DARKGRAY);
            DrawRectangle(660, 950, 30, 30, RED);   DrawTextEx(font, "Đang chơi", {700, 952}, 25, 1, DARKGRAY);
            DrawRectangle(860, 950, 30, 30, ORANGE); DrawTextEx(font, "Dọn dẹp", {900, 952}, 25, 1, DARKGRAY);
            
            DrawTextEx(font, "Loại máy:", {1150, 950}, 25, 1, BLACK);
            DrawRectangle(1270, 950, 60, 30, LIGHTGRAY); DrawTextEx(font, "STD", {1275, 955}, 22, 1, BLACK);
            DrawRectangle(1360, 950, 60, 30, GOLD);      DrawTextEx(font, "VIP", {1365, 955}, 22, 1, BLACK);
            DrawRectangle(1450, 950, 60, 30, SKYBLUE);   DrawTextEx(font, "STR", {1455, 955}, 22, 1, BLACK);
        }
        else if (currentTab == 1) {
            DrawTextEx(font, "QUẢN LÝ TÀI KHOẢN", {100, contentY}, 35, 1, DARKBLUE);
            
            DrawRectangle(tableX, contentY + 80, 1600, 50, DARKBLUE);
            DrawTextEx(font, "STT", {tableX + 20, contentY + 95}, 22, 1, WHITE);
            DrawTextEx(font, "SĐT / Username", {tableX + 150, contentY + 95}, 22, 1, WHITE);
            DrawTextEx(font, "Mật khẩu", {tableX + 450, contentY + 95}, 22, 1, WHITE);
            DrawTextEx(font, "Quyền hạn", {tableX + 650, contentY + 95}, 22, 1, WHITE);
            DrawTextEx(font, "Vị trí máy", {tableX + 850, contentY + 95}, 22, 1, WHITE); 
            DrawTextEx(font, "Số dư (VNĐ)", {tableX + 1050, contentY + 95}, 22, 1, WHITE);

            DrawRectangle(tableX + 1400, contentY + 20, 150, 40, GREEN);
            DrawTextEx(font, "+ THÊM MỚI", {tableX + 1415, contentY + 30}, 20, 1, BLACK);

            float startY = contentY + 150;
            int i = 0;
            for (Node<User*>* p = dsTK.getHead(); p != nullptr; p = p->next) {
                if (i % 2 == 0) DrawRectangle(tableX, startY - 10, 1600, 45, LIGHTGRAY);
                string mayDangNgoi = "-";
                for (int r = 0; r < ROWS; r++) {
                    for (int c = 0; c < COLS; c++) {
                        if (grid[r][c]->getCurrentClient() != nullptr && 
                            grid[r][c]->getCurrentClient()->getUsername() == p->data->getUsername()) {
                            mayDangNgoi = TextFormat("Máy %d", grid[r][c]->getId());
                            break;
                        }
                    }
                }
                
                Client* clientObj = dynamic_cast<Client*>(p->data);
                double soDu = (clientObj != nullptr) ? clientObj->getSoDu() : 0;

                DrawTextEx(font, TextFormat("%d", i + 1), {tableX + 20, startY}, 22, 1, BLACK);
                DrawTextEx(font, p->data->getUsername().c_str(), {tableX + 150, startY}, 22, 1, BLACK);
                DrawTextEx(font, p->data->getPassword().c_str(), {tableX + 450, startY}, 22, 1, BLACK);

                string roleStr = (p->data->getRoleId() == 1) ? "ADMIN" : "CLIENT";
                DrawTextEx(font, roleStr.c_str(), {tableX + 650, startY}, 22, 1, BLACK);

                DrawTextEx(font, mayDangNgoi.c_str(), {tableX + 850, startY}, 22, 1, (mayDangNgoi == "-") ? DARKGRAY : DARKGREEN);
                DrawTextEx(font, TextFormat("%.0f", soDu), {tableX + 1050, startY}, 22, 1, BLUE);
                
                DrawRectangle(tableX + 1280, startY - 5, 80, 30, DARKGREEN); DrawTextEx(font, "Bill", {tableX + 1300, startY}, 20, 1, WHITE); 
                DrawRectangle(tableX + 1380, startY - 5, 80, 30, ORANGE); DrawTextEx(font, "Sửa", {tableX + 1405, startY}, 20, 1, WHITE);
                DrawRectangle(tableX + 1480, startY - 5, 80, 30, RED);    DrawTextEx(font, "Xóa", {tableX + 1500, startY}, 20, 1, WHITE);
                startY += 45;
                i++;
            }
        }
        else if (currentTab == 2) {
            DrawTextEx(font, "QUẢN LÝ THỰC ĐƠN ĐỒ ĂN (F&B)", {100, contentY}, 35, 1, DARKBLUE);
            DrawRectangle(tableX, contentY + 80, 1600, 50, DARKBLUE);
            DrawTextEx(font, "ID", {tableX + 20, contentY + 95}, 22, 1, WHITE);
            DrawTextEx(font, "Tên món ăn / Nước uống", {tableX + 150, contentY + 95}, 22, 1, WHITE);
            DrawTextEx(font, "Đơn giá (VNĐ)", {tableX + 850, contentY + 95}, 22, 1, WHITE);

            DrawRectangle(tableX + 1400, contentY + 20, 150, 40, GREEN);
            DrawTextEx(font, "+ THÊM MÓN", {tableX + 1415, contentY + 30}, 20, 1, BLACK);

            float startY = contentY + 150;
            int i = 0;
            for (Node<MonAn*>* p = dsMon.getHead(); p != nullptr; p = p->next) {
                if (i % 2 == 0) DrawRectangle(tableX, startY - 10, 1600, 45, LIGHTGRAY);
                DrawTextEx(font, TextFormat("%d", p->data->getId()), {tableX + 20, startY}, 22, 1, BLACK);
                DrawTextEx(font, p->data->getTenMon().c_str(), {tableX + 150, startY}, 22, 1, BLACK);
                DrawTextEx(font, TextFormat("%.0f", p->data->getDonGia()), {tableX + 850, startY}, 22, 1, BLUE);
                
                DrawRectangle(tableX + 1380, startY - 5, 80, 30, ORANGE); DrawTextEx(font, "Sửa", {tableX + 1405, startY}, 20, 1, WHITE);
                DrawRectangle(tableX + 1480, startY - 5, 80, 30, RED);    DrawTextEx(font, "Xóa", {tableX + 1500, startY}, 20, 1, WHITE);
                startY += 45;
                i++;
            }
        }
        else if (currentTab == 3) {
            DrawTextEx(font, "QUẢN LÝ ĐƠN HÀNG F&B", {100, contentY}, 35, 1, DARKBLUE);
            DrawRectangle(tableX, contentY + 80, 1600, 50, DARKBLUE);
            
            DrawTextEx(font, "STT", {tableX + 20, contentY + 95}, 20, 1, WHITE);
            DrawTextEx(font, "Thời gian đặt (Mã Bill)", {tableX + 150, contentY + 95}, 20, 1, WHITE);
            DrawTextEx(font, "Tài khoản khách", {tableX + 450, contentY + 95}, 20, 1, WHITE);
            DrawTextEx(font, "Số máy", {tableX + 700, contentY + 95}, 20, 1, WHITE);
            DrawTextEx(font, "Tổng tiền", {tableX + 900, contentY + 95}, 20, 1, WHITE);
            DrawTextEx(font, "Trạng thái", {tableX + 1150, contentY + 95}, 20, 1, WHITE); 
            DrawTextEx(font, "Thao tác", {tableX + 1400, contentY + 95}, 20, 1, WHITE);
            
            struct BillUI {
                int userId; string thoiGian; int machineId;
                double tongTien; int trangThai; int thanhToan;
            };
            Vector<BillUI> bills;
            for (Node<Order*>* p = ordersQueue.getFront(); p != nullptr; p = p->next) {
                Order* o = p->data;
                bool found = false;
                for (int k = 0; k < bills.size(); k++) {
                    if (bills[k].userId == o->getUserId() && bills[k].thoiGian == o->getThoiGian()) {
                        double gia = 0;
                        for (Node<MonAn*>* m = dsMon.getHead(); m != nullptr; m = m->next) {
                            if (m->data->getId() == o->getIdMon()) { gia = m->data->getDonGia(); break; }
                        }
                        bills[k].tongTien += gia * o->getSoLuong();
                        if (o->getStatus() == 0) bills[k].trangThai = 0; 
                        if (o->getPaid() == 0) bills[k].thanhToan = 0;   
                        found = true; break;
                    }
                }
                if (!found) {
                    BillUI b; b.userId = o->getUserId(); b.thoiGian = o->getThoiGian(); b.machineId = o->getMachineId();
                    b.trangThai = o->getStatus(); b.thanhToan = o->getPaid(); b.tongTien = 0;
                    for (Node<MonAn*>* m = dsMon.getHead(); m != nullptr; m = m->next) {
                        if (m->data->getId() == o->getIdMon()) { b.tongTien = m->data->getDonGia() * o->getSoLuong(); break; }
                    }
                    bills.push_back(b);
                }
            }

            float startY = contentY + 150;
            for (int i = 0; i < bills.size(); i++) {
                if (i % 2 == 0) DrawRectangle(tableX, startY - 10, 1600, 45, LIGHTGRAY);
                
                DrawTextEx(font, TextFormat("%d", i + 1), {tableX + 20, startY}, 20, 1, BLACK);
                DrawTextEx(font, bills[i].thoiGian.c_str(), {tableX + 150, startY}, 20, 1, BLACK);

                string sdtKhach = "Unknown";
                for (Node<User*>* u = dsTK.getHead(); u != nullptr; u = u->next) {
                        if (u->data->getId() == bills[i].userId) { 
                        sdtKhach = u->data->getUsername(); 
                        break; 
                    }
                }
                DrawTextEx(font, sdtKhach.c_str(), {tableX + 450, startY}, 20, 1, BLACK);

                DrawTextEx(font, TextFormat("Máy %d", bills[i].machineId), {tableX + 700, startY}, 20, 1, DARKGREEN);
                DrawTextEx(font, TextFormat("%.0f", bills[i].tongTien), {tableX + 900, startY}, 20, 1, BLUE);
                
                string strStatus = ""; Color bgColorStatus;
                if (bills[i].thanhToan == 1) { 
                    strStatus = "Hoàn tất"; 
                    bgColorStatus = GREEN; 
                }
                else if (bills[i].trangThai == 1) { 
                    strStatus = "Chờ TT"; 
                    bgColorStatus = GOLD; 
                }
                else { 
                    strStatus = "Đang làm"; 
                    bgColorStatus = ORANGE; 
                }
                
                float textWidth = MeasureTextEx(font, strStatus.c_str(), 18, 1).x;
                Rectangle statusBg = {tableX + 1140, startY - 5, 110, 30};
                DrawRectangleRec(statusBg, bgColorStatus);
                DrawTextEx(font, strStatus.c_str(), {statusBg.x + (110 - textWidth)/2, startY}, 18, 1, WHITE);

                Rectangle btnDetail = {tableX + 1380, startY - 5, 100, 30};
                Rectangle btnDel = {tableX + 1500, startY - 5, 75, 30};
                DrawRectangleRec(btnDetail, GOLD); DrawTextEx(font, "Chi tiết", {btnDetail.x + 12, startY}, 18, 1, BLACK);
                DrawRectangleRec(btnDel, RED); DrawTextEx(font, "Xóa", {btnDel.x + 20, startY}, 18, 1, WHITE);

                startY += 45;
            }
            if (!pMsg.empty()) DrawTextEx(font, pMsg.c_str(), {160, startY + 20}, 22, 1, RED);
        }
        else if (currentTab == 4) {
            DrawTextEx(font, "QUẢN LÝ LỊCH BOOKING", {100, contentY}, 35, 1, DARKBLUE);
            DrawRectangle(1400, contentY, 200, 45, GREEN);
            DrawTextEx(font, "+ TẠO ĐẶT LỊCH", {1415, contentY + 12}, 20, 1, BLACK);
            DrawRectangle(tableX, contentY + 80, 1600, 50, DARKBLUE);
            DrawTextEx(font, "Mã", {tableX + 20, contentY + 95}, 20, 1, WHITE);
            DrawTextEx(font, "Máy", {tableX + 100, contentY + 95}, 20, 1, WHITE);
            DrawTextEx(font, "Khách hàng", {tableX + 200, contentY + 95}, 20, 1, WHITE);
            DrawTextEx(font, "Ngày đặt", {tableX + 450, contentY + 95}, 20, 1, WHITE);
            DrawTextEx(font, "Giờ đến", {tableX + 700, contentY + 95}, 20, 1, WHITE);
            DrawTextEx(font, "Thời lượng", {tableX + 900, contentY + 95}, 20, 1, WHITE);
            DrawTextEx(font, "Trạng thái", {tableX + 1150, contentY + 95}, 20, 1, WHITE);

            float startY = contentY + 150;
            int i = 0;

            for (Node<DatLich*>* p = dsDatLich.getHead(); p != nullptr; p = p->next) {
                DatLich* b = p->data; 
                
                if (i % 2 == 0) DrawRectangle(tableX, startY - 10, 1600, 45, LIGHTGRAY);
                DrawTextEx(font, TextFormat("%d", b->getId()), {tableX + 20, startY}, 20, 1, BLACK);
                DrawTextEx(font, TextFormat("STR %d", b->getIdMay()), {tableX + 100, startY}, 20, 1, DARKBLUE);
                
                string sdtKhach = "Unknown";
                for (Node<User*>* u = dsTK.getHead(); u != nullptr; u = u->next) {
                    if (u->data->getId() == b->getUserId()) { 
                        sdtKhach = u->data->getUsername(); 
                        break; 
                    }
                }
                DrawTextEx(font, sdtKhach.c_str(), {tableX + 200, startY}, 20, 1, BLACK);
                
                DrawTextEx(font, b->getNgayDat().c_str(), {tableX + 450, startY}, 20, 1, BLACK);
                DrawTextEx(font, TextFormat("%02d:%02d", b->getGioBatDau(), b->getPhutBatDau()), {tableX + 700, startY}, 20, 1, BLACK);
                DrawTextEx(font, TextFormat("%d Phút", b->getThoiLuong()), {tableX + 900, startY}, 20, 1, BLACK);
                
                DrawRectangle(tableX + 1300, startY - 5, 75, 30, GOLD); DrawTextEx(font, "Sửa", {tableX + 1320, startY}, 20, 1, BLACK);
                DrawRectangle(tableX + 1385, startY - 5, 75, 30, RED); DrawTextEx(font, "Xóa", {tableX + 1405, startY}, 20, 1, WHITE);
                
                if (b->getTrangThai() == 0) DrawTextEx(font, "Chưa đến giờ", {tableX + 1150, startY}, 20, 1, ORANGE);
                else if (b->getTrangThai() == 1) DrawTextEx(font, "Đang chơi", {tableX + 1150, startY}, 20, 1, RED);
                else DrawTextEx(font, "Đã xong", {tableX + 1150, startY}, 20, 1, GREEN);
                
                startY += 45;
                i++;
            }
        } 
    } 
    else { 
        DrawTextEx(font, TextFormat("Xin chào: %s", currentUser->getUsername().c_str()), {100, contentY}, 35, 1, DARKBLUE);
        
        Client* currentClient = dynamic_cast<Client*>(currentUser);
        double currentSoDu = (currentClient != nullptr) ? currentClient->getSoDu() : 0;
        
        DrawTextEx(font, TextFormat("Số dư hiện tại: %.0f VNĐ", currentSoDu), {100, contentY + 50}, 35, 1, BLUE);

        int loaiMay = 0, idMay = -1, trangThaiMay = 0;
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                if (grid[r][c]->getCurrentClient() != nullptr && 
                    grid[r][c]->getCurrentClient()->getUsername() == currentUser->getUsername()) {
                    idMay = grid[r][c]->getId(); 
                    loaiMay = grid[r][c]->getLoaiMay(); 
                    trangThaiMay = grid[r][c]->getTrangThai();
                }
            }
        }

        if (currentTab == 0) {
            DrawTextEx(font, "THÔNG TIN TRẠM MÁY", {100, contentY + 120}, 25, 1, DARKGRAY);
            if (idMay != -1) {
                string loaiStr = (loaiMay == 0) ? "STD" : (loaiMay == 1 ? "VIP" : "STR");
                DrawTextEx(font, TextFormat("Vị trí đang ngồi: Máy %d (%s)", idMay, loaiStr.c_str()), {100, contentY + 160}, 30, 1, DARKGREEN);
                
                double rate = ClientController::getTickRate(loaiMay, isHappyHourActive);
                int secRemaining = (int)(currentSoDu / rate);
                int h = secRemaining / 3600;
                int m = (secRemaining % 3600) / 60;
                DrawTextEx(font, TextFormat("Thời gian dự kiến còn lại: %dh %dm", h, m), {600, contentY + 160}, 30, 1, ORANGE);
            } else {
                DrawTextEx(font, "Vị trí: Chưa xếp máy", {100, contentY + 160}, 30, 1, GRAY);
            }
        } 
        else if (currentTab == 1) {
            DrawTextEx(font, "DỊCH VỤ ĂN UỐNG TẠI BÀN", {100, contentY + 120}, 25, 1, DARKGRAY);
            DrawRectangle(100, 200, 400, 80, SKYBLUE); DrawTextEx(font, "MỞ MENU GỌI MÓN", {160, 225}, 28, 1, WHITE);
            DrawRectangle(550, 200, 400, 80, ORANGE);  DrawTextEx(font, "MÓN ĐÃ GỌI", {660, 225}, 28, 1, WHITE);
            DrawTextEx(font, "* Yêu cầu gọi món sẽ được gửi trực tiếp đến quầy thu ngân.", {100, 300}, 20, 1, GRAY);
        } 
        else if (currentTab == 2) {
            DrawTextEx(font, "CÀI ĐẶT TÀI KHOẢN & PHIÊN CHƠI", {100, contentY + 120}, 25, 1, DARKGRAY);
            DrawRectangle(100, 200, 400, 80, GOLD);    DrawTextEx(font, "ĐỔI MẬT KHẨU", {200, 225}, 28, 1, WHITE);
            DrawRectangle(100, 300, 400, 80, RED);     DrawTextEx(font, "TRẢ MÁY", {240, 325}, 28, 1, WHITE);
        }
    }

    if (moveNotificationTimer > 0) {
        moveNotificationTimer -= GetFrameTime();
        DrawRectangle(760, 60, 400, 40, Fade(RED, 0.9f));
        DrawTextEx(font, moveNotificationMsg.c_str(), {780, 70}, 20, 1, WHITE);
    }

    if (popupState != 0) {
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.7f)); 
        
        auto drawInputField = [&](float y, const char* label, string content, int fieldId) {
            DrawTextEx(font, label, {750, y - 30}, 22, 1, DARKGRAY);
            DrawRectangleLines(750, y, 420, 40, (activePopInp == fieldId) ? BLUE : DARKGRAY);
            DrawTextEx(font, content.c_str(), {760, y + 10}, 22, 1, BLACK);
            if (activePopInp == fieldId && ((int)(GetTime() * 2) % 2 == 0)) {
                float textW = MeasureTextEx(font, content.c_str(), 22, 1).x;
                DrawTextEx(font, "_", {765 + textW, y + 10}, 22, 1, BLACK);
            }
        };
       if (popupState == 11 || popupState == 21) {
            DrawRectangle(560, 180, 800, 600, RAYWHITE);
            DrawRectangleLinesEx({560, 180, 800, 600}, 2, DARKBLUE);
            DrawTextEx(font, popupState == 11 ? "DANH SÁCH LẦN GỌI MÓN (ADMIN)" : "LỊCH SỬ GỌI MÓN CỦA BẠN", {750, 220}, 28, 1, DARKBLUE);
            
            int targetUserId = -1;
            if (popupState == 21) targetUserId = currentUser->getId();
            else {
                for (Node<User*>* u = dsTK.getHead(); u != nullptr; u = u->next) {
                    if (u->data->getUsername() == p2) { targetUserId = u->data->getId(); break; }
                }
            }

            std::vector<string> bills;
            for (Node<Order*>* o = ordersQueue.getFront(); o != nullptr; o = o->next) {
                if (o->data->getUserId() == targetUserId) {
                    string tg = o->data->getThoiGian();
                    bool exists = false;
                    for (string b : bills) if (b == tg) { exists = true; break; }
                    if (!exists) bills.push_back(tg);
                }
            }

            DrawRectangle(600, 280, 720, 40, DARKBLUE);
            DrawTextEx(font, "STT", {620, 290}, 20, 1, WHITE);
            DrawTextEx(font, "Thời gian gọi (Mã Bill)", {700, 290}, 20, 1, WHITE);
            DrawTextEx(font, "Vị trí máy", {920, 290}, 20, 1, WHITE);
            DrawTextEx(font, "Thao tác", {1130, 290}, 20, 1, WHITE);

            float bY = 330;
            for (size_t i = 0; i < bills.size(); i++) {
                DrawRectangle(600, bY - 5, 720, 40, (i % 2 == 0) ? LIGHTGRAY : RAYWHITE);
                DrawTextEx(font, TextFormat("%d", i + 1), {620, bY + 5}, 20, 1, BLACK);
                DrawTextEx(font, bills[i].c_str(), {700, bY + 5}, 20, 1, BLACK);
                
                // --- SỬA Ở ĐÂY: Lấy trực tiếp machineId từ order thuộc bill đó ---
                int machineId = -1;
                for (Node<Order*>* o = ordersQueue.getFront(); o != nullptr; o = o->next) {
                    if (o->data->getUserId() == targetUserId && o->data->getThoiGian() == bills[i]) {
                        machineId = o->data->getMachineId(); // Lấy đúng ID máy lúc gọi đơn này
                        break;
                    }
                }
                string viTriStr = (machineId != -1) ? TextFormat("Máy %d", machineId) : "-";
                DrawTextEx(font, viTriStr.c_str(), {920, bY + 5}, 20, 1, DARKGREEN);

                DrawRectangle(1130, bY, 80, 30, ORANGE); 
                DrawTextEx(font, "Chi tiết", {1135, bY + 5}, 18, 1, WHITE);
                bY += 45;
            }
            DrawRectangle(900, 700, 150, 45, GRAY); DrawTextEx(font, "ĐÓNG", {940, 712}, 22, 1, WHITE);
        } 
        
        else if (popupState == 12 || popupState == 22 || popupState == 23) {
            DrawRectangle(510, 150, 900, 700, RAYWHITE);
            DrawRectangleLinesEx({510, 150, 900, 700}, 2, DARKBLUE);
            DrawTextEx(font, (popupState == 22) ? "DANH SÁCH MÓN ĂN ĐÃ GỌI" : "HÓA ĐƠN CHI TIẾT (F&B)", {730, 180}, 28, 1, DARKBLUE);

            string sdtTitle = (popupState == 12) ? TextFormat("Tài khoản: %s", p2.c_str()) : TextFormat("Thời gian: %s", p1.c_str());
            DrawTextEx(font, sdtTitle.c_str(), {550, 240}, 22, 1, BLACK);

            int targetUserId = -1;
            if (popupState == 22) targetUserId = currentUser->getId();
            else {
                for (Node<User*>* u = dsTK.getHead(); u != nullptr; u = u->next) {
                    if (u->data->getUsername() == p2) { targetUserId = u->data->getId(); break; }
                }
            }

           int billMachineId = -1;
            for (Node<Order*>* o = ordersQueue.getFront(); o != nullptr; o = o->next) {
                if (o->data->getUserId() == targetUserId && o->data->getThoiGian() == p1) {
                    billMachineId = o->data->getMachineId(); // Lấy cố định theo đơn hàng
                    break;
                }
            }
            string machineTitle = (billMachineId != -1) ? TextFormat("Vị trí máy: Máy %d", billMachineId) : "Vị trí máy: -";
            DrawTextEx(font, machineTitle.c_str(), {900, 240}, 22, 1, DARKGREEN);
            DrawLineEx({550, 280}, {1350, 280}, 2, LIGHTGRAY);
            
            DrawTextEx(font, "STT", {550, 290}, 20, 1, DARKGRAY);
            DrawTextEx(font, "Tên món", {630, 290}, 20, 1, DARKGRAY);
            DrawTextEx(font, "Thành tiền", {950, 290}, 20, 1, DARKGRAY);

            bool isAdmin = (currentUser != nullptr && currentUser->getRoleId() == 1);
            bool allPaidCheck = true;
            for (Node<Order*>* o = ordersQueue.getFront(); o != nullptr; o = o->next) {
                if (o->data->getUserId() == targetUserId && o->data->getThoiGian() == p1) { 
                    if (o->data->getPaid() == 0) allPaidCheck = false;
                }
            }
            if (isAdmin && popupState == 12) {
                DrawTextEx(font, "Thao tác", {1180, 290}, 20, 1, DARKGRAY);
            }

            float cY = 340; int i = 0; double tongCong = 0; bool allDelivered = true;

            for (Node<Order*>* o = ordersQueue.getFront(); o != nullptr; o = o->next) {
                if (o->data->getUserId() == targetUserId && o->data->getThoiGian() == p1) { 
                    string tenMon = "Unknown"; double gia = 0;
                    for (Node<MonAn*>* m = dsMon.getHead(); m != nullptr; m = m->next) {
                        if (m->data->getId() == o->data->getIdMon()) {
                            tenMon = m->data->getTenMon() + TextFormat("_(x%d)", o->data->getSoLuong());
                            gia = m->data->getDonGia() * o->data->getSoLuong();
                            break;
                        }
                    }
                    tongCong += gia;
                    if (o->data->getStatus() == 0) allDelivered = false;

                    DrawTextEx(font, TextFormat("%d", i + 1), {550, cY}, 20, 1, BLACK);
                    DrawTextEx(font, tenMon.c_str(), {630, cY}, 20, 1, BLACK);
                    DrawTextEx(font, TextFormat("%.0f đ", gia), {950, cY}, 20, 1, BLUE);

                    if (isAdmin && popupState == 12) {
                        Rectangle btnEdit = {1160, cY - 5, 60, 30};
                        Rectangle btnDel = {1230, cY - 5, 60, 30};
                        DrawRectangleRec(btnEdit, GOLD); 
                        DrawTextEx(font, "Sửa", {btnEdit.x + 10, cY}, 18, 1, BLACK);
                        DrawRectangleRec(btnDel, RED); 
                        DrawTextEx(font, "Xóa", {btnDel.x + 10, cY}, 18, 1, WHITE);
                    }

                    cY += 45; i++;
                }
            }

            DrawLineEx({550, 680}, {1350, 680}, 2, LIGHTGRAY);
            DrawTextEx(font, "TỔNG CỘNG:", {550, 710}, 24, 1, BLACK);
            DrawTextEx(font, TextFormat("%.0f VNĐ", tongCong), {900, 710}, 24, 1, RED);

            if (popupState == 12) {
                DrawRectangle(600, 780, 150, 50, GRAY); DrawTextEx(font, "ĐÓNG", {645, 795}, 22, 1, WHITE);
                
                Rectangle btnGiao = {800, 780, 200, 50};
                DrawRectangleRec(btnGiao, allDelivered ? GREEN : ORANGE);
                DrawTextEx(font, allDelivered ? "ĐÃ GIAO" : "XÁC NHẬN GIAO", {820, 795}, 22, 1, WHITE);

                Rectangle btnTT = {1050, 780, 200, 50};
                DrawRectangleRec(btnTT, allPaidCheck ? GREEN : BLUE);
                DrawTextEx(font, allPaidCheck ? "ĐÃ THANH TOÁN" : "XÁC NHẬN TT", {1070, 795}, 22, 1, WHITE);
            } else if (popupState == 23 || popupState == 22) {
                DrawRectangle(870, 780, 150, 50, GRAY); DrawTextEx(font, "ĐÓNG", {915, 795}, 22, 1, WHITE);
            }
        }
        else if (popupState == 20) { 
            Rectangle modalRec = {310, 180, 1300, 720};
            DrawRectangleRec(modalRec, RAYWHITE);
            DrawRectangleLinesEx(modalRec, 3, DARKBLUE);
            DrawTextEx(font, "MENU GỌI ĐỒ ĂN (F&B)", {750, 200}, 30, 1, DARKBLUE);
            DrawLineEx({940, 250}, {940, 820}, 2, LIGHTGRAY);
            DrawTextEx(font, "DANH SÁCH MÓN ĂN", {350, 255}, 24, 1, DARKGREEN);
            
            float leftY = 300;
            int i = 0;
            for (Node<MonAn*>* p = dsMon.getHead(); p != nullptr; p = p->next) {
                DrawRectangle(340, leftY - 5, 570, 45, (i % 2 == 0) ? LIGHTGRAY : RAYWHITE);
                DrawTextEx(font, p->data->getTenMon().c_str(), {350, leftY + 5}, 20, 1, BLACK);
                DrawTextEx(font, TextFormat("%.0f đ", p->data->getDonGia()), {680, leftY + 5}, 20, 1, BLUE);
                Rectangle btnThem = {810, leftY, 90, 32};
                DrawRectangleRec(btnThem, GREEN); DrawTextEx(font, "+ THÊM", {btnThem.x + 10, btnThem.y + 6}, 18, 1, WHITE);
                leftY += 50; i++;
            }

            DrawTextEx(font, "GIỎ HÀNG CỦA BẠN", {970, 255}, 24, 1, DARKBLUE);
            DrawRectangle(960, 290, 620, 35, DARKBLUE);
            DrawTextEx(font, "Tên món", {970, 298}, 18, 1, WHITE);
            DrawTextEx(font, "SL", {1200, 298}, 18, 1, WHITE);
            DrawTextEx(font, "Thành tiền", {1320, 298}, 18, 1, WHITE);
            DrawTextEx(font, "Xóa", {1510, 298}, 18, 1, WHITE);

            double tongTienCart = 0;
            float rightY = 335;
            int cIdx = 0;

            for (Node<CartItem*>* c = cartList.getHead(); c != nullptr; c = c->next) {
                double thanhTien = c->data->getDonGia() * c->data->getSoLuong();
                tongTienCart += thanhTien;

                DrawRectangle(960, rightY - 5, 620, 40, (cIdx % 2 == 0) ? LIGHTGRAY : RAYWHITE);
                DrawTextEx(font, c->data->getTenMon().c_str(), {970, rightY + 3}, 18, 1, BLACK);

                Rectangle btnSub = {1170, rightY, 28, 28};
                Rectangle btnAdd = {1245, rightY, 28, 28};
                DrawRectangleRec(btnSub, GRAY); DrawTextEx(font, "-", {btnSub.x + 9, btnSub.y + 3}, 20, 1, WHITE);
                DrawTextEx(font, TextFormat("%d", c->data->getSoLuong()), {1205, rightY + 3}, 18, 1, BLACK);
                DrawRectangleRec(btnAdd, GRAY); DrawTextEx(font, "+", {btnAdd.x + 7, btnAdd.y + 3}, 20, 1, WHITE);

                DrawTextEx(font, TextFormat("%.0f đ", thanhTien), {1320, rightY + 3}, 18, 1, BLUE);

                Rectangle btnXoa = {1505, rightY, 35, 28};
                DrawRectangleRec(btnXoa, RED); DrawTextEx(font, "X", {btnXoa.x + 11, btnXoa.y + 3}, 18, 1, WHITE);

                rightY += 45;
                cIdx++;
            }

            DrawTextEx(font, TextFormat("TỔNG CỘNG:  %.0f VNĐ", tongTienCart), {970, 730}, 25, 1, RED);

            Rectangle btnDongMenu = {350, 830, 180, 50};
            DrawRectangleRec(btnDongMenu, GRAY);
            DrawTextEx(font, "ĐÓNG", {btnDongMenu.x + 60, btnDongMenu.y + 12}, 20, 1, WHITE);
            Rectangle btnXacNhanDat = {1280, 830, 280, 50};
            if (cartList.getSize() > 0) {
                DrawRectangleRec(btnXacNhanDat, DARKBLUE); 
            } else {
                DrawRectangleRec(btnXacNhanDat, LIGHTGRAY); 
            }
            DrawTextEx(font, "XÁC NHẬN ĐẶT", {btnXacNhanDat.x + 50, btnXacNhanDat.y + 12}, 22, 1, WHITE);
            
            if (!pMsg.empty()) {
                DrawTextEx(font, pMsg.c_str(), {750, 845}, 20, 1, GREEN); 
            }
        }
        else if (popupState == 13) {
            DrawRectangle(700, 300, 520, 400, RAYWHITE);
            DrawRectangleLinesEx({700, 300, 520, 400}, 2, DARKBLUE);
            DrawTextEx(font, "ĐIỀU CHỈNH SỐ LƯỢNG MÓN", {770, 330}, 26, 1, DARKBLUE);
            drawInputField(450, "Nhập số lượng mới:", p1, 1);
            if (!pMsg.empty()) DrawTextEx(font, pMsg.c_str(), {750, 530}, 20, 1, RED);
            DrawRectangle(800, 600, 150, 45, GRAY); DrawTextEx(font, "HỦY", {845, 610}, 22, 1, WHITE);
            DrawRectangle(970, 600, 150, 45, DARKBLUE); DrawTextEx(font, "XÁC NHẬN", {985, 610}, 22, 1, WHITE);
        }
        else if (popupState == 10) {
            DrawRectangle(700, 250, 520, 330, RAYWHITE);
            DrawRectangleLines(700, 250, 520, 330, DARKBLUE);
            DrawTextEx(font, TextFormat("BẢNG ĐIỀU KHIỂN MÁY %d", selectedMachine), {800, 280}, 28, 1, DARKBLUE);

            string sUser = ""; double sSoDu = 0; int sStatus = 0; int sLoai = 0;
            for(int r=0;r<ROWS;r++) for(int c=0;c<COLS;c++) if(grid[r][c]->getId() == selectedMachine) {
                if (grid[r][c]->getCurrentClient()) {
                    sUser = grid[r][c]->getCurrentClient()->getUsername();
                    sSoDu = grid[r][c]->getCurrentClient()->getSoDu();
                }
                sStatus = grid[r][c]->getTrangThai(); sLoai = grid[r][c]->getLoaiMay();
            }

            DrawTextEx(font, TextFormat("Khách hàng: %s", sUser.c_str()), {750, 340}, 25, 1, BLACK);
            DrawTextEx(font, TextFormat("Số dư: %.0f VNĐ", sSoDu), {750, 390}, 25, 1, BLUE);
            
            double rate = ClientController::getTickRate(sLoai, isHappyHourActive);
            int secRem = (int)(sSoDu / rate);
            DrawTextEx(font, TextFormat("Thời gian còn: %dh %dm", secRem/3600, (secRem%3600)/60), {750, 440}, 25, 1, ORANGE);
            
            DrawRectangle(750, 480, 200, 45, GOLD); DrawTextEx(font, "ĐỔI MÁY", {800, 492}, 22, 1, BLACK);
            DrawRectangle(980, 480, 150, 45, GRAY); DrawTextEx(font, "ĐÓNG", {1025, 492}, 22, 1, WHITE);
            if (!pMsg.empty()) DrawTextEx(font, pMsg.c_str(), {750, 540}, 20, 1, RED);
        }
        else if (popupState == 15) {
            DrawRectangle(700, 350, 520, 250, RAYWHITE);
            DrawRectangleLines(700, 350, 520, 250, DARKBLUE);
            DrawTextEx(font, "ĐIỀU CHỈNH THỜI GIAN", {730, 370}, 22, 1, DARKBLUE);
            DrawRectangle(770, 440, 100, 40, SKYBLUE); DrawTextEx(font, "+15P", {795, 450}, 20, 1, WHITE);
            DrawRectangle(890, 440, 100, 40, BLUE);    DrawTextEx(font, "+1H", {920, 450}, 20, 1, WHITE);
            DrawRectangle(1010, 440, 100, 40, DARKBLUE);DrawTextEx(font, "+1D", {1040, 450}, 20, 1, WHITE);
            DrawRectangle(880, 520, 150, 45, GRAY); DrawTextEx(font, "ĐÓNG", {925, 530}, 22, 1, WHITE);
        }
        else if (popupState == 14) {
            DrawRectangle(450, 180, 1000, 580, RAYWHITE);
            DrawRectangleLines(450, 180, 1000, 580, DARKBLUE);
            DrawTextEx(font, "TẠO LỊCH BOOKING (MÁY STREAM)", {750, 210}, 28, 1, DARKBLUE);

            DrawTextEx(font, "--- THÔNG TIN TÀI KHOẢN ---", {500, 270}, 22, 1, DARKGREEN);
            DrawTextEx(font, "--- THÔNG TIN ĐẶT LỊCH ---", {980, 270}, 22, 1, DARKGREEN);

            auto drawCustomInput = [&](float x, float y, float w, const char* label, string content, int fieldId) {
                DrawTextEx(font, label, {x, y - 30}, 20, 1, DARKGRAY);
                DrawRectangleLines(x, y, w, 40, (activePopInp == fieldId) ? BLUE : DARKGRAY);
                DrawTextEx(font, content.c_str(), {x + 10, y + 10}, 22, 1, BLACK);
                if (activePopInp == fieldId && ((int)(GetTime() * 2) % 2 == 0)) {
                    float textW = MeasureTextEx(font, content.c_str(), 22, 1).x;
                    DrawTextEx(font, "_", {x + 15 + textW, y + 10}, 22, 1, BLACK);
                }
            };

            drawCustomInput(500, 340, 420, "SĐT Khách hàng:", p1, 1);
            drawCustomInput(500, 440, 420, "Mật khẩu:", p7, 7);

            drawCustomInput(980, 340, 400, "Số Máy (ID):", p2, 2);
            drawCustomInput(980, 440, 400, "Ngày đặt (DD/MM/YYYY):", p3, 3);
            drawCustomInput(980, 540, 180, "Giờ bắt đầu:", p4, 4);
            drawCustomInput(1200, 540, 180, "Phút bắt đầu:", p5, 5);
            drawCustomInput(980, 640, 400, "Thời lượng chơi dự kiến (Phút - VD: 120):", p6, 6);

            if (!pMsg.empty()) DrawTextEx(font, pMsg.c_str(), {500, 550}, 20, 1, RED);
            
            // Nút bấm riêng cho popup Đặt lịch (Y=700)
            DrawRectangle(800, 700, 150, 45, GRAY); DrawTextEx(font, "HỦY", {845, 712}, 22, 1, WHITE);
            DrawRectangle(970, 700, 150, 45, DARKBLUE); DrawTextEx(font, "XÁC NHẬN", {985, 712}, 22, 1, WHITE);
        }
        else {
            DrawRectangle(700, 300, 520, 450, RAYWHITE);
            DrawRectangleLines(700, 300, 520, 450, DARKBLUE);
            
            if (popupState == 1) { 
                DrawTextEx(font, TextFormat("XẾP MÁY %d CHO KHÁCH", selectedMachine), {800, 330}, 28, 1, DARKBLUE);
                DrawTextEx(font, "Chọn khách hàng đang rảnh:", {750, 370}, 22, 1, DARKGRAY);
                int countUnassigned = 0; float listY = 400;
                
                for (Node<User*>* p = dsTK.getHead(); p != nullptr; p = p->next) {
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
                                Rectangle itemRec = {750, listY, 370, 40};
                                bool isSelected = (p1 == p->data->getUsername());
                                DrawRectangleRec(itemRec, isSelected ? SKYBLUE : LIGHTGRAY);
                                DrawRectangleLinesEx(itemRec, 1, DARKGRAY);
                                
                                Client* cObj = dynamic_cast<Client*>(p->data);
                                double soDuTmp = cObj ? cObj->getSoDu() : 0;
                                
                                DrawTextEx(font, TextFormat("%s - Dư: %.0f", p->data->getUsername().c_str(), soDuTmp), {760, listY + 10}, 22, 1, BLACK);
                                listY += 45;
                            }
                            countUnassigned++;
                        }
                    }
                }
                if (countUnassigned == 0) DrawTextEx(font, "Khong co khach hang nao ranh!", {750, 450}, 22, 1, RED);
                DrawRectangle(1130, 400, 40, 40, GRAY); DrawTextEx(font, "/\\", {1140, 410}, 20, 1, WHITE);
                DrawRectangle(1130, 580, 40, 40, GRAY); DrawTextEx(font, "\\/", {1140, 590}, 20, 1, WHITE);
            } 
            else if (popupState == 2) { 
                DrawTextEx(font, "THÊM TÀI KHOẢN MỚI", {800, 330}, 28, 1, DARKBLUE);
                drawInputField(380, "Số điện thoại:", p1, 1);
                drawInputField(460, "Mật khẩu:", p2, 2);
                drawInputField(540, "Số dư ban đầu (VNĐ):", p3, 3);
            }
            else if (popupState == 3) { 
                DrawTextEx(font, "ĐỔI MẬT KHẨU", {860, 330}, 28, 1, DARKBLUE);
                drawInputField(450, "Mật khẩu cũ:", p1, 1);
                drawInputField(540, "Mật khẩu mới:", p2, 2);
            }
            else if (popupState == 4) { 
                DrawTextEx(font, "XÁC NHẬN TRẢ MÁY", {830, 330}, 28, 1, DARKBLUE);
                DrawTextEx(font, "Bạn có chắc chắn muốn kết thúc\nphiên chơi và trả máy không?", {750, 450}, 25, 1, BLACK);
            }
            else if (popupState == 5) { 
                DrawTextEx(font, "SỬA TÀI KHOẢN / NẠP TIỀN", {780, 330}, 28, 1, DARKBLUE);
                drawInputField(450, "Mật khẩu (Gõ để đổi):", p1, 1);
                drawInputField(540, "Số tiền nạp thêm (VNĐ):", p2, 2);
            }
            else if (popupState == 6 || popupState == 9) { 
                DrawTextEx(font, "XÁC NHẬN XÓA", {860, 330}, 28, 1, DARKBLUE);
                DrawTextEx(font, "Dữ liệu sẽ bị xóa vĩnh viễn khỏi\nhệ thống. Tiếp tục?", {750, 450}, 25, 1, BLACK);
            }
            else if (popupState == 7) { 
                DrawTextEx(font, "THÊM MÓN ĂN MỚI", {830, 330}, 28, 1, DARKBLUE);
                drawInputField(450, "Tên món (Viết_lien_khong_dau):", p1, 1);
                drawInputField(540, "Đơn giá (VNĐ):", p2, 2);
            }
            else if (popupState == 8) { 
                DrawTextEx(font, "CẬP NHẬT MÓN ĂN", {830, 330}, 28, 1, DARKBLUE);
                drawInputField(450, "Tên món mới:", p1, 1);
                drawInputField(540, "Đơn giá mới (VNĐ):", p2, 2);
            }

            if (!pMsg.empty()) DrawTextEx(font, pMsg.c_str(), {750, 600}, 20, 1, RED);
            
            // Nút bấm chung cho các popup 1 đến 9 (Y=650)
            DrawRectangle(800, 650, 150, 50, GRAY); DrawTextEx(font, "HỦY", {845, 662}, 25, 1, WHITE);
            DrawRectangle(970, 650, 150, 50, DARKBLUE); DrawTextEx(font, "XÁC NHẬN", {985, 662}, 25, 1, WHITE);
        }
    }

    if (showConfirmMoveModal) {
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.5f));
        
        Rectangle confirmBox = {760, 400, 400, 200};
        DrawRectangleRec(confirmBox, RAYWHITE);
        DrawRectangleLinesEx(confirmBox, 2, DARKBLUE);      
        DrawTextEx(font, "XÁC NHẬN ĐỔI MÁY", {830, 420}, 22, 1, DARKBLUE);
        string confirmText = TextFormat("Bạn có muốn đổi từ máy %d sang máy %d?", sourceMachineId, targetMachineId);
        DrawTextEx(font, confirmText.c_str(), {790, 480}, 18, 1, BLACK);
        
        Rectangle btnYes = {800, 530, 140, 45};
        Rectangle btnNo = {980, 530, 140, 45};
        
        DrawRectangleRec(btnYes, GREEN); DrawTextEx(font, "Xác nhận", {830, 542}, 18, 1, WHITE);
        DrawRectangleRec(btnNo, RED); DrawTextEx(font, "Hủy", {1035, 542}, 18, 1, WHITE);
    }
}

void RaylibUI::drawGrid2D(Font font, MayTram* grid[ROWS][COLS], int offsetX, int offsetY) {
    int boxWidth = 100, boxHeight = 120, paddingX = 30, paddingY = 40, centerGap = 120; 

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int extraSpace = (j >= 5) ? centerGap : 0;
            int x = offsetX + j * (boxWidth + paddingX) + extraSpace;
            int y = offsetY + i * (boxHeight + paddingY);

            int trangThai = grid[i][j]->getTrangThai();
            int loaiMay = grid[i][j]->getLoaiMay();

            Color statusColor;
            if (trangThai == 0) statusColor = GREEN;
            else if (trangThai == 1) statusColor = RED;
            else if (trangThai == 3) statusColor = BLUE; 
            else statusColor = ORANGE;

            Color typeColor;
            const char* typeName = (loaiMay == 0) ? "STD" : (loaiMay == 1 ? "VIP" : "STR");
            typeColor = (loaiMay == 0) ? LIGHTGRAY : (loaiMay == 1 ? GOLD : SKYBLUE);

            DrawRectangle(x, y + 30, boxWidth, boxHeight - 30, statusColor);
            DrawRectangle(x, y, boxWidth, 30, typeColor);
            DrawRectangleLines(x, y, boxWidth, boxHeight, DARKGRAY);
            DrawLine(x, y + 30, x + boxWidth, y + 30, DARKGRAY); 

            float typeWidth = MeasureTextEx(font, typeName, 20, 1).x;
            DrawTextEx(font, typeName, {(float)(x + (boxWidth - typeWidth) / 2), (float)(y + 5)}, 20, 1, BLACK);

            const char* idText = TextFormat("%d", grid[i][j]->getId());
            float idWidth = MeasureTextEx(font, idText, 35, 1).x;
            DrawTextEx(font, idText, {(float)(x + (boxWidth - idWidth) / 2), (float)(y + 60)}, 35, 1, WHITE);
            
            if (trangThai == 2 && ((int)(GetTime() * 4) % 2 == 0)) {
                DrawRectangleLinesEx({(float)x, (float)y, (float)boxWidth, (float)boxHeight}, 3, YELLOW);
            }
            if ((trangThai == 1 || trangThai == 3) && grid[i][j]->getCurrentClient() != nullptr) { 
                DrawTextEx(font, grid[i][j]->getCurrentClient()->getUsername().c_str(), {(float)x+5, (float)y+95}, 16, 1, WHITE);
            }
        }
    }
}