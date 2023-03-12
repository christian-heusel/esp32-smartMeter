# ESP32 SmartMeter

This project reads out the meter values of an [ISKRA MT681](https://wiki.volkszaehler.org/hardware/channels/meters/power/edl-ehz/iskraemeco_mt681) via an [IR reader](https://wiki.volkszaehler.org/hardware/controllers/ir-schreib-lesekopf). It then takes these values and sends them as payload to a MQTT-Broker, where they get converted to a prometheus metric via [`kpetremann/mqtt-exporter`](https://github.com/kpetremann/mqtt-exporter) and then read into prometheus.

## Installation

1. Install the [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html#manual-installation)

2. Clone this repository:
   ```
   $ git clone git@github.com:christian-heusel/esp32-smartMeter.git
   $ cd esp32-smartMeter
   ```

3. Configure the project
   ```
   $ idf.py menuconfig
   ```
   And especially insert sensible values into the MQTT & Wifi Configuration

4. Build it & flash it
   ```
   $ idf.py build
   $ idf.py flash
   ```
   
## Visualization

Import the grafana dashboard in [contrib/grafana-dashboard.json](https://github.com/christian-heusel/esp32-smartMeter/blob/main/contrib/grafana-dashboard.json)

![image](https://user-images.githubusercontent.com/26827864/224534926-38ba97d7-14db-4c0a-b674-8eaadf1df6d5.png)


## License & Acknowledgements

The contents of this repository are heavily based on the great examples in the ESP IDF:
https://github.com/espressif/esp-idf/tree/master/examples

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or any later version.
