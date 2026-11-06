#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
using namespace std;
const int MAX = 100;  // daugiausia prekiu irasu

string kategorijosVardas(int nr) {
    if (nr == 1) return "Dviraciai";
    if (nr == 2) return "Dalys";
    return "Aksesuarai";
}

// Kartoja klausima, kol ivedamas skaicius nuo..iki
int ivestiPasirinkima(string tekstas, int nuo, int iki) {
    int x;
    cout << tekstas;
    while (!(cin >> x) || x < nuo || x > iki) {
        if (cin.eof()) return iki;  // ivestis baigesi
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Neteisinga reiksme. Iveskite skaiciu nuo " << nuo << " iki " << iki << ": ";
    }
    return x;
}

// Nuskaito duomenis i masyvus. Grazina true, jei viskas pavyko.
bool nuskaityti(string failas, string kat[], string pav[], double kaina[], int kiekis[], int &n) {
    ifstream f(failas);
    if (!f.is_open()) { cout << "Klaida: nepavyko atidaryti failo " << failas << ".\n"; return false; }
    if (f.peek() == 0xEF) f.ignore(3);  // praleidziam galima UTF-8 zymekli (BOM)
    f >> n;
    if (f.fail() || n < 1 || n > MAX) {
        cout << "Klaida: pirmoje eiluteje turi buti irasu skaicius nuo 1 iki " << MAX << ".\n";
        return false;
    }
    for (int i = 0; i < n; i++) {
        getline(f >> ws, kat[i], ';');
        getline(f, pav[i], ';');
        f >> kaina[i];
        f.ignore();  // praleidziam ';'
        f >> kiekis[i];
        if (f.fail() || f.peek() == '.' || f.peek() == ';' || (kat[i] != "Dviraciai" && kat[i] != "Dalys" && kat[i] != "Aksesuarai") || kaina[i] <= 0 || kiekis[i] <= 0) {
            cout << "Klaida: neteisingi arba nepilni duomenys, " << i + 1 << " irasas.\n";
            return false;
        }
    }
    return true;
}

// Eilutes suma pries nuolaida: kaina x kiekis
double eilutesSuma(double kaina, int kiekis) { return kaina * kiekis; }

// 10 % nuolaida, jei kiekis >= 10, kitaip 0
double nuolaida(double suma, int kiekis) { return kiekis >= 10 ? suma * 0.10 : 0; }

// Kategorijos moketina suma po nuolaidu (0, jei tokiu prekiu nera)
double kategorijosSuma(string kat[], double kaina[], int kiekis[], int n, string kategorija) {
    double suma = 0;
    for (int i = 0; i < n; i++)
        if (kat[i] == kategorija) {
            double s = eilutesSuma(kaina[i], kiekis[i]);
            suma += s - nuolaida(s, kiekis[i]);
        }
    return suma;
}

// Spausdina prekiu lentele. kategorija "" = visos prekes. Grazina kiek eiluciu isspausdino.
int spausdintiLentele(ostream &out, string kat[], string pav[], double kaina[], int kiekis[], int n, string kategorija) {
    int kiek = 0;
    out << left << setw(12) << "Kategorija" << setw(32) << "Pavadinimas" << right << setw(9) << "Kaina" << setw(8) << "Kiekis"
        << setw(11) << "Suma" << setw(11) << "Nuolaida" << setw(11) << "Moketina" << "\n";
    for (int i = 0; i < n; i++) {
        if (kategorija != "" && kat[i] != kategorija) continue;
        double s = eilutesSuma(kaina[i], kiekis[i]), nl = nuolaida(s, kiekis[i]);
        out << left << setw(12) << kat[i] << setw(32) << pav[i] << right << setw(9) << kaina[i] << setw(8) << kiekis[i]
            << setw(11) << s << setw(11) << nl << setw(11) << s - nl << "\n";
        kiek++;
    }
    return kiek;
}

void spausdintiViso(ostream &out, string kat[], double kaina[], int kiekis[], int n) {
    double prie = 0, po = 0;
    for (int i = 0; i < n; i++) prie += eilutesSuma(kaina[i], kiekis[i]);
    for (int k = 1; k <= 3; k++) po += kategorijosSuma(kat, kaina, kiekis, n, kategorijosVardas(k));
    out << "Suma pries nuolaidas: " << prie << " EUR\n";
    out << "Bendra nuolaida:      " << prie - po << " EUR\n";
    out << "Moketina suma:        " << po << " EUR\n";
}

void spausdintiKategorijas(ostream &out, string kat[], double kaina[], int kiekis[], int n) {
    for (int k = 1; k <= 3; k++)
        out << left << setw(12) << kategorijosVardas(k) << right << setw(10)
            << kategorijosSuma(kat, kaina, kiekis, n, kategorijosVardas(k)) << " EUR\n";
}

// Ta pati funkcija veikia ir ekranui (cout), ir failui (ofstream)
void spausdintiAtaskaita(ostream &out, string kat[], string pav[], double kaina[], int kiekis[], int n) {
    out << "===== UZSAKYMO ATASKAITA =====\n\n";
    spausdintiLentele(out, kat, pav, kaina, kiekis, n, "");
    out << "\n";
    spausdintiViso(out, kat, kaina, kiekis, n);
    out << "\nKategoriju suvestine (moketina suma):\n";
    spausdintiKategorijas(out, kat, kaina, kiekis, n);
}

bool saugotiAtaskaita(string failas, string kat[], string pav[], double kaina[], int kiekis[], int n) {
    ofstream f(failas);  // ofstream senus failo duomenis perraso
    if (!f.is_open()) { cout << "Klaida: nepavyko atidaryti failo " << failas << " rasymui.\n"; return false; }
    f << fixed << setprecision(2);
    spausdintiAtaskaita(f, kat, pav, kaina, kiekis, n);
    f.close();
    if (f.fail()) cout << "Klaida: nepavyko irasyti duomenu i faila.\n";
    return !f.fail();
}

int main() {
    cout << fixed << setprecision(2);
    string kat[MAX], pav[MAX];
    double kaina[MAX];
    int kiekis[MAX], n = 0, pasirinkimas = 0;
    if (!nuskaityti("uzsakymas.txt", kat, pav, kaina, kiekis, n)) return 1;
    while (pasirinkimas != 7) {
        cout << "\n===== DVIRACIU PARDUOTUVES UZSAKYMAS =====\n"
                "1. Perziureti visas uzsakymo prekes\n2. Perziureti pasirinktos kategorijos prekes\n"
                "3. Perziureti viso uzsakymo sumas\n4. Perziureti sumas pagal kategorijas\n"
                "5. Parodyti visa uzsakymo ataskaita\n6. Issaugoti visa ataskaita i faila\n7. Iseiti\n";
        pasirinkimas = ivestiPasirinkima("Jusu pasirinkimas: ", 1, 7);
        if (pasirinkimas == 1) spausdintiLentele(cout, kat, pav, kaina, kiekis, n, "");
        else if (pasirinkimas == 2) {
            cout << "1 - Dviraciai\n2 - Dalys\n3 - Aksesuarai\n";
            string k = kategorijosVardas(ivestiPasirinkima("Pasirinkite kategorija: ", 1, 3));
            if (spausdintiLentele(cout, kat, pav, kaina, kiekis, n, k) == 0)
                cout << "Kategorijoje " << k << " prekiu uzsakyme nera.\n";
            cout << k << " moketina suma: " << kategorijosSuma(kat, kaina, kiekis, n, k) << " EUR\n";
        }
        else if (pasirinkimas == 3) spausdintiViso(cout, kat, kaina, kiekis, n);
        else if (pasirinkimas == 4) spausdintiKategorijas(cout, kat, kaina, kiekis, n);
        else if (pasirinkimas == 5) spausdintiAtaskaita(cout, kat, pav, kaina, kiekis, n);
        else if (pasirinkimas == 6) {
            if (saugotiAtaskaita("ataskaita.txt", kat, pav, kaina, kiekis, n))
                cout << "Ataskaita issaugota faile ataskaita.txt.\n";
        }
        else cout << "Viso gero!\n";
    }
    return 0;
}