#include "AuroraWeatherDescriptions.h"

namespace {

const char* italianDescription(int code) {
  switch (code) {
    case 0:
      return "Sereno";

    case 1:
      return "Quasi sereno";

    case 2:
      return "Parz. nuvoloso";

    case 3:
      return "Coperto";

    case 45:
    case 48:
      return "Nebbia";

    case 51:
    case 53:
    case 55:
      return "Pioviggine";

    case 56:
    case 57:
    case 61:
    case 63:
    case 65:
    case 66:
    case 67:
      return "Pioggia";

    case 71:
    case 73:
    case 75:
    case 77:
    case 85:
    case 86:
      return "Neve";

    case 80:
    case 81:
    case 82:
      return "Rovesci";

    case 95:
    case 96:
    case 99:
      return "Temporale";

    default:
      return "Non disponibile";
  }
}

const char* englishDescription(int code) {
  switch (code) {
    case 0:
      return "Clear sky";

    case 1:
      return "Mainly clear";

    case 2:
      return "Partly cloudy";

    case 3:
      return "Overcast";

    case 45:
    case 48:
      return "Fog";

    case 51:
    case 53:
    case 55:
      return "Drizzle";

    case 56:
    case 57:
    case 61:
    case 63:
    case 65:
    case 66:
    case 67:
      return "Rain";

    case 71:
    case 73:
    case 75:
    case 77:
    case 85:
    case 86:
      return "Snow";

    case 80:
    case 81:
    case 82:
      return "Showers";

    case 95:
    case 96:
    case 99:
      return "Thunderstorm";

    default:
      return "Unavailable";
  }
}

}  // namespace

const char* auroraWeatherDescription(
  int weatherCode,
  AuroraLanguageId language
) {
  return language == LANGUAGE_ENGLISH
    ? englishDescription(weatherCode)
    : italianDescription(weatherCode);
}
