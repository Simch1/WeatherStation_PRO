# Bug Log / Registro bug

## Italiano

Questo file raccoglie i problemi importanti emersi durante lo sviluppo e lo stato della loro correzione.

| ID | Area | Stato | Descrizione | Correzione |
| --- | --- | --- | --- | --- |
| BUG-008 | Boot | Corretto | Riavvio continuo nella Build 0005. | Caricamento differito impostazioni display tramite `beginSafeState()`. |
| BUG-009 | Meteo | Corretto | Guru Meditation durante download Open-Meteo. | Parsing JSON in streaming con filtro ArduinoJson. |
| BUG-010 | Compatibilita' ESP32 | Corretto | `setBufferSizes()` non disponibile con core ESP32 recente. | Chiamata rimossa. |
| BUG-011 | Home | Corretto | Crash durante rendering temperatura percepita. | Corretto formato testo/valore. |
| BUG-012 | Impostazioni | Corretto | Cambio Wi-Fi e localita' non separati. | Aggiunta pagina Rete e Localita'. |
| BUG-015 | Diagnostica | Corretto | Impossibile uscire dalla Diagnostica. | Footer diagnostica con MENU, precedente e successiva. |
| BUG-016 | Navigazione | Corretto | Loop tra Impostazioni e Diagnostica. | Pulsante HOME/MENU esplicito. |
| BUG-022 | Reset | Corretto | Factory reset non cancellava la vecchia rete. | Cancellazione Preferences e credenziali Wi-Fi dello stack ESP32. |
| BUG-023 | Localita' | Corretto | Cambio localita' richiedeva di nuovo la password Wi-Fi. | Modalita' `PORTAL_LOCATION_ONLY`. |
| BUG-024 | Portale | Corretto | Cambio localita' chiedeva password del portale. | AP aperto per cambio localita'. |
| BUG-025 | Portale Wi-Fi | Corretto | Dopo cambio gestore/router il portale chiedeva password. | `WeatherStation_Setup` reso aperto anche per cambio Wi-Fi e prima configurazione. |
| HW-001 | Pulsante fisico | Corretto | Serviva un comando fisico per riattivare lo schermo e cambiare pagina senza usare il touch. | Aggiunto pulsante su `IO32`, con risveglio standby e navigazione pagine. |
| HW-002 | Diagnostica | Corretto | Il pulsante fisico chiudeva la Diagnostica invece di scorrere le sue videate. | Dentro `OVERLAY_DIAGNOSTICS`, il pulsante avanza tra le 6 pagine diagnostiche. |
| UI-001 | Home | Corretto | Home troppo compressa e poco professionale. | Layout dashboard Aurora UI 2.0. |
| UI-002 | Home | Corretto | Tile inferiori tagliavano unita' o valori. | Riallineamento interno mantenendo stessa altezza. |
| UI-003 | Diagnostica | Corretto | Sei pagine diagnostiche poco uniformi. | Shell diagnostica comune con badge, righe e footer condiviso. |
| UI-004 | Home | Corretto | Il riquadro celeste principale appesantiva la Home e limitava la distribuzione degli spazi. | Rimossa la cornice esterna e ridistribuiti meteo, prossimo aggiornamento, alba e tramonto. |

### Note aperte

- HTTPS usa `client.setInsecure()`: funziona, ma non verifica il certificato server.
- La flash e' circa al 92%, quindi nuove funzioni pesanti vanno valutate con attenzione.
- Alcune stringhe molto lunghe in inglese possono richiedere abbreviazioni sul TFT 320x240.
- L'upload puo' fallire con `COM busy` se Monitor Seriale, Arduino IDE o un altro processo stanno usando la porta.

---

## English

This file tracks the important issues found during development and their fix status.

| ID | Area | Status | Description | Fix |
| --- | --- | --- | --- | --- |
| BUG-008 | Boot | Fixed | Continuous reboot in Build 0005. | Deferred display settings loading through `beginSafeState()`. |
| BUG-009 | Weather | Fixed | Guru Meditation during Open-Meteo download. | Streaming JSON parsing with ArduinoJson filter. |
| BUG-010 | ESP32 compatibility | Fixed | `setBufferSizes()` unavailable with recent ESP32 core. | Call removed. |
| BUG-011 | Home | Fixed | Crash while rendering feels-like temperature. | Fixed text/value formatting. |
| BUG-012 | Settings | Fixed | Wi-Fi and location changes were not separated. | Added Network and Location page. |
| BUG-015 | Diagnostics | Fixed | Could not exit Diagnostics. | Diagnostics footer with MENU, previous, and next. |
| BUG-016 | Navigation | Fixed | Loop between Settings and Diagnostics. | Explicit HOME/MENU button. |
| BUG-022 | Reset | Fixed | Factory reset did not forget old Wi-Fi network. | Cleared Preferences and ESP32 Wi-Fi stack credentials. |
| BUG-023 | Location | Fixed | Location change asked for Wi-Fi password again. | Added `PORTAL_LOCATION_ONLY`. |
| BUG-024 | Portal | Fixed | Location change asked for portal password. | Open AP for location changes. |
| BUG-025 | Wi-Fi portal | Fixed | After changing router/provider, the portal asked for a password. | `WeatherStation_Setup` made open also for Wi-Fi change and first setup. |
| HW-001 | Physical button | Fixed | A physical command was needed to wake the screen and change page without using touch. | Added button on `IO32`, with standby wake and page navigation. |
| HW-002 | Diagnostics | Fixed | The physical button closed Diagnostics instead of cycling through its views. | Inside `OVERLAY_DIAGNOSTICS`, the button advances through the 6 diagnostics pages. |
| UI-001 | Home | Fixed | Home was too compressed and not polished enough. | Aurora UI 2.0 dashboard layout. |
| UI-002 | Home | Fixed | Bottom tiles clipped units or values. | Internal realignment while preserving row height. |
| UI-003 | Diagnostics | Fixed | Six diagnostics pages were visually inconsistent. | Shared diagnostics shell with badge, rows, and footer. |
| UI-004 | Home | Fixed | The main cyan frame made Home visually heavier and limited spacing. | Removed the outer frame and redistributed weather, next update, sunrise, and sunset. |

### Open notes

- HTTPS uses `client.setInsecure()`: it works, but it does not validate the server certificate.
- Flash usage is around 92%, so large new features should be evaluated carefully.
- Some long English strings may require abbreviations on the 320x240 TFT.
- Upload can fail with `COM busy` if Serial Monitor, Arduino IDE, or another process is using the port.
