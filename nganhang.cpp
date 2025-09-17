#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <locale>
#include <codecvt>
#include <sstream>

// Sử dụng std::string cho chuỗi tiếng Việt và đặt encoding cho console
// để đảm bảo tương thích trên nhiều hệ điều hành.
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

// Class GiaoDich - đại diện cho một giao dịch trong hệ thống ngân hàng
class GiaoDich {
private:
    double soTien;
    string loaiGiaoDich;
    string thoiGian;
    string moTa;

public:
    GiaoDich(double tien, string loai, string mota)
        : soTien(tien), loaiGiaoDich(loai), moTa(mota) {
        time_t bayGio = time(0);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&bayGio));
        thoiGian = buffer;
    }

    void hienThi() const {
        cout << thoiGian << " | " << loaiGiaoDich << " | $" << soTien << " | " << moTa << endl;
    }

    double laySoTien() const { return soTien; }
    string layLoaiGiaoDich() const { return loaiGiaoDich; }
    string layMoTa() const { return moTa; }
};

// Class TaiKhoan - đại diện cho tài khoản ngân hàng
class TaiKhoan {
protected:
    string soTaiKhoan;
    double soDu;
    string tenChuTaiKhoan;
    vector<GiaoDich> lichSuGiaoDich;

public:
    TaiKhoan(string soTK, string tenChuTK, double soTienBanDau)
        : soTaiKhoan(soTK), tenChuTaiKhoan(tenChuTK), soDu(soTienBanDau) {
        if (soTienBanDau > 0) {
            lichSuGiaoDich.push_back(GiaoDich(soTienBanDau, "Nap tien", "So tien gui ban dau"));
        }
    }

    virtual ~TaiKhoan() {}

    virtual void napTien(double soTien, string moTa = "Nap tien") {
        if (soTien > 0) {
            soDu += soTien;
            lichSuGiaoDich.push_back(GiaoDich(soTien, "Nap tien", moTa));
            cout << "Da nap $" << fixed << setprecision(2) << soTien << " vao tai khoan " << soTaiKhoan << endl;
        } else {
            cout << "So tien nap vao khong hop le. Vui long kiem tra lai!" << endl;
        }
    }

    virtual bool rutTien(double soTien, string moTa = "Rut tien") {
        if (soTien > 0 && soDu >= soTien) {
            soDu -= soTien;
            lichSuGiaoDich.push_back(GiaoDich(soTien, "Rut tien", moTa));
            cout << "Da rut $" << fixed << setprecision(2) << soTien << " tu tai khoan " << soTaiKhoan << endl;
            return true;
        } else {
            cout << "So du khong du hoac so tien khong hop le. Giao dich that bai!" << endl;
            return false;
        }
    }

    virtual void hienThiThongTin() const {
        cout << "\n--- Thong tin tai khoan ---" << endl;
        cout << "So tai khoan: " << soTaiKhoan << endl;
        cout << "Chu tai khoan: " << tenChuTaiKhoan << endl;
        cout << "So du hien tai: $" << fixed << setprecision(2) << soDu << endl;
        
        if (!lichSuGiaoDich.empty()) {
            cout << "Lich su giao dich:" << endl;
            for (const auto& giaoDich : lichSuGiaoDich) {
                cout << "  ";
                giaoDich.hienThi();
            }
        } else {
            cout << "Chua co giao dich nao duoc thuc hien." << endl;
        }
    }

    TaiKhoan& operator+=(const GiaoDich& giaoDich) {
        if (giaoDich.layLoaiGiaoDich() == "Nap tien") {
            soDu += giaoDich.laySoTien();
        } else if (giaoDich.layLoaiGiaoDich() == "Rut tien") {
            soDu -= giaoDich.laySoTien();
        }
        lichSuGiaoDich.push_back(giaoDich);
        return *this;
    }

    bool operator==(const TaiKhoan& taiKhoanKhac) const {
        return soDu == taiKhoanKhac.soDu;
    }

    bool operator<(const TaiKhoan& taiKhoanKhac) const {
        return soDu < taiKhoanKhac.soDu;
    }

    string laySoTaiKhoan() const { return soTaiKhoan; }
    string layTenChuTaiKhoan() const { return tenChuTaiKhoan; }
    double laySoDu() const { return soDu; }
    vector<GiaoDich> layLichSuGiaoDich() const { return lichSuGiaoDich; }
};

// Class TaiKhoanTietKiem kế thừa từ TaiKhoan
class TaiKhoanTietKiem : public TaiKhoan {
private:
    double laiSuat;
    int gioiHanRut;
    int soLanRutTrongThang;

public:
    TaiKhoanTietKiem(string soTK, string tenChuTK, double soTienBanDau,
                     double lai = 0.025, int gioiHan = 2) // Giảm giới hạn rút để dễ test
        : TaiKhoan(soTK, tenChuTK, soTienBanDau), laiSuat(lai),
          gioiHanRut(gioiHan), soLanRutTrongThang(0) {}

    bool rutTien(double soTien, string moTa = "Rut tien") override {
        if (soLanRutTrongThang >= gioiHanRut) {
            cout << "Da vuot qua so lan rut tien cho phep trong thang. Khong the thuc hien giao dich!" << endl;
            return false;
        }
        if (TaiKhoan::rutTien(soTien, moTa)) {
            soLanRutTrongThang++;
            return true;
        }
        return false;
    }

    void tinhLai() {
        double tienLai = soDu * laiSuat;
        napTien(tienLai, "Tien lai hang thang");
        cout << "Da ap dung lai suat: $" << fixed << setprecision(2) << tienLai << " vao tai khoan." << endl;
    }

    void resetSoLanRut() {
        soLanRutTrongThang = 0;
        cout << "Da reset so lan rut tien cho thang moi." << endl;
    }

    double layLaiSuat() const { return laiSuat; }
    int layGioiHanRut() const { return gioiHanRut; }
    int laySoLanRutTrongThang() const { return soLanRutTrongThang; }
};

// Class KhachHang đại diện cho người dùng
class KhachHang {
private:
    string maKhachHang;
    string hoTen;
    vector<unique_ptr<TaiKhoan>> danhSachTaiKhoan;

public:
    KhachHang(string maKH, string ten) : maKhachHang(maKH), hoTen(ten) {}

    void themTaiKhoan(unique_ptr<TaiKhoan> taiKhoan) {
        cout << "Da them tai khoan " << taiKhoan->laySoTaiKhoan() << " cho khach hang " << hoTen << endl;
        danhSachTaiKhoan.push_back(move(taiKhoan));
    }

    TaiKhoan* timTaiKhoan(string soTK) const {
        for (const auto& tk : danhSachTaiKhoan) {
            if (tk->laySoTaiKhoan() == soTK) {
                return tk.get();
            }
        }
        return nullptr;
    }

    double tinhTongSoDu() const {
        double tong = 0;
        for (const auto& taiKhoan : danhSachTaiKhoan) {
            tong += taiKhoan->laySoDu();
        }
        return tong;
    }

    void hienThiThongTin() const {
        cout << "\n=== Thong tin khach hang ===" << endl;
        cout << "Ma KH: " << maKhachHang << endl;
        cout << "Ho ten: " << hoTen << endl;
        cout << "So tai khoan: " << danhSachTaiKhoan.size() << endl;
        cout << "Tong so du: $" << fixed << setprecision(2) << tinhTongSoDu() << endl;
        
        if (!danhSachTaiKhoan.empty()) {
            cout << "Danh sach tai khoan:" << endl;
            for (const auto& taiKhoan : danhSachTaiKhoan) {
                cout << "  - " << taiKhoan->laySoTaiKhoan() << " | So du: $" << taiKhoan->laySoDu() << endl;
            }
        }
    }

    string layMaKhachHang() const { return maKhachHang; }
    string layHoTen() const { return hoTen; }
};

// Class NganHang quản lý toàn bộ hoạt động
class NganHang {
private:
    vector<KhachHang> danhSachKhachHang;
    int boDemTaiKhoan;

public:
    NganHang() : boDemTaiKhoan(1000) {}

    KhachHang* taoKhachHang(string ten) {
        string maKH = "KH" + to_string(danhSachKhachHang.size() + 1);
        danhSachKhachHang.emplace_back(maKH, ten);
        cout << "Da tao khach hang moi: " << ten << " (Ma: " << maKH << ")" << endl;
        return &danhSachKhachHang.back();
    }

    void taoTaiKhoan(KhachHang* khachHang, const string& loaiTaiKhoan, double soTienBanDau = 0) {
        if (!khachHang) {
            cout << "Khong tim thay khach hang!" << endl;
            return;
        }

        string soTK = "TK" + to_string(boDemTaiKhoan++);
        unique_ptr<TaiKhoan> taiKhoanMoi;

        if (loaiTaiKhoan == "Tiet kiem") {
            taiKhoanMoi = make_unique<TaiKhoanTietKiem>(soTK, khachHang->layHoTen(), soTienBanDau);
            cout << "Da tao tai khoan tiet kiem: " << soTK << endl;
        } else {
            taiKhoanMoi = make_unique<TaiKhoan>(soTK, khachHang->layHoTen(), soTienBanDau);
            cout << "Da tao tai khoan thuong: " << soTK << endl;
        }

        khachHang->themTaiKhoan(move(taiKhoanMoi));
    }

    KhachHang* timKhachHang(string maKH) {
        for (auto& khachHang : danhSachKhachHang) {
            if (khachHang.layMaKhachHang() == maKH) {
                return &khachHang;
            }
        }
        return nullptr;
    }

    bool chuyenTien(string maKH_nguon, string soTK_nguon, string maKH_dich, string soTK_dich, double soTien, string moTa = "Chuyen khoan") {
        KhachHang* kh_nguon = timKhachHang(maKH_nguon);
        KhachHang* kh_dich = timKhachHang(maKH_dich);

        if (!kh_nguon || !kh_dich) {
            cout << "Mot hoac ca hai khach hang khong ton tai. Vui long kiem tra lai!" << endl;
            return false;
        }

        TaiKhoan* nguon = kh_nguon->timTaiKhoan(soTK_nguon);
        TaiKhoan* dich = kh_dich->timTaiKhoan(soTK_dich);

        if (!nguon || !dich) {
            cout << "Mot hoac ca hai tai khoan khong ton tai. Vui long kiem tra lai!" << endl;
            return false;
        }

        if (nguon->rutTien(soTien, "Chuyen den " + soTK_dich + ": " + moTa)) {
            dich->napTien(soTien, "Chuyen tu " + soTK_nguon + ": " + moTa);
            cout << "Chuyen tien thanh cong!" << endl;
            return true;
        } else {
            cout << "Chuyen tien that bai. Vui long kiem tra so du tai khoan!" << endl;
            return false;
        }
    }
};

void config_console_encoding() {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif
}

int main() {
    config_console_encoding();
    cout << "=== HE THONG QUAN LY TAI KHOAN NGAN HANG ===" << endl;
    cout << "Khoi tao he thong..." << endl;
    
    NganHang nganHang;

    cout << "\n--- Tao khach hang ---" << endl;
    KhachHang* khachHang1 = nganHang.taoKhachHang("Nguyen Van An");
    KhachHang* khachHang2 = nganHang.taoKhachHang("Tran Thi Bich");

    cout << "\n--- Tao tai khoan ---" << endl;
    nganHang.taoTaiKhoan(khachHang1, "Thuong", 500);
    nganHang.taoTaiKhoan(khachHang1, "Tiet kiem", 1000);
    nganHang.taoTaiKhoan(khachHang2, "Thuong", 1500);

    TaiKhoan* tk1 = khachHang1->timTaiKhoan("TK1000");
    TaiKhoan* tk2 = khachHang1->timTaiKhoan("TK1001");
    TaiKhoan* tk3 = khachHang2->timTaiKhoan("TK1002");

    cout << "\n--- Thuc hien giao dich ---" << endl;
    if (tk1) tk1->napTien(200, "Luong thang");
    if (tk1) tk1->rutTien(50, "Mua sach");
    if (tk2) tk2->rutTien(100, "Di sieu thi");
    
    cout << "\n--- Chuyen tien ---" << endl;
    nganHang.chuyenTien("KH1", "TK1000", "KH1", "TK1001", 150, "Thanh toan hoa don");

    cout << "\n--- Su dung toan tu ---" << endl;
    GiaoDich gd(300, "Nap tien", "Nap tien qua toan tu +=");
    if (tk1) {
        *tk1 += gd;
        cout << "Da them giao dich qua toan tu +=. So du moi: $" << fixed << setprecision(2) << tk1->laySoDu() << endl;
    }

    if (tk1 && tk2) {
        cout << "\n--- So sanh tai khoan ---" << endl;
        if (*tk1 == *tk2) {
            cout << "Hai tai khoan co so du bang nhau." << endl;
        } else if (*tk1 < *tk2) {
            cout << "Tai khoan 1 co so du it hon tai khoan 2." << endl;
        } else {
            cout << "Tai khoan 1 co so du nhieu hon tai khoan 2." << endl;
        }
    }

    cout << "\n--- Kiem tra tai khoan tiet kiem ---" << endl;
    TaiKhoanTietKiem* tktk = dynamic_cast<TaiKhoanTietKiem*>(tk2);
    if (tktk) {
        tktk->resetSoLanRut();
        for (int i = 0; i < 5; i++) {
            tktk->rutTien(10, "Rut thu nghiem");
        }
        tktk->tinhLai();
    }

    cout << "\n--- Thong tin khach hang ---" << endl;
    khachHang1->hienThiThongTin();
    cout << endl;
    khachHang2->hienThiThongTin();

    cout << "\n--- Chi tiet tai khoan ---" << endl;
    if (tk1) tk1->hienThiThongTin();
    cout << endl;
    if (tk2) tk2->hienThiThongTin();

    cout << "\n=== KET THUC CHUONG TRINH ===" << endl;
    return 0;
}