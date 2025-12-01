# Wizualizacja 3D - Klucz Kryptograficzny

**Autor:** Mateusz Kołodziej
**Data:** 1 Grudzień 2025

---

## Opis projektu

Interaktywna aplikacja 3D przedstawiająca wizualizację klucza kryptograficznego zbudowanego z tysięcy małych sześcianów. Projekt demonstruje zastosowanie OpenGL z oświetleniem Phonga, teksturowaniem oraz animacją pęknięcia i eksplozji obiektu.

### Główne funkcjonalności:
- **Proceduralne generowanie geometrii** - klucz składa się z ~10000 sześcianów tworzących realistyczny kształt klucza z główką, trzonem i zębami
- **Interaktywna kamera** - swobodny ruch i rotacja kamery w przestrzeni 3D
- **Oświetlenie Phonga** - pełny model oświetlenia (ambient + diffuse + specular)
- **Teksturowanie** - każdy sześcian pokryty teksturą matrix.png
- **Animacja pęknięcia** - realistyczne dwuetapowe pękanie klucza (pęknięcie po 1.5s → wybuch po 3s)



## Sterowanie

| Klawisz | Akcja 
| **W/A/S/D** | Ruch kamery (przód/lewo/tył/prawo) 
| **Spacja** | Ruch kamery w górę 
| **Left Shift** | Ruch kamery w dół 
| **Mysz** | Obrót kamery 
| **O** | Włącz/wyłącz automatyczną rotację klucza 
| **R** | Uruchom sekwencję pęknięcia i eksplozji 
| **N** | Reset sceny (nowy klucz) 
| **ESC** | Wyjście z programu 

---

## Technologie i biblioteki

### Wykorzystane technologie:
- **OpenGL** - fixed pipeline
- **SFML** - okno, kontekst OpenGL, ładowanie tekstur
- **GLU** - funkcje pomocnicze (gluPerspective, gluLookAt)
- **C++** - język programowania


### Tekstury:
- `matrix.png` - tekstura w stylu Matrix (512×512 px)

### Uruchomienie
#### 1. Utwórz nowy projekt w Visual Studio
#### 2. Dodaj biblioteki SFML przez NuGet (tak jak na laboratorium)
#### 3. Skonfiguruj linkowanie OpenGL (tak jak na laboratorium)
#### 4. Dodaj kod źródłowy (projektGrafika3D.cpp)
#### 5. Dodaj teksturę
**WAŻNE:** W linii 430 kodu znajduje się ścieżka do tekstury, zmień tę ścieżkę na taką gdzie masz matrix.png
#### 6. Skompiluj i uruchom

## Prezentacja projektu
Do repozytorium dołączony jest plik GIF prezentujący działanie.