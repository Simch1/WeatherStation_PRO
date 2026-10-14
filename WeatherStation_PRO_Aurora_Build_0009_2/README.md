# WeatherStation PRO / Aurora Build 0007.2

## Italiano

WeatherStation PRO e' una stazione meteo touch basata su ESP32 e display TFT 320x240. La build Aurora integra interfaccia multilingua, portale Wi-Fi, ricerca localita', dati meteo Open-Meteo, Home dashboard, previsioni, grafici, impostazioni, diagnostica e pulsante fisico di navigazione.

### Funzioni principali

- Home dashboard con temperatura, descrizione meteo, percepita, alba, tramonto, min/max, umidita', vento e pressione.
- Home aggiornata con area meteo principale senza cornice esterna, spazi ridistribuiti e alba/tramonto centrati in due colonne equivalenti.
- Previsioni 5 giorni e dettaglio orario.
- Grafico temperature quando i dati orari sono disponibili.
- Portale Wi-Fi captive aperto per prima configurazione, cambio router/gestore e cambio localita'.
- Lingua selezionabile: Italiano / English.
- Impostazioni display, tema, luminosita' e standby.
- Diagnostica a 6 pagine: Sistema, Rete, Meteo, Display, Stato sistema, Event Log.
- Pulsante fisico su `IO32`: risveglia lo schermo, cambia pagina Home/Grafici/Meteo/More e scorre le pagine di Diagnostica.
- Factory reset completo con cancellazione impostazioni e credenziali Wi-Fi.

### Pulsante fisico

Il firmware e' configurato per un pulsante normalmente aperto collegato tra `IO32` e `GND`.

Schema:

```text
IO32 ---- pulsante NO ---- GND
```

Il pin usa logica `active low`: quando il pulsante viene premuto, `IO32` viene portato a massa. Il codice usa il pull-up interno della ESP32, quindi non e' obbligatoria una resistenza esterna. Una resistenza da 10k verso `3V3` puo' comunque essere usata come pull-up esterno.

Comportamento:

- con schermo in standby, la prima pressione riattiva la retroilluminazione;
- con schermo acceso, le pressioni successive cambiano pagina: Home, Grafici, Meteo, More;
- dentro Diagnostica, il pulsante scorre le 6 videate diagnostiche;
- durante il portale Wi-Fi, il pulsante non cambia pagina per non disturbare la configurazione.

### Librerie richieste

- ESP32 core 3.3.x.
- TFT_eSPI configurata per il display usato.
- ArduinoJson 7.x.

### Installazione

1. Aprire `WeatherStation_PRO_Aurora_Build_0007_2.ino` con Arduino IDE.
2. Selezionare una scheda ESP32 compatibile, ad esempio `ESP32 Dev Module`.
3. Verificare che TFT_eSPI sia configurata per pin, driver e touch del display.
4. Compilare e caricare.
5. Alla prima accensione collegarsi alla rete `WeatherStation_Setup`.
6. Aprire `192.168.4.1` e inserire Wi-Fi e localita'.

### Nota Wi-Fi

Il portale `WeatherStation_Setup` e' aperto, senza password. Questa scelta evita blocchi quando si cambia router, gestore ADSL/fibra o rete domestica.

---

## English

WeatherStation PRO is an ESP32 touch weather station for a 320x240 TFT display. The Aurora build includes multilingual UI, Wi-Fi captive portal, location lookup, Open-Meteo weather data, dashboard Home, forecasts, charts, settings, diagnostics, and a physical navigation button.

### Main features

- Dashboard Home with temperature, weather description, feels-like temperature, sunrise, sunset, min/max, humidity, wind, and pressure.
- Updated Home layout with no outer frame around the main weather area, improved spacing, and sunrise/sunset centered in two equal columns.
- 5-day forecast and hourly detail.
- Temperature chart when hourly data is available.
- Open Wi-Fi captive portal for first setup, router/provider changes, and location changes.
- Selectable language: Italiano / English.
- Display, theme, brightness, and standby settings.
- 6-page diagnostics: System, Network, Weather, Display, System Status, Event Log.
- Physical button on `IO32`: wakes the screen, switches Home/Charts/Weather/More, and cycles Diagnostics pages.
- Full factory reset, including settings and saved Wi-Fi credentials.

### Physical button

The firmware is configured for a normally open button connected between `IO32` and `GND`.

Wiring:

```text
IO32 ---- normally open button ---- GND
```

The pin uses `active low` logic: when the button is pressed, `IO32` is connected to ground. The ESP32 internal pull-up is used, so an external resistor is not required. A 10k resistor to `3V3` can still be used as an external pull-up.

Behavior:

- when the screen is in standby, the first press wakes the backlight;
- when the screen is already on, each press switches page: Home, Charts, Weather, More;
- inside Diagnostics, the button cycles through the 6 diagnostics pages;
- while the Wi-Fi portal is active, the button does not switch pages, so setup is not interrupted.

### Required libraries

- ESP32 core 3.3.x.
- TFT_eSPI configured for the target display.
- ArduinoJson 7.x.

### Installation

1. Open `WeatherStation_PRO_Aurora_Build_0007_2.ino` in Arduino IDE.
2. Select a compatible ESP32 board, for example `ESP32 Dev Module`.
3. Check that TFT_eSPI is configured for the display driver, pins, and touch controller.
4. Compile and upload.
5. On first boot, connect to `WeatherStation_Setup`.
6. Open `192.168.4.1` and enter Wi-Fi and location data.

### Wi-Fi note

The `WeatherStation_Setup` portal is open and does not require a password. This prevents lockouts when changing router, internet provider, or home network.
