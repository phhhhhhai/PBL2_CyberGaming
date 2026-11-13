#ifndef PRICING_H
#define PRICING_H

class GiaMay {
public:
    virtual double tinhTien1Giay(bool isHappyHour) = 0;
    virtual ~GiaMay() {}
};

class GiaStandard : public GiaMay {
public:
    double tinhTien1Giay(bool isHappyHour) override {
        double gia = 10000.0 / 3600.0;
        return isHappyHour ? gia * 0.7 : gia; 
    }
};

class GiaVIP : public GiaMay {
public:
    double tinhTien1Giay(bool isHappyHour) override {
        double gia = 14000.0 / 3600.0;
        return isHappyHour ? gia * 0.7 : gia;
    }
};

class GiaStream : public GiaMay {
public:
    double tinhTien1Giay(bool isHappyHour) override {
        double gia = 20000.0 / 3600.0;
        return isHappyHour ? gia * 0.7 : gia;
    }
};

class GiaHoaDon {
public:
    virtual double tinhGiamGia(double thanhTienGoc) const = 0;
    virtual double tinhTongCong(double thanhTienGoc) const {
        return thanhTienGoc - tinhGiamGia(thanhTienGoc);
    }
    virtual ~GiaHoaDon() {}
};

class GiaHoaDonGoc : public GiaHoaDon {
public:
    double tinhGiamGia(double thanhTienGoc) const override {
        (void)thanhTienGoc;
        return 0.0;
    }
};

class GiaHoaDonVoucher10 : public GiaHoaDon {
public:
    double tinhGiamGia(double thanhTienGoc) const override {
        return thanhTienGoc * 0.1;
    }
};

#endif