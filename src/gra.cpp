#include <iostream>
#include <stdlib.h>
#include <memory>
#include <string>
#include "generator_imion.cpp"

#define N 3                     // okno sredniej przychodu przy obliczaniu wartosci firmy
#define M 2                     // maksymalna wartosc calkowitego zadluzenia jako krotnosc wartosci firmy 
#define X 12                    // maksymalny czas splaty kredytu
#define KWOTA_POCZATKOWA 100.   // stan konta na poczatku gry
#define KWOTA_DOCELOWA 1000000. // stan konta decydujacy o zwycieztwie 


class Pracownik {
    protected:
        std::string imie;
    public:
        Pracownik() {imie = Generator::losuj_imie();}
        // Ro5
};

class Inz: public Pracownik {
    std::string wydzial {"MEiL"};
    
    public:
        const static double CI;
        void print() const { std::cout << imie << " \tInzynier(ka) z wydzialu " << wydzial << "\n"; }
};

class Mkt: public Pracownik {
    int follows {42};

    public:
        const static double CMag;
        void print() const { std::cout << imie << " \tMarketer(ka) z " << follows << " followers-ami \n"; }
};

class Mag: public Pracownik {
    bool obsl_widl {true};

    public:
        const static double CMkt;
        void print() const { std::string obsl = (obsl_widl) ? "tak" : "nie"; std::cout << imie << " \tMagazynier(ka), " << "uprawnienia na wozek widlowy: " << obsl << "\n"; }
};

class Rob: public Pracownik {
    double but {43};

    public:
        const static double CR;
        void print() const { std::cout << imie << " \tRobotnik(czka) o rozmairze buta " << but << " \n"; }
};


class DrukarzPracownikow {
    public:
        void operator()(const auto& pracownik) const { pracownik.print(); }
};


class Kredyt {
    double dlug;
    int pozostale_raty;

    public:
        Kredyt(): dlug(0), pozostale_raty(0) {}
        Kredyt(double d, int t): dlug(d), pozostale_raty(t) {}
        // Ro5
        Kredyt(const Kredyt&) = default;
        Kredyt& operator=(const Kredyt&) = default;
        Kredyt& operator=(Kredyt&&) = default;
        ~Kredyt() = default;

        double nastepna_rata() const { 
            if(pozostale_raty <= 0) { return 0.; } 
            return dlug / (double)pozostale_raty;
        }

        double splac_rate() { 
            double rata = nastepna_rata();
            dlug -= rata;
            pozostale_raty--;
            return rata;
        }
};


class Firma {
    double stan_konta {KWOTA_POCZATKOWA};

    int n_kredytow {0};
    std::unique_ptr<Kredyt[]> kredyty = std::make_unique<Kredyt[]>(100);
    double calkowite_zadluzenie {0.};

    int n_prac {0};
    std::unique_ptr<std::variant<Inz, Mkt, Mag, Rob>[]> prac = std::make_unique<std::variant<Inz, Mkt, Mag, Rob>[]>(100);
    int n_role[] = {0, 0, 0, 0};
    
    int n_miesiac {0};
    std::unique_ptr<double[]> historia_przych = std::make_unique<double[]>(100);

    double nastepna_rata() const {
        double nr = 0.;
        for (int i = 0; i < n_kredytow; i++) {
            nr += kredyty[i].nastepna_rata();
        }
        return nr;
    }

    double oblicz_przychod() const { 
        
    }

    public:
        Firma() {
            zatrudnij(*std::make_unique<Inz>());
            zatrudnij(*std::make_unique<Mkt>());
            zatrudnij(*std::make_unique<Mag>());
            zatrudnij(*std::make_unique<Rob>());
        }
        // Ro5
        Firma(const Firma&) = default;
        Firma& operator=(const Firma& f) = default;
        Firma& operator=(Firma&&) = default;
        ~Firma() = default;

        // akcje gracza
        void wez_kredyt(const double& kwota, const int& czas_splaty) { 
            if (czas_splaty > X) { return; }
            if (calkowite_zadluzenie + kwota > M * wartosc_firmy()) { return; }
            
            stan_konta += kwota;

            const double odsetki = kwota * 0.01 * czas_splaty;
            const double wartosc_kredytu = kwota + odsetki;
            calkowite_zadluzenie += wartosc_kredytu;
            kredyty[n_kredytow] = *std::make_unique<Kredyt>(wartosc_kredytu, czas_splaty);
            n_kredytow++;
        }

        template <typename pracownik_t>
        void zatrudnij(const pracownik_t& pracownik) { prac[n_prac] = pracownik; n_prac++; }

        // akcje automatyczne
        void otrzymaj_przychod() { 
            double przychod = oblicz_przychod();
            historia_przych[n_miesiac] = przychod;
            n_miesiac++;
            stan_konta += przychod;
        }

        void zaplac_wynagrodzenie() {  }
        void splac_raty() {  }

        // funkcje pomocnicze
        double get_stan_konta() const { return stan_konta; }

        double wartosc_firmy() const {
            double suma = 0.;
            if (n_miesiac > N) {
                for (int i = 0; i < N; i++) { suma += historia_przych[i]; }
            }
            else {
                for (int i = n_miesiac - N; i < n_miesiac; i++) { suma += historia_przych[i]; }
            }
            return suma / (double)N;
        }

        void drukuj_pracownikow() const { 
            std::cout << "Lista pracownikow: \n";
            for(int i = 0; i < n_prac; i++) {
                std::cout << i+1 << ". "; 
                std::visit(DrukarzPracownikow{}, prac[i]); 
            } 
            std::cout << "\n";
        }

        void drukuj_stan() const { 
            std::cout << "\t\t   MIESIAC " << n_miesiac  << "\n";
            std::cout << " | Stan konta: " << stan_konta;
            std::cout << "\t| Liczba pracownikow:" << n_prac << " \t| \n";
            std::cout << " | Zadluzenie: " << calkowite_zadluzenie;
            std::cout << "\t| Nastepna rata: " << nastepna_rata() << " \t| \n\n";
        } 

        void drukuj_rozliczenie() const {  }
};


class Gra {
    bool stan { true };
    Firma firma;

    void tick() {
        firma.drukuj_rozliczenie();
        firma.otrzymaj_przychod();
        firma.zaplac_wynagrodzenie();
        firma.splac_raty();
        // wyswietl stan gry na kolejny miesiac
        // sprawdz wygrana, przegrana, zaktualizuj stan gry
        if (firma.get_stan_konta() > KWOTA_DOCELOWA) {    //wygrana 
            // powiadom o wygranej
            stan = false;
            return;
        }
        if (firma.get_stan_konta() < 0.) {                //przegrana
            // powiadom o przegranej
            stan = false;
            return;
        }
        firma.drukuj_stan();
    }

    void przyjmij_dane_do_kredytu() { 
        double kwota;
        int czas_splaty;
        // przyjmij dane do kredytu 
        firma.wez_kredyt(kwota, czas_splaty);
    }

    public:
        void akcja_gracza() {
            // wyswietl instrukcje
            std::cout << "Wybierz akcje, (h) pomoc: \n> ";

            // przyjmij polecenie
            std::string polecenie;
            std::cin >> polecenie;
            std::cout << "\n";

            if      (polecenie == "lp")   { firma.drukuj_pracownikow();                }
            else if (polecenie == "zinz") { firma.zatrudnij(*std::make_unique<Inz>()); }
            else if (polecenie == "zmag") { firma.zatrudnij(*std::make_unique<Mag>()); }
            else if (polecenie == "zmkt") { firma.zatrudnij(*std::make_unique<Mkt>()); }
            else if (polecenie == "zrob") { firma.zatrudnij(*std::make_unique<Rob>()); }
            else if (polecenie == "kred") { przyjmij_dane_do_kredytu();                }
            else if (polecenie == "kt")   { tick();                                    }
            else if (polecenie == "h")    { drukuj_instrukcje();                       }
            else                          { drukuj_dostepne_polecenia();               }
        }

        bool get_stan() { return stan; }

        void drukuj_dostepne_polecenia() { 
            std::cout << "Dostepne polecenia: \n"; 
            std::cout << "\tlp\t- pokaz liste pracownikow \n"; 
            std::cout << "\tzinz, zmag, zmkt, zrob\t- zatrudnij pracownika \n"; 
            std::cout << "\tkred\t- wez kredyt \n"; 
            std::cout << "\tkt\t- zakoncz ture \n"; 
            std::cout << "\th\t- wyswietl instrukcje \n"; 
            std::cout << "\n";
        }

        void drukuj_instrukcje() {
            // drukuj zasady gry
            drukuj_dostepne_polecenia();
        }

        void drukuj_powitanie() {
            std::cout << "Witaj! \n";
            firma.drukuj_stan();
        }
};
