# ESP32-SmartMeter
# Copyright (C) 2022 Christian Heusel
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.


idf_component_register(SRCS "esp32-smartMeter.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES wifi-connection
                                mqtt-connection
                                uart-communication
                                sml-parser
                                esp_timer)
