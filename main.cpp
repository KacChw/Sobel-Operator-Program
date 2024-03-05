#include <iostream>  
#include <fstream>   

using namespace std; 

struct Piksel {
        float rgbBlue;
        float rgbGreen;
        float rgbRed;

        Piksel()
        {
                this->rgbBlue = 0;
                this->rgbGreen = 0;
                this->rgbRed = 0;
        }

        Piksel(float blue, float green, float red)
        {
                this->rgbBlue = blue;
                this->rgbGreen = green;
                this->rgbRed = red;
        }
};

typedef struct
{
        unsigned short bfType;
        unsigned int   bfSize;
        unsigned short bfReserved1;
        unsigned short bfReserved2;
        unsigned int   bfOffBits;
} BITMAPFILEHEADER;

typedef struct
{
        unsigned int   biSize;
        unsigned int   biWidth;
        unsigned int   biHeight;
        unsigned short biPlanes;
        unsigned short biBitCount;
        unsigned int   biCompression;
        unsigned int   biSizeImage;
        unsigned int   biXpelsPerMeter;
        unsigned int   biYpelsPerMeter;
        unsigned int   biCrlUses;
        unsigned int   biCrlImportant;
} BITMAPINFOHEADER;

int Tablice_Sobel[8][3][3] = {
        { {-1,0,1},   {-2,0,2}, {-1,0,1}   }, 
        { {0,1,2},    {-1,0,1}, {-2,-1,0}  }, 
        { {1,2,1},    {0,0,0},  {-1,-2,-1} }, 
        { {2,1,0},    {1,0,-1}, {0,-1,-2}  }, 
        { {1,0,-1},   {2,0,-2}, {1,0,-1}   }, 
        { {0,-1,-2},  {1,0,-1}, {2,1,0}    }, 
        { {-1,-2,-1}, {0,0,0},  {1,2,1}    }, 
        { {-2,-1,0},  {-1,0,1}, {0,1,2}    }  
};

BITMAPFILEHEADER biFileHeader; 
BITMAPINFOHEADER biInfoHeader; 

Piksel** piksele;       
ifstream plikBMP;       
string nazwaPliku;      
bool czyObrazWpamieciRAM = false; 
                                                                  

void wypiszParametryBMP()
{
        cout << "Wszystkie parametry pliku z naglowka obrazu:" << endl;
        cout << biFileHeader.bfType << "\t - sygnatura pliku" << endl;
        cout << biFileHeader.bfSize << "\t - dlugosc calego pliku w bajtach" << endl;
        cout << biFileHeader.bfReserved1 << "\t - pole zarezerwowane" << endl;
        cout << biFileHeader.bfReserved2 << "\t - pole zarezerwowane" << endl;
        cout << biFileHeader.bfOffBits << "\t - pozycja danych obrazowych w pliku" << endl;
        cout << biInfoHeader.biSize << "\t - rozmiar naglowka informacyjnego" << endl;
        cout << biInfoHeader.biWidth << "\t - szerokosc obrazu w pikselach" << endl;
        cout << biInfoHeader.biHeight << "\t - wysokosc obrazu w pikselach" << endl;
        cout << biInfoHeader.biPlanes << "\t - liczba platow" << endl;
        cout << biInfoHeader.biBitCount << "\t - liczba bitow na piksel" << endl;
        cout << biInfoHeader.biCompression << "\t - algorytm kompresji" << endl;
        cout << biInfoHeader.biSizeImage << "\t - rozmiar rysunku" << endl;
        cout << biInfoHeader.biXpelsPerMeter << "\t - rozdzielczosc pozioma" << endl;
        cout << biInfoHeader.biYpelsPerMeter << "\t - rozdzielczosc pionowa" << endl;
        cout << biInfoHeader.biCrlUses << "\t - liczba kolorow w palecie" << endl;
        cout << biInfoHeader.biCrlImportant << "\t - liczba waznych kolorow w palecie" << endl;
}

void odczytajBFH(BITMAPFILEHEADER& bfh)
{
        plikBMP.read(reinterpret_cast<char*>(&bfh.bfType), 2);
        plikBMP.read(reinterpret_cast<char*>(&bfh.bfSize), 4);
        plikBMP.read(reinterpret_cast<char*>(&bfh.bfReserved1), 2);
        plikBMP.read(reinterpret_cast<char*>(&bfh.bfReserved2), 2);
        plikBMP.read(reinterpret_cast<char*>(&bfh.bfOffBits), 4);
}

void odczytajBIH(BITMAPINFOHEADER& bih)
{
        plikBMP.read(reinterpret_cast<char*>(&bih.biSize), 4);
        plikBMP.read(reinterpret_cast<char*>(&bih.biWidth), 4);
        plikBMP.read(reinterpret_cast<char*>(&bih.biHeight), 4);
        plikBMP.read(reinterpret_cast<char*>(&bih.biPlanes), 2);
        plikBMP.read(reinterpret_cast<char*>(&bih.biBitCount), 2);
        plikBMP.read(reinterpret_cast<char*>(&bih.biCompression), 4);
        plikBMP.read(reinterpret_cast<char*>(&bih.biSizeImage), 4);
        plikBMP.read(reinterpret_cast<char*>(&bih.biXpelsPerMeter), 4);
        plikBMP.read(reinterpret_cast<char*>(&bih.biYpelsPerMeter), 4);
        plikBMP.read(reinterpret_cast<char*>(&bih.biCrlUses), 4);
        plikBMP.read(reinterpret_cast<char*>(&bih.biCrlImportant), 4);
}

void wczytajBMP()
{
        piksele = new Piksel * [biInfoHeader.biWidth];

        for (int i = 0; i < biInfoHeader.biWidth; i++)
                piksele[i] = new Piksel[biInfoHeader.biHeight];

        unsigned int liczbaBajtowZerowych = ((4 - (biInfoHeader.biWidth * 3) % 4) % 4);

        for (unsigned int y = 0; y < biInfoHeader.biHeight; y++)
        {
                for (unsigned int x = 0; x < biInfoHeader.biWidth; x++) 
                {
                        unsigned char blue  = 0;
                        unsigned char green = 0;
                        unsigned char red   = 0;

                        plikBMP.read(reinterpret_cast<char*>(&blue), 1);  
                        plikBMP.read(reinterpret_cast<char*>(&green), 1); 
                        plikBMP.read(reinterpret_cast<char*>(&red), 1); 

                        unsigned int pozY = biInfoHeader.biHeight - (y + 1); 

                        piksele[x][pozY].rgbBlue  = static_cast<float>(blue);
                        piksele[x][pozY].rgbGreen = static_cast<float>(green);
                        piksele[x][pozY].rgbRed   = static_cast<float>(red);
                }

                plikBMP.ignore(liczbaBajtowZerowych);
        } 
}

void zapiszBMP(Piksel** px, string nazwa_pliku)
{
        unsigned int liczbaBajtowZerowych = ((4 - (biInfoHeader.biWidth * 3) % 4) % 4);
        unsigned char bajty_zerowe[3] = { 0,0,0 };

        ofstream bmp; 

        bmp.open(nazwa_pliku, ios::binary); 

        bmp.write(reinterpret_cast<char*>(&biFileHeader.bfType), 2);
        bmp.write(reinterpret_cast<char*>(&biFileHeader.bfSize), 4);
        bmp.write(reinterpret_cast<char*>(&biFileHeader.bfReserved1), 2);
        bmp.write(reinterpret_cast<char*>(&biFileHeader.bfReserved2), 2);
        bmp.write(reinterpret_cast<char*>(&biFileHeader.bfOffBits), 4);
        bmp.write(reinterpret_cast<char*>(&biInfoHeader.biSize), 4);
        bmp.write(reinterpret_cast<char*>(&biInfoHeader.biWidth), 4);
        bmp.write(reinterpret_cast<char*>(&biInfoHeader.biHeight), 4);
        bmp.write(reinterpret_cast<char*>(&biInfoHeader.biPlanes), 2);
        bmp.write(reinterpret_cast<char*>(&biInfoHeader.biBitCount), 2);
        bmp.write(reinterpret_cast<char*>(&biInfoHeader.biCompression), 4);
        bmp.write(reinterpret_cast<char*>(&biInfoHeader.biSizeImage), 4);
        bmp.write(reinterpret_cast<char*>(&biInfoHeader.biXpelsPerMeter), 4);
        bmp.write(reinterpret_cast<char*>(&biInfoHeader.biYpelsPerMeter), 4);
        bmp.write(reinterpret_cast<char*>(&biInfoHeader.biCrlUses), 4);
        bmp.write(reinterpret_cast<char*>(&biInfoHeader.biCrlImportant), 4);

        for (unsigned int y = 0; y < biInfoHeader.biHeight; y++)
        {
                for (unsigned int x = 0; x < biInfoHeader.biWidth; x++)
                {
                        unsigned int pozY = biInfoHeader.biHeight - (y + 1);

                        unsigned char bgr[3] = {   static_cast<unsigned char>(px[x][pozY].rgbBlue),
                                                                           static_cast<unsigned char>(px[x][pozY].rgbGreen),
                                                                           static_cast<unsigned char>(px[x][pozY].rgbRed) };

                        bmp.write(reinterpret_cast<char*>(bgr), 3);
                }

                bmp.write(reinterpret_cast<char*>(bajty_zerowe), liczbaBajtowZerowych);
        }

        bmp.close();
}


Piksel pobierzPixel(int y, int x)
{
        if (czyObrazWpamieciRAM)
                return piksele[x][y]; 

        unsigned char blue = 0;
        unsigned char green = 0;
        unsigned char red = 0;

        int liczbaBajtowZerowych = ((4 - (biInfoHeader.biWidth * 3) % 4) % 4);
        int rozmiarWierszaDanych = biInfoHeader.biWidth * 3 + liczbaBajtowZerowych;

        plikBMP.seekg(biFileHeader.bfOffBits + rozmiarWierszaDanych * (biInfoHeader.biHeight - (y + 1)) + 3 * x);

        plikBMP.read(reinterpret_cast<char*>(&blue), 1);
        plikBMP.read(reinterpret_cast<char*>(&green), 1);
        plikBMP.read(reinterpret_cast<char*>(&red), 1);

        return Piksel(static_cast<float>(blue), static_cast<float>(green), static_cast<float>(red));
}

Piksel** operacjaKonwolucji(int filtr[][3])
{
        Piksel** przetworzonyObraz = new Piksel * [biInfoHeader.biWidth];

        for (int i = 0; i < biInfoHeader.biWidth; i++)
                przetworzonyObraz[i] = new Piksel[biInfoHeader.biHeight];

        for (int y = 1; y < biInfoHeader.biHeight - 1; y++)
        {
                for (int x = 1; x < biInfoHeader.biWidth - 1; x++)
                {
                        
                        Piksel okno[9];
                        okno[0] = pobierzPixel(y - 1, x - 1);
                        okno[1] = pobierzPixel(y, x - 1);
                        okno[2] = pobierzPixel(y + 1, x - 1);
                        okno[3] = pobierzPixel(y - 1, x);
                        okno[4] = pobierzPixel(y, x);
                        okno[5] = pobierzPixel(y + 1, x);
                        okno[6] = pobierzPixel(y - 1, x + 1);
                        okno[7] = pobierzPixel(y, x + 1);
                        okno[8] = pobierzPixel(y + 1, x + 1);

                        float blue = 0;
                        float green = 0;
                        float red = 0;

                        
                        unsigned int i = 0;

                        for (unsigned int xx = 0; xx < 3; xx++)
                        {
                                for (unsigned int yy = 0; yy < 3; yy++)
                                {
                                        blue  += okno[i].rgbBlue  * filtr[xx][yy];
                                        green += okno[i].rgbGreen * filtr[xx][yy];
                                        red   += okno[i].rgbRed   * filtr[xx][yy];
                                        i++;
                                }
                        }

                        Piksel nowy_piksel = Piksel(blue, green, red);

                        
                        if (nowy_piksel.rgbBlue > 255)
                                nowy_piksel.rgbBlue = 255;
                        else if (nowy_piksel.rgbBlue < 0)
                                nowy_piksel.rgbBlue = 0;

                        if (nowy_piksel.rgbGreen > 255)
                                nowy_piksel.rgbGreen = 255;
                        else if (nowy_piksel.rgbGreen < 0)
                                nowy_piksel.rgbGreen = 0;

                        if (nowy_piksel.rgbRed > 255)
                                nowy_piksel.rgbRed = 255;
                        else if (nowy_piksel.rgbRed < 0)
                                nowy_piksel.rgbRed = 0;

                        przetworzonyObraz[x][y] = nowy_piksel;
                }
        }

        return przetworzonyObraz; 
}

int main() 
{
        while (1) 
        {
                cout << "\n**************** MENU UZYTKOWNIKA: ****************" << endl;
                cout << "1. Odczytaj i wyswietl naglowek obrazu bmp." << endl;
                cout << "2. Ustaw sposob wczytywania pikseli obrazu." << endl;
                cout << "3. Przeprowadz operacje konwolucji obrazu z tablicami charakterystycznymi operatora Sobel." << endl;

                int nr_wyboru;    
                cin >> nr_wyboru; 

                if (nr_wyboru == 1)
                {
                        cout << "Wprowadz nazwe pliku ktory chcesz odczytac i nastepnie nacisnij enter:\n";

                        cin >> nazwaPliku; 

                        czyObrazWpamieciRAM = false; 

                        plikBMP = ifstream(nazwaPliku, ios::binary); 

                        if (plikBMP.is_open()) 
                        {
                                odczytajBFH(biFileHeader); 
                                odczytajBIH(biInfoHeader); 
                                wypiszParametryBMP();      
                        }
                }
                else if (nr_wyboru == 2)
                {
                        cout << "Wprowadz numer sposobu wczytywania pikseli, 0 - wczytujemy piksele z pliku, 1 - alokujemy dynamiczna tablice w pamieci na dane obrazu: " << endl;
                        cout << "Ustawiono" << std::endl;

                        int nr = 0; 
                        cin >> nr;

                        if (nr == 0)
                        {
                                czyObrazWpamieciRAM = false;
                                continue;
                        }

                        
                        plikBMP.seekg(biFileHeader.bfOffBits);
                        wczytajBMP();
                        czyObrazWpamieciRAM = true;
                }
                else if (nr_wyboru == 3)
                {
                        cout << "Wprowadz nazwe zapisywanego pliku" << endl;

                        string nazwa_pliku;
                        cin >> nazwa_pliku;

                        Piksel** obraz[8]; 

                        
                        for (unsigned int i = 0; i < 8; i++)
                                obraz[i] = operacjaKonwolucji(Tablice_Sobel[i]);

                        
                        for (unsigned int y = 0; y < biInfoHeader.biHeight; y++)
                        {
                                for (unsigned int x = 0; x < biInfoHeader.biWidth; x++)
                                {
                                        obraz[0][x][y].rgbBlue = obraz[0][x][y].rgbBlue + obraz[1][x][y].rgbBlue +
                                                                                         obraz[2][x][y].rgbBlue + obraz[3][x][y].rgbBlue +
                                                                                         obraz[4][x][y].rgbBlue + obraz[5][x][y].rgbBlue +
                                                                                          obraz[6][x][y].rgbBlue + obraz[7][x][y].rgbBlue;

                                        obraz[0][x][y].rgbGreen = obraz[0][x][y].rgbGreen + obraz[1][x][y].rgbGreen +
                                                                                          obraz[2][x][y].rgbGreen + obraz[3][x][y].rgbGreen +
                                                                                          obraz[4][x][y].rgbGreen + obraz[5][x][y].rgbGreen +
                                                                                          obraz[6][x][y].rgbGreen + obraz[7][x][y].rgbGreen;

                                        obraz[0][x][y].rgbRed = obraz[0][x][y].rgbRed + obraz[1][x][y].rgbRed +
                                                                                        obraz[2][x][y].rgbRed + obraz[3][x][y].rgbRed +
                                                                                        obraz[4][x][y].rgbRed + obraz[5][x][y].rgbRed +
                                                                                        obraz[6][x][y].rgbRed + obraz[7][x][y].rgbRed;

                                        obraz[0][x][y].rgbBlue  = obraz[0][x][y].rgbBlue  / 8;
                                        obraz[0][x][y].rgbGreen = obraz[0][x][y].rgbGreen / 8;
                                        obraz[0][x][y].rgbRed   = obraz[0][x][y].rgbRed   / 8;

                                }
                        }

                        
                        zapiszBMP(obraz[0], nazwa_pliku);

                        cout << "Obraz o podanej nazwie zostal zapisany na dysk" << endl;
                }
                else
                {
                        std::cout << "Podano nieistniejacy numer polecenia" << std::endl;
                }
        }

        return 0;
}
