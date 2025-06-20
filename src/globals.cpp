// Copyright (c) Tyler Veness

#include "globals.hpp"

#include <SFML/Graphics/Font.hpp>

sf::Font& global_font() {
  static sf::Font font{"resources/technical.ttf"};
  return font;
}
