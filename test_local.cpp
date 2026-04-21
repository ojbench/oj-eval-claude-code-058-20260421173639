#include "src.hpp"
#include <iostream>
#include <cassert>

void test_basic() {
    Pokedex p("test.db");
    p.pokeAdd("Squirtle", 7, "water");
    p.pokeAdd("Charmander", 4, "fire");
    p.pokeAdd("Bulbasaur", 1, "grass");

    assert(p.pokeFind(7) == "Squirtle");
    assert(p.pokeFind(4) == "Charmander");
    assert(p.pokeFind(1) == "Bulbasaur");
    assert(p.pokeFind(2) == "None");

    std::cout << "Basic find tests passed\n";

    std::string waterFind = p.typeFind("water");
    std::cout << "Water find:\n" << waterFind << std::endl;

    assert(p.attack("water", 4) == 2.0f);
    assert(p.attack("fire", 1) == 2.0f);
    assert(p.attack("grass", 7) == 2.0f);

    std::cout << "Basic attack tests passed\n";
}

void test_exceptions() {
    Pokedex p("test_ex.db");
    try {
        p.pokeAdd("pm1", 1, "water");
    } catch (const ArgumentException& e) {
        std::cout << "Caught expected exception: " << e.what() << std::endl;
    }

    try {
        p.typeFind("water#ice#block");
    } catch (const ArgumentException& e) {
        std::cout << "Caught expected exception: " << e.what() << std::endl;
    }
}

void test_iterator() {
    Pokedex p("test_it.db");
    p.pokeAdd("Bulbasaur", 1, "grass");
    p.pokeAdd("Charmander", 4, "fire");
    p.pokeAdd("Squirtle", 7, "water");

    std::cout << "Iterator test:\n";
    for (auto it = p.begin(); it != p.end(); ++it) {
        std::cout << it->id << ": " << it->name << std::endl;
    }
}

int main() {
    test_basic();
    test_exceptions();
    test_iterator();
    std::cout << "All local tests passed!\n";
    return 0;
}
