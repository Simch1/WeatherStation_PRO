# Localization / Localizzazione

## Italiano

Il progetto supporta due lingue:

- Italiano
- English

La lingua viene salvata nelle Preferences e ripristinata al riavvio.

### File coinvolti

- `AuroraLanguage.h`: enum delle lingue e degli ID testo.
- `AuroraLanguage.cpp`: tabelle testi e risoluzione `AuroraLanguage::text()`.
- `AuroraWeatherDescriptions.cpp`: descrizioni meteo bilingue per codice Open-Meteo.
- `AuroraPages.cpp`: usa sempre `_language.text(...)` per i testi dell'interfaccia quando possibile.

### Regole

- Evitare testi fissi nella logica, salvo sigle tecniche brevi (`WiFi`, `hPa`, `km/h`).
- Ogni nuovo testo UI deve avere una voce in `AuroraTextId`.
- Ogni nuova voce deve essere tradotta in italiano e inglese.
- Le etichette su TFT devono restare brevi per non uscire dai riquadri.
- Nella barra di navigazione usare etichette corte: `HOME`, `GRAF`, `METEO`, `MORE`.
- Nella Home, le stringhe meteo lunghe devono essere testate in entrambe le lingue per non sovrapporsi a temperatura, percepita, alba e tramonto.
- Le unita' tecniche restano uguali in entrambe le lingue: `C`, `%`, `km/h`, `hPa`.
- Il testo relativo al pulsante fisico puo' essere documentato nei file Markdown; sul TFT non serve aggiungere istruzioni visibili.

### Aggiungere una lingua

1. Aggiungere un valore a `AuroraLanguageId`.
2. Aggiungere una nuova tabella testi in `AuroraLanguage.cpp`.
3. Aggiornare `AuroraLanguage::text()`.
4. Aggiungere le descrizioni meteo in `AuroraWeatherDescriptions.cpp`.
5. Testare Home, Previsioni, Impostazioni, Portale e Diagnostica.

### Test linguistici consigliati

- Home in italiano con descrizioni come `Quasi sereno`, `Coperto`, `Temporale`.
- Home in inglese con descrizioni piu' lunghe, verificando che restino leggibili.
- Barra inferiore dati con unita' complete: min/max in `C`, umidita' in `%`, vento in `km/h`, pressione in `hPa`.
- Diagnostica in entrambe le lingue, controllando titoli, righe e footer.
- Portale Wi-Fi in entrambe le lingue, verificando lo stato `APERTA` / `OPEN`.

---

## English

The project supports two languages:

- Italiano
- English

The selected language is saved in Preferences and restored after reboot.

### Related files

- `AuroraLanguage.h`: language enum and text ID enum.
- `AuroraLanguage.cpp`: text tables and `AuroraLanguage::text()` resolution.
- `AuroraWeatherDescriptions.cpp`: bilingual Open-Meteo weather descriptions.
- `AuroraPages.cpp`: uses `_language.text(...)` for UI text whenever possible.

### Rules

- Avoid hardcoded UI text in logic, except short technical labels (`WiFi`, `hPa`, `km/h`).
- Every new UI text must have an `AuroraTextId` entry.
- Every new entry must be translated into Italian and English.
- TFT labels must stay short to avoid overflowing their containers.
- Use short navigation labels: `HOME`, `GRAF`, `METEO`, `MORE`.
- On Home, long weather descriptions must be tested in both languages so they do not overlap temperature, feels-like, sunrise, or sunset.
- Technical units stay the same in both languages: `C`, `%`, `km/h`, `hPa`.
- Physical button instructions can stay in Markdown documentation; no visible TFT instructions are required.

### Adding a language

1. Add a value to `AuroraLanguageId`.
2. Add a new text table in `AuroraLanguage.cpp`.
3. Update `AuroraLanguage::text()`.
4. Add weather descriptions in `AuroraWeatherDescriptions.cpp`.
5. Test Home, Forecast, Settings, Portal, and Diagnostics.

### Recommended language tests

- Home in Italian with descriptions such as `Quasi sereno`, `Coperto`, `Temporale`.
- Home in English with longer descriptions, checking that they remain readable.
- Bottom data row with complete units: min/max in `C`, humidity in `%`, wind in `km/h`, pressure in `hPa`.
- Diagnostics in both languages, checking titles, rows, and footer.
- Wi-Fi portal in both languages, checking the `APERTA` / `OPEN` status.
