
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <array>
#include <limits>
#include <algorithm>

using namespace std;

// ============================================================
//  STRUKTUR DATA BARANG
// ============================================================
struct Barang {
    int    id;
    string nama;
    string kategori;
    int    stok;
    double harga;
};

// ============================================================
//  KELAS UTAMA : TokoElektronik
// ============================================================
class TokoElektronik {
private:
    // Etalase dengan kapasitas 3 elemen (prinsip enkapsulasi)
    array<string, 3> etalase;

public:
    // Constructor – mengisi data awal etalase secara otomatis
    TokoElektronik() {
        etalase[0] = "Laptop ASUS VivoBook 14 (Rak 1)";
        etalase[1] = "Smartphone Samsung Galaxy A55 (Rak 2)";
        etalase[2] = "Smart TV LG 43 Inch 4K (Rak 3)";
    }

    // Method pengambilan produk dari etalase
    // Menggunakan .at() agar out_of_range terdeteksi otomatis
    string ambilProduk(size_t nomorRak) {
        try {
            return etalase.at(nomorRak);
        }
        catch (const out_of_range&) {
            // Lempar ulang pesan error kustom
            throw string("Gagal Mengambil Barang : Rak nomor " +
                         to_string(nomorRak) +
                         " kosong atau tidak tersedia!");
        }
    }
};

// ============================================================
//  KONSTANTA FILE GUDANG
// ============================================================
const string FILE_GUDANG = "gudang.txt";

// ============================================================
//  UTILITAS : tampilan garis pembatas
// ============================================================
void garisPembatas(char karakter = '=', int lebar = 60) {
    cout << string(lebar, karakter) << "\n";
}

void headerMenu(const string& judul) {
    garisPembatas();
    cout << "  " << judul << "\n";
    garisPembatas();
}

// ============================================================
//  UTILITAS : parse satu baris teks menjadi struct Barang
//  Format : id|nama|kategori|stok|harga
// ============================================================
bool parseBaris(const string& baris, Barang& b) {
    istringstream ss(baris);
    string token;
    try {
        if (!getline(ss, token, '|')) return false; b.id       = stoi(token);
        if (!getline(ss, token, '|')) return false; b.nama     = token;
        if (!getline(ss, token, '|')) return false; b.kategori = token;
        if (!getline(ss, token, '|')) return false; b.stok     = stoi(token);
        if (!getline(ss, token, '|')) return false; b.harga    = stod(token);
        return true;
    }
    catch (...) { return false; }
}

// ============================================================
//  UTILITAS : serialisasi struct Barang ke string
// ============================================================
string serialisasiBarang(const Barang& b) {
    return to_string(b.id)       + "|" +
           b.nama                + "|" +
           b.kategori            + "|" +
           to_string(b.stok)     + "|" +
           to_string(b.harga);
}

// ============================================================
//  UTILITAS : baca semua barang dari file
// ============================================================
vector<Barang> bacaSemuaBarang() {
    vector<Barang> daftar;
    ifstream file(FILE_GUDANG);
    if (!file.is_open()) return daftar;

    string baris;
    while (getline(file, baris)) {
        if (baris.empty()) continue;
        Barang b;
        if (parseBaris(baris, b)) daftar.push_back(b);
    }
    file.close();
    return daftar;
}

// ============================================================
//  UTILITAS : tulis ulang seluruh data ke file
// ============================================================
void tulisSemuaBarang(const vector<Barang>& daftar) {
    ofstream file(FILE_GUDANG, ios::trunc);
    for (const auto& b : daftar)
        file << serialisasiBarang(b) << "\n";
    file.close();
}

// ============================================================
//  UTILITAS : format harga ke Rupiah
// ============================================================
string formatRupiah(double harga) {
    ostringstream oss;
    oss << fixed << setprecision(0) << harga;
    string s = oss.str();
    int n = (int)s.size();
    string hasil;
    int sisaDepan = n % 3;
    for (int i = 0; i < n; ++i) {
        if (i > 0 && (i - sisaDepan) % 3 == 0) hasil += '.';
        hasil += s[i];
    }
    return "Rp " + hasil;
}

// ============================================================
//  UTILITAS : tampilkan tabel daftar barang
// ============================================================
void tampilkanTabelBarang(const vector<Barang>& daftar) {
    if (daftar.empty()) {
        cout << "  [INFO] Gudang masih kosong. Belum ada data barang.\n";
        return;
    }
    cout << left
         << setw(5)  << "ID"
         << setw(25) << "Nama Barang"
         << setw(15) << "Kategori"
         << setw(8)  << "Stok"
         << setw(18) << "Harga"
         << "\n";
    garisPembatas('-');
    for (const auto& b : daftar) {
        cout << left
             << setw(5)  << b.id
             << setw(25) << b.nama
             << setw(15) << b.kategori
             << setw(8)  << b.stok
             << setw(18) << formatRupiah(b.harga)
             << "\n";
    }
}

// ============================================================
//  ID otomatis – ambil ID terbesar + 1
// ============================================================
int idBerikutnya(const vector<Barang>& daftar) {
    int maxId = 0;
    for (const auto& b : daftar) if (b.id > maxId) maxId = b.id;
    return maxId + 1;
}

// ============================================================
//  FITUR CREATE – Tambah barang baru
// ============================================================
void tambahBarang() {
    headerMenu("TAMBAH BARANG BARU (CREATE)");
    vector<Barang> daftar = bacaSemuaBarang();

    Barang baru;
    baru.id = idBerikutnya(daftar);

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "  Nama Barang    : "; getline(cin, baru.nama);
    cout << "  Kategori       : "; getline(cin, baru.kategori);
    cout << "  Stok           : "; cin >> baru.stok;
    cout << "  Harga (Rp)     : "; cin >> baru.harga;

    daftar.push_back(baru);
    tulisSemuaBarang(daftar);

    cout << "\n  [SUKSES] Barang berhasil ditambahkan dengan ID = " << baru.id << ".\n";
}

// ============================================================
//  FITUR READ – Tampilkan semua barang
// ============================================================
void tampilkanBarang() {
    headerMenu("DAFTAR BARANG GUDANG (READ)");
    vector<Barang> daftar = bacaSemuaBarang();
    tampilkanTabelBarang(daftar);
}

// ============================================================
//  FITUR UPDATE – Perbarui data barang berdasarkan ID
// ============================================================
void updateBarang() {
    headerMenu("PERBARUI DATA BARANG (UPDATE)");
    vector<Barang> daftar = bacaSemuaBarang();
    tampilkanTabelBarang(daftar);

    if (daftar.empty()) return;

    int targetId;
    cout << "\n  Masukkan ID barang yang akan diperbarui: ";
    cin >> targetId;

    bool ditemukan = false;
    for (auto& b : daftar) {
        if (b.id == targetId) {
            ditemukan = true;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  Nama baru      [" << b.nama     << "] : "; getline(cin, b.nama);
            cout << "  Kategori baru  [" << b.kategori << "] : "; getline(cin, b.kategori);
            cout << "  Stok baru      [" << b.stok     << "] : "; cin >> b.stok;
            cout << "  Harga baru     [" << b.harga    << "] : "; cin >> b.harga;
            break;
        }
    }

    if (ditemukan) {
        tulisSemuaBarang(daftar);
        cout << "\n  [SUKSES] Data barang dengan ID " << targetId << " berhasil diperbarui.\n";
    } else {
        cout << "\n  [ERROR] Barang dengan ID " << targetId << " tidak ditemukan.\n";
    }
}

// ============================================================
//  FITUR DELETE – Hapus barang berdasarkan ID
// ============================================================
void hapusBarang() {
    headerMenu("HAPUS DATA BARANG (DELETE)");
    vector<Barang> daftar = bacaSemuaBarang();
    tampilkanTabelBarang(daftar);

    if (daftar.empty()) return;

    int targetId;
    cout << "\n  Masukkan ID barang yang akan dihapus: ";
    cin >> targetId;

    size_t sebelum = daftar.size();
    daftar.erase(
        remove_if(daftar.begin(), daftar.end(),
                  [targetId](const Barang& b){ return b.id == targetId; }),
        daftar.end()
    );

    if (daftar.size() < sebelum) {
        tulisSemuaBarang(daftar);
        cout << "\n  [SUKSES] Barang dengan ID " << targetId << " berhasil dihapus.\n";
    } else {
        cout << "\n  [ERROR] Barang dengan ID " << targetId << " tidak ditemukan.\n";
    }
}

// ============================================================
//  SIMULASI ETALASE – Uji Exception Handling
// ============================================================
void simulasiEtalase() {
    headerMenu("SIMULASI ETALASE & EXCEPTION HANDLING");
    TokoElektronik toko;

    // ── Skenario 1 : Indeks 1 → SUKSES ─────────────────────
    cout << "\n  >>> Skenario 1: Ambil produk di rak indeks ke-1\n";
    try {
        string produk = toko.ambilProduk(1);
        cout << "  [SUKSES] Produk ditemukan : " << produk << "\n";
    }
    catch (const string& pesanError) {
        cout << "  [GAGAL] " << pesanError << "\n";
    }

    // ── Skenario 2 : Indeks 5 → GAGAL ──────────────────────
    cout << "\n  >>> Skenario 2: Ambil produk di rak indeks ke-5\n";
    try {
        string produk = toko.ambilProduk(5);
        cout << "  [SUKSES] Produk ditemukan : " << produk << "\n";
    }
    catch (const string& pesanError) {
        cout << "  [GAGAL] " << pesanError << "\n";
    }

    cout << "\n  [INFO] Program tetap berjalan aman setelah error tertangani.\n";
}

// ============================================================
//  MENU UTAMA
// ============================================================
void tampilkanMenu() {
    garisPembatas();
    cout << "      TOKO ELEKTRONIK \"GIBRAN JAYA\"\n";
    cout << "         Sistem Manajemen Digital\n";
    garisPembatas();
    cout << "  [1] Tampilkan Semua Barang (Read)\n";
    cout << "  [2] Tambah Barang Baru     (Create)\n";
    cout << "  [3] Perbarui Data Barang   (Update)\n";
    cout << "  [4] Hapus Data Barang      (Delete)\n";
    cout << "  [5] Simulasi Etalase       (Exception Handling)\n";
    cout << "  [0] Keluar\n";
    garisPembatas();
    cout << "  Pilih menu: ";
}

// ============================================================
//  INISIALISASI – buat file gudang dengan data sampel
//  jika file belum ada
// ============================================================
void inisialisasiFile() {
    ifstream cek(FILE_GUDANG);
    if (cek.good()) { cek.close(); return; }
    cek.close();

    // Tulis data awal ke file
    ofstream file(FILE_GUDANG);
    file << "1|Laptop ASUS VivoBook 14|Laptop|5|8500000\n";
    file << "2|Smartphone Samsung A55|Smartphone|12|5200000\n";
    file << "3|Smart TV LG 43 Inch 4K|TV|3|6750000\n";
    file << "4|Headphone Sony WH-1000XM5|Audio|8|4900000\n";
    file << "5|Printer Canon PIXMA G2020|Printer|6|1350000\n";
    file.close();

    cout << "  [INFO] File gudang.txt dibuat dengan data awal.\n";
}

// ============================================================
//  FUNGSI MAIN
// ============================================================
int main() {
    inisialisasiFile();

    int pilihan;
    do {
        tampilkanMenu();
        cin >> pilihan;

        // Validasi input non-integer
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\n  [ERROR] Input tidak valid. Masukkan angka.\n\n";
            continue;
        }

        cout << "\n";
        switch (pilihan) {
            case 1: tampilkanBarang(); break;
            case 2: tambahBarang();    break;
            case 3: updateBarang();    break;
            case 4: hapusBarang();     break;
            case 5: simulasiEtalase(); break;
            case 0:
                garisPembatas();
                cout << "  Terima kasih telah menggunakan sistem Gibran Jaya!\n";
                garisPembatas();
                break;
            default:
                cout << "  [ERROR] Pilihan tidak tersedia. Coba lagi.\n";
        }
        cout << "\n";

    } while (pilihan != 0);

    return 0;
}