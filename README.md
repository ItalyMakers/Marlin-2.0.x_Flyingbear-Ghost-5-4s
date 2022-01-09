_development_

# Adattamento marlin 2.0.x Flyingbear Ghost 4s e 5 by Italy Makers

Adattamento a cura di Italy Makers del firmware Mks-Robin-Nano-Marlin2.0-Firmware per le stampanti Flying Bear Ghost 4s e 5

***La scheda madre Robin Nano v1.3-S potrebbe riscontrare problemi sul bootloader***

## _*firmware in fase di test*_

![Italy Makers Firmware](docs/bmp_logo.png)


###### **Versione** 3.1.2
###### Feature
- versione Marlin 2.0.9.2
- adattamento MKS Robin Nano Marlin 2.0
- iconografia ristrutturata sulla base delle icone di [majurca/MKS-TFT28-NEW-PICTURES](https://github.com/majurca/MKS-TFT28-NEW-PICTURES)
- modifica di colori e font sulla base della grafica
- adattamento delle schermate sulla base della nuova grafica
- modifica, bugfix e refactoring per schermate interfaccia
- gestione e ottimizzazione buffer e schermate per Ghost 5 e 4S
- adattamento interfacce e funzionalità per i requisiti della stampante
- settaggio di tutte le variabili e configurazioni su caratterische delle stampanti Ghost 5 e 4S
- creazione di file semplificato di gestione configurazioni
- introduzione gestione BLTouch su versione 2.0.9.2
- inserimento e gestione Mesh Bed Leveling su versione 2.0.9.2

###### Bugfix
- bugfix gestioni bltouch e per stampa
- bugfix runout sensor
- gestione sensore filamento su robin nano v1.x
- microfix presenti per gestione con bltouch e per Mesh Bed Leveling

---

## Personalizzazioni necessarie

Per comodità i parametri più comuni da settare sono presenti nel file `Marlin/FlyingBearGhost_Custom_config.h`
i parametri presenti:
- `FBGHOST_MOTHERBOARD` scheda madre. **se modificata modificare anche il file `platformio.ini`**
- Printer: scegliere se usare la configurazione 
  - `FBGHOST_IS_5` per la Ghost 5
  - `FBGHOST_IS_4S` per la Ghost 4S
- BLtouch
- drivers: è possibile scegliere se settare driver e direzioni custom o usare una tra le configurazioni pre-settate:
  - `FBGHOST_DRIVER_ALL_A4988`
  - `FBGHOST_DRIVER_ALL_TMC2208`
  - `FBGHOST_DRIVER_ALL_TMC2209`
  - `FBGHOST_DRIVER_XY_TMC2208_ZE_A4988` TMC2208 su assi X e Y e A4988 su Z ed E (conf standard delle prime Ghost 5)
- Step/mm configurazione che varia per tutte le stampanti
- parametri custom che cambiano con o senza BLTOUCH
  - **con BLtouch**
    - punti della griglia per X e Y
    - centro per homing Z
    - heating pre leveling (sconsiglato, non è visibile il progress sullo schermo, pare che la macchina sia bloccata)
  - **senza BLtouch**
    - Mesh Bed Leveling
    - inset Mesh Bed Leveling, quando rientrare rispetto ai bordi
    - punti della griglia per X e Y del Mesh Bel Leveling
    - start dell'asse Z nei punti di misurazione
    - centro per homing Z
- Nozzle-to-Probe offsets posizione del punto di misurazione rispetto al Nozzle
- Margini per effettuare la prova rispetto agli estremi del piatto
- `FBGHOST_FILAMENT_RUNOUT_DISTANCE_MM` distanza dopo la quale la stampante va in pausa quando non più sentito il filamento
- `FBGHOST_EXTRUDE_MINTEMP` temperatura sotto la quale la stampante non muove il filamento
- 2 preset per materiali
- settaggi del Pid
- settaggi JERK
- distanza massima di estrusione
- massimo feedrate in mm/s per tutti i motori
- massima accelerazione per tutti gli assi
- pin per LED aggiunta dei led su scheda madre tramite mosfet (per i NeoPixel usare i file standard di configurazione)
- valori di default di accelerazione
- Linear Pressure Control migliora le stampe ma ha problemi in bowden.
- debug


Tutte le altre configurazioni sono settabili dai file `Marlin/Configuration.h` e `Marlin/Configuration_adv.h`

Credits:
fork from [Makerbase Robin Nano firmware](https://github.com/makerbase-mks/Mks-Robin-Nano-Marlin2.0-Firmware)
Italy Makers Flyingbear Ghost Group: [Italy Makers - Flying Bear Ghost](https://www.facebook.com/groups/907067056500590)



