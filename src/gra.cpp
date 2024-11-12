#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <memory>
#include <string>
#include "generator_imion.cpp"

#define N 3                     // okno sredniej przychodu przy obliczaniu wartosci firmy
#define M 2                     // maksymalna wartosc calkowitego zadluzenia jako krotnosc wartosci firmy 
#define X 12                    // maksymalny czas splaty kredytu
#define KWOTA_POCZATKOWA 10000. // stan konta na poczatku gry
#define KWOTA_DOCELOWA 1000000. // stan konta decydujacy o zwycieztwie 
#define WYNAGRODZENIE 520.      // wynagrodzenie kazdego pracownika
#define WARTOSC_BAZOWA 50.      // wartosc poczatkowa produktu ($)


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
        constexpr static double CI {1.1};
        void print() const { std::cout << imie << " \tInzynier(ka) z wydzialu " << wydzial << "\n"; }
};

class Mkt: public Pracownik {
    int follows {42};

    public:
        constexpr static double CMkt {15};
        void print() const { std::cout << imie << " \tMarketer(ka) z " << follows << " followers-ami \n"; }
};

class Mag: public Pracownik {
    bool obsl_widl {true};

    public:
        constexpr static double CMag {20};
        void print() const { std::string obsl = (obsl_widl) ? "tak" : "nie"; std::cout << imie << " \tMagazynier(ka), " << "uprawnienia na wozek widlowy: " << obsl << "\n"; }
};

class Rob: public Pracownik {
    double but {43};

    public:
        constexpr static double CR {10};
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
        int produkcja = 0;
        int pojemnosc_mag = 0;
        int popyt = 0;
        double wartosc = WARTOSC_BAZOWA;

        for(int i = 0; i < n_prac; i++) {
            switch(prac[i].index()) {
                case 0:
                    wartosc *= Inz::CI;
                case 1:
                    popyt += Mkt::CMkt;
                case 2:
                    pojemnosc_mag += Mag::CMag;
                case 3:
                    produkcja += Rob::CR;
            }
        } 

        int stan_magazynu = std::min(produkcja, pojemnosc_mag);
        int sprzedaz = std::min(stan_magazynu, popyt);
        double cena = wartosc;
        double przychod = cena * (double)sprzedaz;
        return przychod;
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
        int wez_kredyt(const double& kwota, const int& czas_splaty) { 
            if (czas_splaty > X) { return 1; }
            if (calkowite_zadluzenie + kwota > M * wartosc_firmy()) { return 2; }
            
            stan_konta += kwota;

            const double odsetki = kwota * 0.01 * czas_splaty;
            const double wartosc_kredytu = kwota + odsetki;
            calkowite_zadluzenie += wartosc_kredytu;
            kredyty[n_kredytow] = *std::make_unique<Kredyt>(wartosc_kredytu, czas_splaty);
            n_kredytow++;
            return 0;
        }

        template <typename pracownik_t>
        void zatrudnij(const pracownik_t& pracownik) { prac[n_prac] = pracownik; n_prac++; }

        // akcje automatyczne
        double otrzymaj_przychod() { 
            double przychod = oblicz_przychod();
            historia_przych[n_miesiac] = przychod;
            n_miesiac++;
            stan_konta += przychod;
            return przychod;
        }

        double zaplac_wynagrodzenie() { 
            double sum_w = WYNAGRODZENIE * (double)n_prac; 
            stan_konta -= sum_w;
            return sum_w;
        }
        
        double splac_raty() { 
            double sum_r = 0.;
            for(int i = 0; i < n_kredytow; i++) {
                sum_r += kredyty[i].splac_rate();
            }
            stan_konta -= sum_r;
            calkowite_zadluzenie -= sum_r;
            return sum_r;
        }

        // funkcje pomocnicze
        double get_stan_konta()           const { return stan_konta; }
        double get_calkowite_zadluzenie() const { return calkowite_zadluzenie; }

        double wartosc_firmy() const {
            double suma = 0.;
            if (n_miesiac == 0) { return 1000.; }
            else if (n_miesiac > N) {
                for (int i = 0; i < N; i++) { suma += historia_przych[i]; }
            }
            else {
                for (int i = n_miesiac - N; i < n_miesiac; i++) { suma += historia_przych[i]; }
            }
            return suma / (double)N;
        }

        // informatory
        void drukuj_pracownikow() const { 
            std::cout << "Lista pracownikow: \n";
            for(int i = 0; i < n_prac; i++) {
                std::cout << " " << i+1 << ". "; 
                std::visit(DrukarzPracownikow{}, prac[i]); 
            } 
            std::cout << "\n";
        }

        void drukuj_stan() const { 
            std::cout << std::setw(26) << "MIESIAC" << std::setw(4) << n_miesiac  << "\n";
            std::cout << " | Stan konta: " << stan_konta << "$";
            std::cout << " | Liczba pracownikow: " << n_prac << "\t| \n";
            std::cout << " | Zadluzenie: " << calkowite_zadluzenie << "$";
            std::cout << " | Nastepna rata: " << nastepna_rata() << "$ \t| \n\n";
        } 

        void rozlicz() { 
            double przychod = otrzymaj_przychod();
            double wyplaty = zaplac_wynagrodzenie();
            double raty = splac_raty();
            double dochod = przychod - wyplaty - raty;
            std::string sym = (dochod > 0.) ? "+" : "";

            std::cout << " |" << std::setw(23) << "Przychod: " << przychod << "$   \t\t|\n";
            std::cout << " |" << std::setw(23) << "Wynagrodzenia: " << wyplaty << "$   \t\t|\n";
            std::cout << " |" << std::setw(23) << "Raty kredytow: " << raty << "$\t\t\t|\n";
            std::cout << " |" << std::setw(23) << "Dochod: " << sym << dochod << "$   \t\t|\n\n";
        }
};


class Gra {
    bool stan { true };
    Firma firma;

    void tick() {
        firma.rozlicz();
        firma.drukuj_stan();
        // sprawdz wygrana, przegrana, zaktualizuj stan gry
        if (firma.get_stan_konta() > KWOTA_DOCELOWA) {    //wygrana 
            std::cout << "\t WYGRYWASZ!";
            stan = false;
            return;
        }
        if (firma.get_stan_konta() < 0.) {                //przegrana
            std::cout << "\t GAME OVER";
            stan = false;
            return;
        }
    }

    void przyjmij_dane_do_kredytu() { 
        double kwota;
        int czas_splaty;
        // przyjmij dane do kredytu 
        std::cout << "Kwota kredytu ($): \n> ";
        std::cin  >> kwota;
        std::cout << "Czas splaty (miesiace): \n> ";
        std::cin  >> czas_splaty;
        std::cout << "\n";
        switch(firma.wez_kredyt(kwota, czas_splaty)) {
            case 0: 
                return;
            case 1:
                std::cout << "Zbyt dlugi czas splaty! \n\n";
                break;
            case 2:
                std::cout << "Zbyt duza kwota! Zdolnosc kredytowa: " << M * firma.wartosc_firmy() - firma.get_calkowite_zadluzenie() << "$ \n\n";
        }
    }

    public:
        void akcja_gracza() {
            // wyswietl instrukcje
            std::cout << "Wybierz akcje, (h)pomoc: \n> ";

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
            std::cout << "\tDostepne polecenia: \n"; 
            std::cout << "\t lp\t- pokaz liste pracownikow \n"; 
            std::cout << "\t zinz, zmag, zmkt, zrob\t- zatrudnij pracownika \n"; 
            std::cout << "\t kred\t- wez kredyt \n"; 
            std::cout << "\t kt\t- zakoncz ture \n"; 
            std::cout << "\t h\t- wyswietl instrukcje \n"; 
            std::cout << "\n";
        }

        void drukuj_instrukcje() {
            drukuj_dostepne_polecenia();
            std::cout << "\tCelem gry jest zgromadzenie " << KWOTA_DOCELOWA << "$.\n";
            std::cout << "\t W kazdej turze mozesz wykonac dowolna ilosc polecen.\n";
            std::cout << "\t Pracownicy zwiekszaja mozliwosci twojej firmy: \n";
            std::cout << "\t  * Inzynier(ka) zwieksza wartosc i cene produktu \n";
            std::cout << "\t  * Robotnik(czka) zwieksza liczbe produkowanych co miesiac jednostek \n";
            std::cout << "\t  * Magazynier(ka) zwieksza pojemnosc magazynow \n";
            std::cout << "\t  * Marketer(ka) zwieksza popyt na twoj produkt. \n";
            std::cout << "\t Po wykonaniu polecen, zakoncz ture wpisujac (kt), otrzymasz rozliczenie za ten miesiac.\n";
            std::cout << "\n";
        }

        void drukuj_powitanie() {
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "\t  Witaj! \n";
            firma.drukuj_stan();
        }
};
