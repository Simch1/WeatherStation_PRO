# Test Checklist / Checklist di test

## Italiano

### Compilazione e caricamento

- [ ] Compilazione completata senza errori.
- [ ] Upload completato sulla porta corretta.
- [ ] Avvio con checkpoint seriali fino a completamento.
- [ ] Nessun reset spontaneo dopo il caricamento.
- [ ] Se l'upload fallisce con porta occupata, chiudere Monitor Seriale/Arduino IDE, scollegare e ricollegare USB, poi riselezionare la COM corretta.

### Pulsante fisico

- [ ] Pulsante normalmente aperto collegato tra `IO32` e `GND`.
- [ ] `AuroraConfig.h` contiene `AURORA_BUTTON_PIN = 32`.
- [ ] A schermo spento, una pressione riattiva la retroilluminazione e ridisegna la pagina corrente.
- [ ] A schermo acceso, il pulsante cambia pagina in sequenza: Home, Grafici, Meteo, More, Home.
- [ ] Durante il portale Wi-Fi, il pulsante non cambia pagina.
- [ ] Dentro Diagnostica, il pulsante scorre tutte le 6 videate diagnostiche.
- [ ] Il pulsante non genera doppi click evidenti grazie al debounce.

### Home

- [ ] Ora, localita' e Wi-Fi leggibili nell'header.
- [ ] Area meteo principale senza cornice celeste esterna, ma leggibile e ordinata.
- [ ] Icona meteo grande spostata a sinistra senza uscire dallo schermo.
- [ ] Temperatura, simbolo grado e `C` allineati.
- [ ] Temperatura, descrizione meteo e percepita leggermente a sinistra e non sovrapposte ad alba/tramonto.
- [ ] Descrizione meteo leggibile in italiano e inglese.
- [ ] `Percepita` leggibile e non sovrapposta alle linee verticali.
- [ ] `PROSSIMO AGG.` e minuti su una sola riga.
- [ ] Alba e tramonto centrati in colonne con spazi uguali.
- [ ] Min/Max, Umidita', Vento e Pressione completi con unita'.
- [ ] I quattro pulsanti di navigazione sono leggibili e touch funzionante.

### Portale Wi-Fi

- [ ] Senza credenziali salvate viene aperto `WeatherStation_Setup`.
- [ ] Il telefono entra nel portale senza password.
- [ ] `192.168.4.1` mostra il form corretto.
- [ ] Salvataggio Wi-Fi e localita' riavvia la stazione.
- [ ] Cambio Wi-Fi da Impostazioni non richiede password del portale.
- [ ] Cambio localita' mantiene la rete domestica quando il Wi-Fi e' connesso.

### Diagnostica

- [ ] Le 6 pagine diagnostiche sono navigabili con avanti/indietro.
- [ ] Sistema mostra firmware, build, uptime e memoria.
- [ ] Rete mostra stato, SSID, RSSI e IP.
- [ ] Meteo mostra provider, ultimo aggiornamento, localita' e coordinate.
- [ ] Display mostra risoluzione, profilo, luminosita' e retroilluminazione.
- [ ] Stato sistema mostra rete, meteo, memoria e uptime.
- [ ] Event Log mostra gli eventi o il messaggio di assenza eventi.

### Reset

- [ ] Factory reset cancella impostazioni Aurora.
- [ ] Factory reset cancella SSID e password Wi-Fi salvati.
- [ ] Dopo reset viene riproposto il portale di configurazione.

---

## English

### Compile and upload

- [ ] Build completes without errors.
- [ ] Upload completes on the correct serial port.
- [ ] Boot serial checkpoints reach completion.
- [ ] No unexpected reboot after upload.
- [ ] If upload fails because the port is busy, close Serial Monitor/Arduino IDE, unplug and reconnect USB, then select the correct COM port again.

### Physical button

- [ ] Normally open button connected between `IO32` and `GND`.
- [ ] `AuroraConfig.h` contains `AURORA_BUTTON_PIN = 32`.
- [ ] With the screen off, one press wakes the backlight and redraws the current page.
- [ ] With the screen on, the button cycles pages: Home, Charts, Weather, More, Home.
- [ ] While the Wi-Fi portal is active, the button does not change page.
- [ ] Inside Diagnostics, the button cycles through all 6 diagnostics pages.
- [ ] The button does not create obvious double clicks thanks to debounce.

### Home

- [ ] Time, location, and Wi-Fi are readable in the header.
- [ ] Main weather area has no cyan outer frame, while staying readable and organized.
- [ ] Large weather icon is moved left without leaving the screen.
- [ ] Temperature, degree symbol, and `C` are aligned.
- [ ] Temperature, weather description, and feels-like text are slightly left and do not overlap sunrise/sunset.
- [ ] Weather description is readable in Italian and English.
- [ ] `Feels like` is readable and does not overlap vertical lines.
- [ ] `NEXT UPDATE` and minutes stay on a single row.
- [ ] Sunrise and sunset are centered in equally spaced columns.
- [ ] Min/Max, Humidity, Wind, and Pressure show complete values with units.
- [ ] The four navigation buttons are readable and touch works.

### Wi-Fi portal

- [ ] With no saved credentials, `WeatherStation_Setup` is started.
- [ ] The phone can join the portal without a password.
- [ ] `192.168.4.1` shows the correct form.
- [ ] Saving Wi-Fi and location restarts the station.
- [ ] Wi-Fi change from Settings does not require a portal password.
- [ ] Location change keeps the home network connected when Wi-Fi is online.

### Diagnostics

- [ ] The 6 diagnostics pages can be navigated forward/backward.
- [ ] System shows firmware, build, uptime, and memory.
- [ ] Network shows status, SSID, RSSI, and IP.
- [ ] Weather shows provider, last update, location, and coordinates.
- [ ] Display shows resolution, profile, brightness, and backlight.
- [ ] System Status shows network, weather, memory, and uptime.
- [ ] Event Log shows events or the no-events message.

### Reset

- [ ] Factory reset clears Aurora settings.
- [ ] Factory reset clears saved Wi-Fi SSID and password.
- [ ] After reset, the setup portal is shown again.
