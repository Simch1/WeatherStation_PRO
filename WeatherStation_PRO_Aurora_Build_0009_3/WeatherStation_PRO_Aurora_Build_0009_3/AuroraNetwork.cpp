#include "AuroraNetwork.h"
#include "AuroraConfig.h"

AuroraNetwork::AuroraNetwork(AuroraSettings& settings)
  : _settings(settings),
    _server(80),
    _portalActive(false),
    _restartRequested(false),
    _portalClosedEvent(false),
    _lastReconnectAttempt(0),
    _portalStartedAt(0),
    _portalMode(PORTAL_WIFI_AND_LOCATION) {}

void AuroraNetwork::begin() {
  _ssid = _settings.loadSSID();
  _password = _settings.loadPassword();
}

bool AuroraNetwork::hasCredentials() const {
  return _ssid.length() > 0;
}

bool AuroraNetwork::connect(unsigned long timeoutMs) {
  if (!hasCredentials()) {
    return false;
  }

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);
  WiFi.begin(_ssid.c_str(), _password.c_str());

  const unsigned long start = millis();

  while (
    WiFi.status() != WL_CONNECTED &&
    millis() - start < timeoutMs
  ) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi collegato: ");
    Serial.println(WiFi.localIP());
    return true;
  }

  return false;
}

void AuroraNetwork::update() {
  if (_portalActive) {
    _dns.processNextRequest();
    _server.handleClient();

    if (
      WiFi.softAPgetStationNum() == 0 &&
      millis() - _portalStartedAt >= CONFIG_PORTAL_TIMEOUT_MS
    ) {
      cancelPortal();
    }

    return;
  }

  if (
    WiFi.status() != WL_CONNECTED &&
    millis() - _lastReconnectAttempt >=
      WIFI_RECONNECT_INTERVAL_MS
  ) {
    _lastReconnectAttempt = millis();
    WiFi.reconnect();
  }
}

bool AuroraNetwork::isConnected() const {
  return WiFi.status() == WL_CONNECTED;
}

int AuroraNetwork::rssi() const {
  return isConnected() ? WiFi.RSSI() : -100;
}

String AuroraNetwork::ipAddress() const {
  return isConnected()
    ? WiFi.localIP().toString()
    : "Non collegato";
}

String AuroraNetwork::ssid() const {
  return _ssid;
}

void AuroraNetwork::startPortal(
  AuroraPortalMode mode
) {
  if (_portalActive) {
    return;
  }

  _portalActive = true;
  _portalClosedEvent = false;
  _portalMode = mode;
  _portalStartedAt = millis();

  if (
    mode == PORTAL_LOCATION_ONLY &&
    WiFi.status() == WL_CONNECTED
  ) {
    // Cambio localita: conserva la connessione domestica
    // e crea un AP temporaneo APERTO. In questo modo
    // lo smartphone non deve inserire alcuna password.
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(SETUP_AP_NAME);
  } else {
    // Cambio Wi-Fi o prima configurazione:
    // usa un AP aperto. Se l'utente cambia router/gestore
    // deve poter entrare subito dal telefono senza ricordare
    // la vecchia password del portale.
    WiFi.disconnect(true);
    delay(250);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(SETUP_AP_NAME);
  }

  _dns.start(
    53,
    "*",
    WiFi.softAPIP()
  );

  _server.on("/", HTTP_GET, [this]() {
    handleRoot();
  });

  _server.on("/save", HTTP_POST, [this]() {
    handleSave();
  });

  _server.onNotFound([this]() {
    handleNotFound();
  });

  _server.begin();
}


void AuroraNetwork::cancelPortal() {
  if (!_portalActive) {
    return;
  }

  _server.stop();
  _dns.stop();
  WiFi.softAPdisconnect(true);
  delay(150);

  _portalActive = false;
  _portalClosedEvent = true;

  // Ritorna alla sola modalita stazione.
  // Se la connessione e rimasta attiva non viene riavviata.
  WiFi.mode(WIFI_STA);

  if (
    WiFi.status() != WL_CONNECTED &&
    hasCredentials()
  ) {
    WiFi.begin(
      _ssid.c_str(),
      _password.c_str()
    );
  }
}

void AuroraNetwork::restartPortal() {
  if (_portalActive) {
    _server.stop();
    _dns.stop();
    WiFi.softAPdisconnect(true);
    _portalActive = false;
    delay(200);
  }

  startPortal(_portalMode);
}

bool AuroraNetwork::isPortalActive() const {
  return _portalActive;
}

bool AuroraNetwork::restartRequested() const {
  return _restartRequested;
}

bool AuroraNetwork::consumePortalClosed() {
  const bool value = _portalClosedEvent;
  _portalClosedEvent = false;
  return value;
}

unsigned long AuroraNetwork::portalSecondsRemaining() const {
  if (!_portalActive) {
    return 0;
  }

  const unsigned long elapsed = millis() - _portalStartedAt;

  if (elapsed >= CONFIG_PORTAL_TIMEOUT_MS) {
    return 0;
  }

  return (CONFIG_PORTAL_TIMEOUT_MS - elapsed + 999UL) / 1000UL;
}

uint8_t AuroraNetwork::portalClientCount() const {
  return _portalActive
    ? static_cast<uint8_t>(WiFi.softAPgetStationNum())
    : 0;
}

AuroraPortalMode AuroraNetwork::portalMode() const {
  return _portalMode;
}

void AuroraNetwork::handleRoot() {
  _server.sendHeader(
    "Cache-Control",
    "no-store, no-cache, must-revalidate, max-age=0"
  );
  _server.sendHeader("Pragma", "no-cache");
  _server.sendHeader("Expires", "0");
  _server.send(200, "text/html", portalPage());
}

void AuroraNetwork::handleSave() {
  String newSSID = _server.arg("ssid");
  String newPassword = _server.arg("password");
  String newCity = _server.arg("city");

  newSSID.trim();
  newCity.trim();

  if (
    _portalMode != PORTAL_LOCATION_ONLY &&
    newSSID.length() == 0
  ) {
    _server.send(
      400,
      "text/html",
      portalPage(
        _settings.loadLanguage() == 1
          ? "Enter the Wi-Fi network name."
          : "Inserisci il nome della rete Wi-Fi."
      )
    );
    return;
  }

  if (
    _portalMode != PORTAL_WIFI_ONLY &&
    newCity.length() == 0
  ) {
    _server.send(
      400,
      "text/html",
      portalPage(
        _settings.loadLanguage() == 1
          ? "Enter the location."
          : "Inserisci la localita."
      )
    );
    return;
  }

  if (_portalMode != PORTAL_LOCATION_ONLY) {
    _settings.saveWiFi(
      newSSID,
      newPassword
    );

    _ssid = newSSID;
    _password = newPassword;
  }

  if (_portalMode != PORTAL_WIFI_ONLY) {
    _settings.savePendingCity(newCity);
  }

  _server.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
  _server.send(
    200,
    "text/html",
    portalPage(
      _settings.loadLanguage() == 1
        ? "Saved. The station is restarting."
        : "Salvato. La stazione si sta riavviando."
    )
  );

  _restartRequested = true;
}

void AuroraNetwork::handleNotFound() {
  _server.sendHeader("Location", "/", true);
  _server.send(302, "text/plain", "");
}

String AuroraNetwork::portalPage(
  const String& message
) {
  const bool english =
    _settings.loadLanguage() == 1;

  const bool showWiFi =
    _portalMode != PORTAL_LOCATION_ONLY;

  const bool showLocation =
    _portalMode != PORTAL_WIFI_ONLY;

  const char* intro;

  if (_portalMode == PORTAL_WIFI_ONLY) {
    intro = english
      ? "Enter the new Wi-Fi connection."
      : "Inserisci la nuova connessione Wi-Fi.";
  } else if (_portalMode == PORTAL_LOCATION_ONLY) {
    intro = english
      ? "Enter the new weather location."
      : "Inserisci la nuova localita meteo.";
  } else {
    intro = english
      ? "Enter Wi-Fi and location data."
      : "Inserisci Wi-Fi e localita.";
  }

  String page;
  page.reserve(3000);

  page += F(
    "<!DOCTYPE html><html><head>"
    "<meta charset='UTF-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<style>"
    "body{font-family:Arial;background:#071521;color:#fff;margin:0;padding:18px}"
    ".box{max-width:440px;margin:25px auto;background:#10283b;padding:24px;"
    "border:1px solid #31536a;border-radius:16px}"
    "h1{margin-top:0;color:#fff}.accent{color:#38d7ff}"
    "label{display:block;margin-top:17px;color:#d7e3ea}"
    "input{width:100%;box-sizing:border-box;padding:13px;margin-top:7px;"
    "font-size:17px;border:1px solid #527086;border-radius:9px}"
    "button{width:100%;margin-top:24px;padding:14px;font-size:17px;"
    "background:#087da0;color:#fff;border:0;border-radius:9px}"
    ".msg{padding:11px;background:#173d2c;border-radius:8px;margin-bottom:14px}"
    ".mode{color:#9cc8d8;font-size:14px}"
    "</style></head><body><div class='box'>"
  );

  page += "<h1>WeatherStation <span class='accent'>PRO</span></h1>";
  page += "<p>";
  page += intro;
  page += "</p>";

  if (message.length() > 0) {
    page += "<div class='msg'>";
    page += message;
    page += "</div>";
  }

  page += "<form method='POST' action='/save' autocomplete='off'>";

  if (showWiFi) {
    page += "<label>";
    page += english ? "Wi-Fi network" : "Nome rete Wi-Fi";
    page += "</label>";
    page += "<input name='ssid' maxlength='32' required value='";
    page += _ssid;
    page += "'>";

    page += "<label>Password</label>";
    page += "<input name='password' type='password' maxlength='64'>";
  }

  if (showLocation) {
    const AuroraLocationSettings location =
      _settings.loadLocation();

    page += "<label>";
    page += english ? "Location" : "Localita";
    page += "</label>";
    page += "<input name='city' maxlength='80' required value='";
    page += location.name;
    page += "'>";
  }

  page += "<button type='submit'>";
  page += english ? "Save and restart" : "Salva e riavvia";
  page += "</button>";

  page += "</form></div></body></html>";
  return page;
}
