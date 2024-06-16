///Standardowe operacje wejścia/wyjścia
#include <iostream>
///Dynamiczne tablice do przechowywania planszy
#include <vector>
///Przechowywanie ostatnich wyników z pliku (deque<string>)
#include <deque>
///Operacje na plikach (czytanie i zapisywanie wyników)
#include <fstream>
///Operacje na łańcuchach znaków(std::string itp.)
#include <string>
///Funkcje standardowej biblioteki, np. rand() i srand()
#include <cstdlib>
///Inicjalizacja generatora losowego srand(time(NULL))
#include <ctime>
///Podstawowe funkcje Allegro 5.
#include <allegro5/allegro.h>
///Rysowanie prymitywów (linii do siatki, była używana wcześniej)
#include <allegro5/allegro_primitives.h>
///Podstawowe funkcje czcionek Allegro 5
#include <allegro5/allegro_font.h>
///Wczytywanie czcionek TrueType
#include <allegro5/allegro_ttf.h>
///Pokazywanie natywnych okien dialogowych (komunikaty o błędach)
#include <allegro5/allegro_native_dialog.h>
///Ładowanie i operacje na obrazach
#include <allegro5/allegro_image.h>

using namespace std;

const int szerokosc = 15;
const int wysokosc = 15;
const int szerokosc_s = 3;
const int wysokosc_s = 3;
const int rozmiar = 60;
const int rozmiar2 = 135;
const int xoffset = 41;

bool koniecGry = false;
ALLEGRO_EVENT_QUEUE* queue = NULL;
ALLEGRO_DISPLAY* display = NULL;
ALLEGRO_BITMAP* bitmap;
ALLEGRO_BITMAP* cross;
ALLEGRO_BITMAP* circle;
ALLEGRO_BITMAP* gameOverImage;
ALLEGRO_FONT* font = NULL;
ALLEGRO_TIMER* timer = NULL;
double elapsedTime = 0;  // Czas upłynął w sekundach
bool startTimer = false;  // Flaga wskazująca, czy stoper ma być uruchomiony


class Kolizja
{
public:
    Kolizja()
    {
        x = 0;
        y = 0;
        size = 0;
    }
    Kolizja(int _x, int _y, int _size) : x(_x), y(_y), size(_size) {}
    bool isMouseInside(int mouseX, int mouseY);
private:
    int x;
    int y;
    int size;

};

bool Kolizja::isMouseInside(int mouseX, int mouseY)
{
    if (mouseX < x) return false;
    if (mouseX > (x + size)) return false;
    if (mouseY < y) return false;
    if (mouseY > (y + size)) return false;

    return true;
}

struct Pole {
    char wartosc;
    Pole() : wartosc(' ') {}
};

void inicjalizuj(vector<Pole>& plansza, vector<Kolizja>& kolizje) {
    plansza.assign(9, Pole());

    // Inicjalizacja timera
    timer = al_create_timer(1.0);  // Ustawia timer na 60 FPS


    // Tworzenie kolejki zdarzeń
    queue = al_create_event_queue();
    if (!queue) {
        al_show_native_message_box(NULL, NULL, NULL, "Failed to create event queue!", NULL, NULL);
        al_destroy_display(display);
        return;
    }
    if (!al_init_font_addon()) {
        cout << "Could not init the font addon.\n";
    }
    if (!al_init_ttf_addon()) {
        cout << "Could not init the ttf addon.\n";
    }
    font = al_load_ttf_font("C:\\Windows\\Fonts\\Arial.ttf", 100, 0);
    if (!font) {
        cout << "Failed to load font!" << endl;
        return;
    }

    // Rejestracja źródeł zdarzeń w kolejce
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    // Inicjalizacja timera (60 klatek na sekundę)
  //  timer = al_create_timer(1.0 / 60);
//    if (!timer) {
//        cerr << "Failed to create timer!" << endl;
//         return;
 //    }
    al_register_event_source(queue, al_get_timer_event_source(timer));

    // Inicjalizacja planszy i kolizji
    int planszaX = (al_get_display_width(display) - szerokosc_s * rozmiar2) / 2 - xoffset;
    int planszaY = (al_get_display_height(display) - wysokosc_s * rozmiar2) / 2;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            kolizje.push_back(Kolizja(planszaX + j * rozmiar2, planszaY + i * rozmiar2, rozmiar2));
        }
    }

    // Rozpoczęcie timera
    al_start_timer(timer);
    al_get_timer_count(timer);
}

void wyswietl(vector<Pole>& plansza) {
    al_clear_to_color(al_map_rgb(0, 0, 0));  // Wyczyść ekran, ustawiając kolor tła na czarny
    ALLEGRO_COLOR color2 = al_map_rgb(255, 255, 255);  // Ustaw kolor na biały

    // Oblicz współrzędne początkowe dla planszy, aby była na środku ekranu
    int planszaX = (al_get_display_width(display) - szerokosc_s * rozmiar2) / 2;
    int planszaY = (al_get_display_height(display) - wysokosc_s * rozmiar2) / 2;

    // Rysowanie siatki
    /*for (int i = 0; i < szerokosc_s + 1; i++) {
        al_draw_line(planszaX + i * rozmiar2, planszaY, planszaX + i * rozmiar2, planszaY + wysokosc_s * rozmiar2, color2, 3);
    }
    for (int i = 0; i < wysokosc_s + 1; i++) {
        al_draw_line(planszaX, planszaY + i * rozmiar2, planszaX + szerokosc_s * rozmiar2, planszaY + i * rozmiar2, color2, 3);
    }*/


    al_draw_bitmap(bitmap, 0, 0, 0);



    // Rysowanie znaków na planszy
    for (int i = 0; i < 9; i++)
    {
        int x = planszaX + (i % 3) * rozmiar2 + rozmiar2 / 2;
        int y = planszaY + (i / 3) * rozmiar2 + rozmiar2 / 2;

        if (plansza[i].wartosc == 'X')
        {
            //70 to offset w pikselach, moze sie zmieniac w zaleznosci od rozmiaru zdjecia, aktualna tekstura miała rozmiar 140px x 140px, ale już nie ma
            al_draw_bitmap(cross, x - 112, y - 60, 0);
        }
        else if (plansza[i].wartosc == 'O')
        {
            al_draw_bitmap(circle, x - 112, y - 60, 0);
        }
    }

    // Rysowanie stopera
    int stoperX = al_get_display_width(display) - 250;
    int stoperY = al_get_display_height(display) - 100;
    al_draw_textf(font, al_map_rgb(255, 255, 255), stoperX, stoperY, ALLEGRO_ALIGN_RIGHT, "Czas: %.0f s", elapsedTime);

    /*
    for (int i = 0; i < 9; ++i) {
        int x = planszaX + (i % 3) * rozmiar2 + rozmiar2 / 2;
        int y = planszaY + (i / 3) * rozmiar2 + rozmiar2 / 2;
        al_draw_textf(font, al_map_rgb(255, 255, 255), x, y - 55, ALLEGRO_ALIGN_CENTER, "%c", plansza[i].wartosc);
    }
    */


    al_flip_display();
    //al_destroy_font(font);
}



bool planszaPelna(const vector<Pole>& plansza) {
    for (int i = 0; i < 9; ++i) {
        if (plansza[i].wartosc == ' ') {
            return false;
        }
    }
    return true;
}

bool sprawdzWygrana(char gracz, const vector<Pole>& plansza) {
    for (int i = 0; i < 9; i += 3) {
        if (plansza[i].wartosc == gracz && plansza[i + 1].wartosc == gracz && plansza[i + 2].wartosc == gracz) {
            return true;
        }
    }
    for (int i = 0; i < 3; ++i) {
        if (plansza[i].wartosc == gracz && plansza[i + 3].wartosc == gracz && plansza[i + 6].wartosc == gracz) {
            return true;
        }
    }
    if ((plansza[0].wartosc == gracz && plansza[4].wartosc == gracz && plansza[8].wartosc == gracz) ||
        (plansza[2].wartosc == gracz && plansza[4].wartosc == gracz && plansza[6].wartosc == gracz)) {
        return true;
    }
    return false;
}

int sztucznaInteligencja(char gracz, vector<Pole>& plansza, bool OptymalnyRuch) {
    if (sprawdzWygrana('X', plansza)) return -1;
    if (sprawdzWygrana('O', plansza)) return 1;
    if (planszaPelna(plansza)) return 0;

    int najlepszyRuch = -1;
    int najlepszaWartosc = (gracz == 'O') ? -10 - 1 : 10;

    for (int i = 0; i < 9; ++i) {
        if (plansza[i].wartosc == ' ') {
            plansza[i].wartosc = gracz;
            int wartoscRuchu = sztucznaInteligencja((gracz == 'X') ? 'O' : 'X', plansza, OptymalnyRuch);
            if ((gracz == 'O' && wartoscRuchu > najlepszaWartosc) || (gracz == 'X' && wartoscRuchu < najlepszaWartosc)) {
                najlepszaWartosc = wartoscRuchu;
                najlepszyRuch = i;
            }
            plansza[i].wartosc = ' ';
        }
    }
    if (najlepszyRuch == -1) return 0;
    return (gracz == 'O') ? najlepszaWartosc : najlepszaWartosc;
}


void ruchKomputera(vector<Pole>& plansza) {
    int najlepszyRuch = -1;
    int najlepszaWartosc = -10 - 1;

    bool OptymalnyRuch = (rand() % 3 != 0); // 2/3 szansy na optymalny ruch

    for (int i = 0; i < 9; ++i) {
        if (plansza[i].wartosc == ' ') {
            plansza[i].wartosc = 'O';
            int wartoscRuchu = sztucznaInteligencja('X', plansza, OptymalnyRuch);
            plansza[i].wartosc = ' ';

            if (wartoscRuchu > najlepszaWartosc) {
                najlepszaWartosc = wartoscRuchu;
                najlepszyRuch = i;
            }
        }
    }

    if (!OptymalnyRuch && najlepszyRuch != -1) {
        // Wybiera losowo zamiast tego, by od razu podjął najlepszy ruch
        vector<int> MozliweRuchy;
        for (int i = 0; i < 9; ++i) {
            if (plansza[i].wartosc == ' ') {
                MozliweRuchy.push_back(i);
            }
        }
        najlepszyRuch = MozliweRuchy[rand() % MozliweRuchy.size()];
    }

    plansza[najlepszyRuch].wartosc = 'O';
}

void sprawdzKolizje(vector<Kolizja> kolizje, vector<Pole>& plansza, int mouseX, int mouseY)
{
    for (int i = 0; i < kolizje.size(); i++)
    {
        if (kolizje[i].isMouseInside(mouseX, mouseY))
        {
            if (plansza[i].wartosc == ' ') {
                plansza[i].wartosc = 'X';
                if (sprawdzWygrana('X', plansza)) {
                    wyswietl(plansza);
                    //cout << "\nWygrywa X!" << endl;
                    koniecGry = true;
                }
                else if (!planszaPelna(plansza)) {
                    ruchKomputera(plansza);
                    if (sprawdzWygrana('O', plansza)) {
                        wyswietl(plansza);
                        // cout << "\nWygrywa O!" << endl;
                        koniecGry = true;
                    }
                }
                else {
                    //cout << "\nRemis!" << endl;
                    koniecGry = true;
                }
            }
            break;
        }
    }
}

bool okno() {
    if (!al_init()) {
        al_show_native_message_box(NULL, NULL, NULL, "Failed to initialize Allegro 5", NULL, NULL);
        return false;
    }
    if (!al_init_native_dialog_addon()) {
        cout << "Could not init the native dialog addon.\n";
        return false;
    }
    if (!al_init_primitives_addon()) {
        cout << "Could not init the primitives addon.\n";
        return false;
    }
    if (!al_init_image_addon())
    {
        cout << "Could not init the image addon.\n";
        return false;
    }
    if (!al_install_keyboard()) {
        al_show_native_message_box(NULL, NULL, NULL, "Failed to initialize the keyboard!", NULL, NULL);
        return false;
    }
    if (!al_install_mouse()) {
        al_show_native_message_box(NULL, NULL, NULL, "Failed to initialize the mouse!", NULL, NULL);
        return false;
    }
    display = al_create_display(szerokosc * rozmiar, wysokosc * rozmiar);
    if (!display) {
        al_show_native_message_box(NULL, NULL, NULL, "Failed to create Allegro 5 display", NULL, NULL);
        return false;
    }
    queue = al_create_event_queue();
    if (!queue) {
        al_show_native_message_box(NULL, NULL, NULL, "Failed to create event queue!", NULL, NULL);
        al_destroy_display(display);
        return false;
    }
    // Inicjalizacja stopera
    //timer = al_create_timer(1.0);  // Timer z interwałem 1 sekundy
   // if (!timer) {
   //     al_show_native_message_box(NULL, NULL, NULL, "Failed to create timer!", NULL, NULL);
   //     al_destroy_display(display);
  //      return false;
  //  }
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    return true;
}

void zapiszWynik(const string& wynik, const string& nick, const double& czas) {
    ofstream file("wyniki.txt", ios::app);
    if (file.is_open()) {
        file << nick << ": " << wynik << ": " << czas << " s" << endl;
        file.close();
    }
    else {
        cout << "Nie można otworzyć pliku do zapisu!" << endl;
    }
}

deque<string> czytajOstatnieWyniki(const string& filePath, int n) {
    deque<string> lines;
    ifstream file(filePath);
    string line;
    if (file.is_open()) {
        while (getline(file, line)) {
            if (lines.size() == n) {
                lines.pop_front();
            }
            lines.push_back(line);
        }
        file.close();
    }
    else {
        cerr << "Nie można otworzyć pliku do odczytu!" << endl;
    }
    return lines;
}

void wyswietlWyniki(const deque<string>& wyniki)
{

   //al_clear_to_color(al_map_rgb(0, 0, 0));
   //al_draw_bitmap(gameOverImage, 0, 0, 0);
    ALLEGRO_DISPLAY* resultsDisplay = al_create_display(400, 400);
    if (!resultsDisplay) {
        al_show_native_message_box(NULL, NULL, NULL, "Nie udało się utworzyć wyświetlacza wyników", NULL, NULL);
        return;
    }

    //al_clear_to_color(al_map_rgb(0, 0, 0));

    if (!al_init_font_addon() || !al_init_ttf_addon()) {
        cout << "Nie udało się zainicjować dodatków fontów lub ttf." << endl;
        return;
    }

    ALLEGRO_FONT* font = al_load_ttf_font("C:\\Windows\\Fonts\\Arial.ttf", 16, 0);
    if (!font) {
        cout << "Nie udało się załadować fontu!" << endl;
        return;
    }

    int y = 50;
    for (const auto& wynik : wyniki) {
        al_draw_text(font, al_map_rgb(255, 255, 255), 200, y, ALLEGRO_ALIGN_CENTER, wynik.c_str());
        y += 30;
    }

    al_flip_display();
    al_rest(5.0);

    al_destroy_font(font);
    al_destroy_display(resultsDisplay);
}

string getNickname() {
    string nickname;
    ALLEGRO_EVENT event;
    ALLEGRO_FONT* font = al_load_ttf_font("C:\\Windows\\Fonts\\Arial.ttf", 32, 0);

    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_text(font, al_map_rgb(255, 255, 255), 400, 300, ALLEGRO_ALIGN_CENTER, "Enter your nickname:");
    al_flip_display();

    while (true) {
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
            if (event.keyboard.unichar == '\r') { // Enter key
                break;
            }
            else if (event.keyboard.unichar == '\b') { // Backspace key
                if (!nickname.empty()) {
                    nickname.pop_back();
                }
            }
            else {
                nickname += event.keyboard.unichar;
            }
        }

        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_bitmap(gameOverImage, 0, 0, 0);
        //al_draw_text(font, al_map_rgb(160, 82, 45), 450, 300, ALLEGRO_ALIGN_CENTER, "Wprowadz swoj pseudonim:");
        al_draw_text(font, al_map_rgb(160, 82, 45), 450, 325, ALLEGRO_ALIGN_CENTER, nickname.c_str());
        al_flip_display();
    }

    al_destroy_font(font);
    return nickname;
}

int main() {
    srand(time(NULL));
    if (!okno()) {
        return -1;
    }

    int mouseX = 0;
    int mouseY = 0;

    vector<Pole> plansza(9);
    vector<Kolizja> kolizje;
    inicjalizuj(plansza, kolizje);

    int los = rand() % 2;

    if (los == 1)
    {
        bitmap = al_load_bitmap("..//grafiki//tlo2.png");
    }
    else
    {
        bitmap = al_load_bitmap("..//grafiki//tlo.png");
    }

    cross = al_load_bitmap("..//grafiki//krzyzyk.png");
    circle = al_load_bitmap("..//grafiki//kolko.png");
    gameOverImage = al_load_bitmap("..//grafiki//gameover.png");
    wyswietl(plansza);
    while (!koniecGry)
    {
        ALLEGRO_EVENT ev;
        while (!koniecGry) {
            al_wait_for_event(queue, &ev);

            if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                koniecGry = true;
            }
            else if (ev.type == ALLEGRO_EVENT_TIMER) {
                // if (startTimer) {
                elapsedTime += 1.0;  // Zwiększenie czasu o 1 sekundę
                // }
            }
            else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
                mouseX = ev.mouse.x;
                mouseY = ev.mouse.y;
            }
            else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                sprawdzKolizje(kolizje, plansza, mouseX, mouseY);
            }
            else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                if (ev.keyboard.keycode >= ALLEGRO_KEY_1 && ev.keyboard.keycode <= ALLEGRO_KEY_9) {
                    int pole = ev.keyboard.keycode - ALLEGRO_KEY_1;
                }
            }
            wyswietl(plansza);
        }
    }

    string nick = getNickname();
    if (sprawdzWygrana('X', plansza)) {
        zapiszWynik("Wygrana gracza", nick, elapsedTime);
    }
    else if (sprawdzWygrana('O', plansza)) {
        zapiszWynik("Wygrana komputera", nick, elapsedTime);
    }
    else {
        zapiszWynik("Remis", nick, elapsedTime);
    }

    auto wyniki = czytajOstatnieWyniki("wyniki.txt", 10);
    wyswietlWyniki(wyniki);

    al_destroy_display(display);
    al_destroy_event_queue(queue);
    return 0;
}