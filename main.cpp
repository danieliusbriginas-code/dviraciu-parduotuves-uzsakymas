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

int main() {
    cout << fixed << setprecision(2);
    string kat[MAX], pav[MAX];
    double kaina[MAX];
    int kiekis[MAX], n = 0;
    if (!nuskaityti("uzsakymas.txt", kat, pav, kaina, kiekis, n)) return 1;
    spausdintiLentele(cout, kat, pav, kaina, kiekis, n, "");
    return 0;
}