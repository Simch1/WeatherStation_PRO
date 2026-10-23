#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <FS.h>

// Alcune configurazioni di TFT_eSPI definiscono FS_NO_GLOBALS.
// In quel caso FS.h espone soltanto fs::FS, mentre WebServer.h
// della versione ESP32 3.3.10 usa ancora il nome globale FS.
// Questa dichiarazione rende esplicitamente disponibile il tipo.
using fs::FS;

#include <WebServer.h>
#include <DNSServer.h>
#include "AuroraSettings.h"
#include "AuroraLanguage.h"

enum AuroraPortalMode : uint8_t {
  PORTAL_WIFI_AND_LOCATION = 0,
  PORTAL_WIFI_ONLY = 1,
  PORTAL_LOCATION_ONLY = 2
};

class AuroraNetwork {
 public:
  AuroraNetwork(
    AuroraSettings& settings
  );

  void begin();
  bool hasCredentials() const;
  bool connect(unsigned long timeoutMs);
  void update();

  bool isConnected() const;
  int rssi() const;
  String ipAddress() const;
  String ssid() const;

  void startPortal(
    AuroraPortalMode mode = PORTAL_WIFI_AND_LOCATION
  );
  void cancelPortal();
  void restartPortal();

  bool isPortalActive() const;
  bool restartRequested() const;
  bool consumePortalClosed();

  unsigned long portalSecondsRemaining() const;
  uint8_t portalClientCount() const;
  AuroraPortalMode portalMode() const;

 private:
  AuroraSettings& _settings;
  WebServer _server;
  DNSServer _dns;

  String _ssid;
  String _password;

  bool _portalActive;
  bool _restartRequested;
  bool _portalClosedEvent;

  unsigned long _lastReconnectAttempt;
  unsigned long _portalStartedAt;
  AuroraPortalMode _portalMode;

  void handleRoot();
  void handleSave();
  void handleNotFound();
  String portalPage(const String& message = "");
};
