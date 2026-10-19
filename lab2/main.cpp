#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include <iostream>

enum Narzedzie
{
    LINIA,
    PROSTOKAT,
    PELNY_PROSTOKAT,
    OKRAG
};

struct Figura
{
    Narzedzie typ;
    sf::Vector2f start;
    sf::Vector2f koniec;
    sf::Color kolor;
    sf::Color wypelnienie;
    float grubosc;
};

void rysujFigure(sf::RenderTarget& ekran, const Figura& f)
{
    sf::Vector2f rozmiar(
        std::abs(f.koniec.x - f.start.x),
        std::abs(f.koniec.y - f.start.y)
    );

    if (f.typ == LINIA)
    {
        sf::Vertex linia[] =
        {
            sf::Vertex(f.start, f.kolor),
            sf::Vertex(f.koniec, f.kolor)
        };

        ekran.draw(linia, 2, sf::PrimitiveType::Lines);
    }
    else if (f.typ == OKRAG)
    {
        float promien = std::sqrt(
            rozmiar.x * rozmiar.x +
            rozmiar.y * rozmiar.y
        );

        sf::CircleShape okrag(promien);
        okrag.setPosition(f.start);
        okrag.setFillColor(f.wypelnienie);
        okrag.setOutlineColor(f.kolor);
        okrag.setOutlineThickness(f.grubosc);

        ekran.draw(okrag);
    }
    else
    {
        sf::RectangleShape prostokat(rozmiar);

        prostokat.setPosition(sf::Vector2f(
            std::min(f.start.x, f.koniec.x),
            std::min(f.start.y, f.koniec.y)
        ));

        if (f.typ == PELNY_PROSTOKAT)
            prostokat.setFillColor(f.wypelnienie);
        else
            prostokat.setFillColor(sf::Color::Transparent);

        prostokat.setOutlineColor(f.kolor);
        prostokat.setOutlineThickness(f.grubosc);

        ekran.draw(prostokat);
    }
}

int main()
{
    sf::RenderWindow okno(
        sf::VideoMode({ 1800, 900 }),
        "Program do rysowania",
        sf::Style::Titlebar | sf::Style::Close
    );

    okno.setFramerateLimit(60);

    if (!ImGui::SFML::Init(okno))
        return -1;

    // Wczytywanie tekstur ikon
    sf::Texture texLinia, texProstokat, texPelnyProstokat, texOkrag, texMiotla;

    // Upewnij się, że pliki ikon znajdują się w katalogu roboczym programu
    texLinia.loadFromFile("line.png");
    texProstokat.loadFromFile("square.png");
    texPelnyProstokat.loadFromFile("filled_square.png");
    texOkrag.loadFromFile("circle.png");
    texMiotla.loadFromFile("broom.png");

    sf::RenderTexture obraz(sf::Vector2u({ 1800, 900 }));
    obraz.clear(sf::Color::White);
    obraz.display();

    std::vector<Figura> figury;

    Narzedzie narzedzie = LINIA;

    sf::Color kolor = sf::Color::Black;
    sf::Color wypelnienie = sf::Color::White;

    float grubosc = 3.0f;

    bool rysowanie = false;
    bool pokazOpcje = true;
    bool gradient = false;
    bool potwierdzWyjscie = false;
    bool about = false;

    sf::Vector2f start;
    sf::Vector2f koniec;

    char nazwaPliku[256] = "obraz.png";

    sf::Clock zegar;

    while (okno.isOpen())
    {
        while (const auto zdarzenie = okno.pollEvent())
        {
            ImGui::SFML::ProcessEvent(okno, *zdarzenie);

            if (zdarzenie->is<sf::Event::Closed>())
                okno.close();

            if (const auto* klik = zdarzenie->getIf<sf::Event::MouseButtonPressed>())
            {
                if (klik->button == sf::Mouse::Button::Left &&
                    !ImGui::GetIO().WantCaptureMouse)
                {
                    start = okno.mapPixelToCoords(klik->position);
                    koniec = start;
                    rysowanie = true;
                }
            }

            if (const auto* ruch = zdarzenie->getIf<sf::Event::MouseMoved>())
            {
                if (rysowanie)
                    koniec = okno.mapPixelToCoords(ruch->position);
            }

            if (const auto* klik = zdarzenie->getIf<sf::Event::MouseButtonReleased>())
            {
                if (klik->button == sf::Mouse::Button::Left && rysowanie)
                {
                    koniec = okno.mapPixelToCoords(klik->position);

                    figury.push_back({
                        narzedzie,
                        start,
                        koniec,
                        kolor,
                        wypelnienie,
                        grubosc
                        });

                    rysowanie = false;
                }
            }
        }

        ImGui::SFML::Update(okno, zegar.restart());

        // MENU
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New"))
                {
                    figury.clear();
                    obraz.clear(sf::Color::White);
                    obraz.display();
                }

                ImGui::InputText(
                    "Nazwa pliku",
                    nazwaPliku,
                    sizeof(nazwaPliku)
                );

                if (ImGui::MenuItem("Open"))
                {
                    sf::Texture wczytanaTekstura;

                    if (wczytanaTekstura.loadFromFile(nazwaPliku))
                    {
                        sf::Sprite sprite(wczytanaTekstura);

                        obraz.clear(sf::Color::White);
                        obraz.draw(sprite);
                        obraz.display();

                        figury.clear();
                    }
                }

                if (ImGui::MenuItem("Save As..."))
                {
                    for (const auto& f : figury)
                        rysujFigure(obraz, f);

                    obraz.display();

                    sf::Image doZapisu = obraz.getTexture().copyToImage();
                    doZapisu.saveToFile(nazwaPliku);
                }

                if (ImGui::MenuItem("Exit"))
                    potwierdzWyjscie = true;

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                ImGui::MenuItem(
                    "Show Tool Options",
                    nullptr,
                    &pokazOpcje
                );

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("About"))
                    about = true;

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        // PANEL TOOLS Z IKONAMI
        ImGui::Begin("Tools");

        sf::Vector2f iconSize(24.0f, 24.0f);

        // Przycisk: Linia (używający ikony miotełki)
        if (ImGui::ImageButton("btn_linia_miotla", texMiotla, iconSize))
            narzedzie = LINIA;
        ImGui::SameLine();
        if (ImGui::Button("Linia"))
            narzedzie = LINIA;

        // Przycisk: Prostokat
        if (ImGui::ImageButton("btn_prostokat", texProstokat, iconSize))
            narzedzie = PROSTOKAT;
        ImGui::SameLine();
        if (ImGui::Button("Prostokat"))
            narzedzie = PROSTOKAT;

        // Przycisk: Wypelniony prostokat
        if (ImGui::ImageButton("btn_pelny_prostokat", texPelnyProstokat, iconSize))
            narzedzie = PELNY_PROSTOKAT;
        ImGui::SameLine();
        if (ImGui::Button("Wypelniony prostokat"))
            narzedzie = PELNY_PROSTOKAT;

        // Przycisk: Okrag
        if (ImGui::ImageButton("btn_okrag", texOkrag, iconSize))
            narzedzie = OKRAG;
        ImGui::SameLine();
        if (ImGui::Button("Okrag"))
            narzedzie = OKRAG;

        ImGui::Separator();

        float c1[] = {
            kolor.r / 255.0f,
            kolor.g / 255.0f,
            kolor.b / 255.0f
        };

        if (ImGui::ColorEdit3("Border Color", c1))
        {
            kolor = sf::Color(
                static_cast<std::uint8_t>(c1[0] * 255),
                static_cast<std::uint8_t>(c1[1] * 255),
                static_cast<std::uint8_t>(c1[2] * 255)
            );
        }

        float c2[] = {
            wypelnienie.r / 255.0f,
            wypelnienie.g / 255.0f,
            wypelnienie.b / 255.0f
        };

        if (ImGui::ColorEdit3("Fill Color", c2))
        {
            wypelnienie = sf::Color(
                static_cast<std::uint8_t>(c2[0] * 255),
                static_cast<std::uint8_t>(c2[1] * 255),
                static_cast<std::uint8_t>(c2[2] * 255)
            );
        }

        if (pokazOpcje)
        {
            ImGui::SliderFloat(
                "Grubosc",
                &grubosc,
                1.0f,
                20.0f
            );

            ImGui::Checkbox("Gradient linii", &gradient);
        }

        ImGui::End();

        // OKNO ABOUT
        if (about)
        {
            ImGui::OpenPopup("About");
            about = false;
        }

        if (ImGui::BeginPopupModal(
            "About",
            nullptr,
            ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Program do rysowania");
            ImGui::Text("Autor: Natalia");

            if (ImGui::Button("OK"))
                ImGui::CloseCurrentPopup();

            ImGui::EndPopup();
        }

        // POTWIERDZENIE WYJSCIA
        if (potwierdzWyjscie)
            ImGui::OpenPopup("Czy na pewno wyjsc?");

        if (ImGui::BeginPopupModal(
            "Czy na pewno wyjsc?",
            nullptr,
            ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Czy chcesz zamknac program?");

            if (ImGui::Button("Tak"))
                okno.close();

            ImGui::SameLine();

            if (ImGui::Button("Nie"))
            {
                potwierdzWyjscie = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        // RYSOWANIE
        okno.clear(sf::Color::White);

        sf::Sprite spriteTla(obraz.getTexture());
        okno.draw(spriteTla);

        for (const auto& f : figury)
            rysujFigure(okno, f);

        if (rysowanie)
        {
            Figura tymczasowa = {
                narzedzie,
                start,
                koniec,
                kolor,
                wypelnienie,
                grubosc
            };

            if (narzedzie == LINIA && gradient)
            {
                sf::Vertex linia[] =
                {
                    sf::Vertex(start, kolor),
                    sf::Vertex(koniec, wypelnienie)
                };

                okno.draw(linia, 2, sf::PrimitiveType::Lines);
            }
            else
            {
                rysujFigure(okno, tymczasowa);
            }
        }

        ImGui::SFML::Render(okno);
        okno.display();
    }

    ImGui::SFML::Shutdown(okno);

    return 0;
}
