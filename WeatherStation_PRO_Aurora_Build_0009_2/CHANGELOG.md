# Changelog / Registro modifiche

## Build 0007.2 - Home Open Layout and Hardware Button

### Italiano

- Aggiunto supporto al pulsante fisico su `IO32`, con collegamento consigliato tra `IO32` e `GND`.
- Il pulsante riattiva lo schermo quando la retroilluminazione e' in standby.
- A schermo acceso, il pulsante cambia le pagine principali in sequenza: Home, Grafici, Meteo, More.
- Dentro Diagnostica, il pulsante scorre le 6 videate diagnostiche senza uscire dal menu.
- La gestione del pulsante e' disattivabile/modificabile da `AuroraConfig.h` tramite `AURORA_BUTTON_PIN`, `AURORA_BUTTON_ACTIVE_LOW` e `AURORA_BUTTON_DEBOUNCE_MS`.
- Home aggiornata con area meteo principale senza riquadratura celeste esterna.
- Ridistribuiti gli spazi della Home: icona meteo piu' a sinistra, temperatura e testi meteo leggermente spostati a sinistra, alba e tramonto in colonne equivalenti.
- Mantenuta la riga dati inferiore con la stessa altezza e con unita' complete: `C`, `%`, `km/h`, `hPa`.

### English

- Added physical button support on `IO32`, with recommended wiring between `IO32` and `GND`.
- The button wakes the screen when the backlight is in standby.
- When the screen is on, the button cycles through the main pages: Home, Charts, Weather, More.
- Inside Diagnostics, the button cycles through the 6 diagnostics pages without leaving the menu.
- Button handling can be disabled/changed in `AuroraConfig.h` through `AURORA_BUTTON_PIN`, `AURORA_BUTTON_ACTIVE_LOW`, and `AURORA_BUTTON_DEBOUNCE_MS`.
- Home updated with no cyan outer frame around the main weather area.
- Home spacing refined: weather icon moved left, temperature and weather text slightly moved left, sunrise and sunset arranged in equal columns.
- Bottom data row height preserved with complete units: `C`, `%`, `km/h`, `hPa`.

## Build 0007.2 Alpha - Final UI Pass

### Italiano

- Home ridisegnata come dashboard compatta per TFT 320x240.
- Mantenuta l'altezza della riga dati inferiori, ripristinando tutte le unita': `C`, `%`, `km/h`, `hPa`.
- Allineato `PROSSIMO AGG.` su una sola riga con valore minuti.
- Barra navigazione uniformata con quattro pulsanti bilanciati.
- Diagnostica ridisegnata in 6 pagine con layout professionale, badge stato, righe dati e footer comune.
- Portale `WeatherStation_Setup` reso aperto anche per cambio Wi-Fi, cambio router o cambio gestore.
- Schermata istruzioni portale aggiornata con stato `APERTA` / `OPEN`.
- Descrizioni meteo bilingue tramite `AuroraWeatherDescriptions`.

### English

- Home redesigned as a compact dashboard for 320x240 TFT displays.
- Bottom data row height preserved while restoring all units: `C`, `%`, `km/h`, `hPa`.
- `NEXT UPDATE` aligned on a single row with remaining minutes.
- Navigation bar normalized with four balanced buttons.
- Diagnostics redesigned into 6 professional pages with status badge, data rows, and shared footer.
- `WeatherStation_Setup` portal made open also for Wi-Fi changes, router changes, or internet provider changes.
- Portal instruction page updated with `APERTA` / `OPEN`.
- Bilingual weather descriptions via `AuroraWeatherDescriptions`.

## Build 0007.0 - 0007.1 Alpha

### Italiano

- Prima integrazione Aurora UI 2.0 sulla Home.
- Nuovi componenti visuali: Hero Card, tile dati, footer Home.
- Migliorato allineamento di temperatura, alba, tramonto e pulsanti.

### English

- First Aurora UI 2.0 integration on the Home page.
- New visual components: Hero Card, data tiles, Home footer.
- Improved alignment for temperature, sunrise, sunset, and buttons.

## Build 0006.x

### Italiano

- Aggiunte impostazioni avanzate, diagnostica, event log e gestione display.
- Separati cambio Wi-Fi e cambio localita'.
- Factory reset esteso alla cancellazione credenziali Wi-Fi.

### English

- Added advanced settings, diagnostics, event log, and display handling.
- Separated Wi-Fi change and location change flows.
- Factory reset extended to remove saved Wi-Fi credentials.
