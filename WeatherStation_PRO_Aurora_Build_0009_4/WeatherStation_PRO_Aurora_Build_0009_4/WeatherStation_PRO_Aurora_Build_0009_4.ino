/*
  WeatherStation PRO / Aurora Framework
  Versione 0.12.2-alpha - Build 0007.2 Alpha

  Funzioni principali:
  - Meteo attuale, previsioni 5 giorni e dettaglio orario
  - Configurazione Wi-Fi e localita dal portale web
  - Ricerca automatica della localita con Open-Meteo Geocoding
  - Profili display con prova e conferma entro 5 secondi
  - Luminosita e standby memorizzati
  - Riattivazione tramite primo tocco
  - Diagnostica e ripristino impostazioni

  Libreria esterna richiesta:
  - ArduinoJson
*/

#include "AuroraConfig.h"
#include "AuroraTheme.h"
#include "AuroraLanguage.h"
#include "AuroraEventLog.h"
#include "AuroraDisplay.h"
#include "AuroraUI.h"
#include "AuroraSettings.h"
#include "AuroraNetwork.h"
#include "AuroraClock.h"
#include "AuroraLocation.h"
#include "AuroraWeather.h"
#include "AuroraPower.h"
#include "AuroraPages.h"

AuroraDisplay display;
AuroraUI ui(display);
AuroraSettings settings;
AuroraLanguage language;
AuroraEventLog eventLog;
AuroraNetwork network(settings);
AuroraClock clockService;
AuroraLocation locationService(settings);
AuroraWeather weatherService(settings);
AuroraPower powerService(settings);
AuroraPages pages(
  display,
  ui,
  settings,
  language,
  network,
  clockService,
  locationService,
  weatherService,
  powerService,
  eventLog
);

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println("================================");
  Serial.println("WeatherStation PRO");
  Serial.println("Aurora OS Build 0007.2 Alpha");
  Serial.println("Avvio sicuro...");
  Serial.println("================================");

  Serial.println("[1/9] Display");
  display.begin();

  Serial.println("[2/9] Settings");
  settings.begin();

  Serial.println("[3/9] Power");
  powerService.begin();
  Serial.println("[4/9] Language");
  language.setLanguage(
    settings.loadLanguage() == 1
      ? LANGUAGE_ENGLISH
      : LANGUAGE_ITALIAN
  );

  Serial.println("[5/9] Theme");
  ui.setTheme(
    settings.loadTheme() == 1
      ? THEME_LIGHT
      : THEME_DARK
  );

  Serial.println("[6/9] Pages");
  eventLog.begin();
  eventLog.add(EVENT_INFO, "Boot");

  pages.beginSafeState();

  pages.showSplashStep(
    10,
    TEXT_INITIALIZING
  );

  Serial.println("[7/9] Network");
  network.begin();

  if (!network.hasCredentials()) {
    network.startPortal();
    pages.showPortalInstructions();
    return;
  }

  pages.showSplashStep(
    30,
    TEXT_CONNECTING_WIFI
  );

  if (!network.connect(20000)) {
    network.startPortal();
    pages.showPortalInstructions();
    return;
  }

  pages.showSplashStep(
    50,
    TEXT_FINDING_LOCATION
  );
  Serial.println("[8/9] Location");
  eventLog.add(
    network.isConnected() ? EVENT_INFO : EVENT_WARNING,
    network.isConnected() ? "WiFi connected" : "WiFi offline"
  );

  locationService.resolvePendingCity(network.isConnected());

  pages.showSplashStep(
    70,
    TEXT_SYNCING_TIME
  );
  clockService.begin();
  clockService.synchronize(15000);

  pages.showSplashStep(
    90,
    TEXT_DOWNLOADING_WEATHER
  );
  Serial.println("[9/9] Weather");
  weatherService.begin();
  if (weatherService.update(network.isConnected())) {
    eventLog.add(EVENT_INFO, "Weather updated");
  } else {
    eventLog.add(EVENT_WARNING, "Weather update failed");
  }

  pages.showSplashStep(
    100,
    TEXT_READY
  );
  delay(350);
  pages.begin();

  Serial.println("Avvio completato.");
}

void loop() {
  network.update();

  if (network.isPortalActive()) {
    pages.update();

    if (network.restartRequested()) {
      delay(700);
      ESP.restart();
    }

    delay(2);
    return;
  }

  if (network.consumePortalClosed()) {
    pages.returnToSettingsAfterPortal();
  }

  powerService.update();
  clockService.update();

  if (
    network.isConnected() &&
    weatherService.shouldUpdate()
  ) {
    if (weatherService.update(true)) {
      eventLog.add(EVENT_INFO, "Weather updated");
      pages.notifyWeatherUpdated();
    } else {
      eventLog.add(EVENT_WARNING, "Weather update failed");
    }
  }

  pages.update();
  delay(10);
}
