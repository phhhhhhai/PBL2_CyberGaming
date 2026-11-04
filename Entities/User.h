#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

class User {
protected:
    int id;
    string username; 
    string password;
    int roleId; // 1: ADMIN, 2: CLIENT
    int status; // 1: Hoạt động, 0: Đã xóa  

public:
    User(int id = 0, string u = "", string p = "", int r = 2, int st = 1) 
        : id(id), username(u), password(p), roleId(r), status(st) {}
    virtual ~User() {}

    string getUsername() const { return username; }
    string getPassword() const { return password; }
    void setPassword(string p) { password = p; }
    
    int getId() const { return id; }
    int getRoleId() const { return roleId; }
    int getStatus() const { return status; }
    void setStatus(int st) { status = st; }
    
    virtual void showInfo() const = 0; 
};

#endif