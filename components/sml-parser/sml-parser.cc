#include "sml-parser.hh"

#include "esp_log.h"
#include <stdio.h>

unsigned char manuf[5];
double T1Wh = -2, SumWh = -2;

SMLParser::SMLParser(MQTTClient* mqttClient)
:
    mqttClient(mqttClient)
{};

void SMLParser::Manufacturer() { smlOBISManufacturer(manuf, 5); }

void SMLParser::PowerT1() { smlOBISWh(T1Wh); }

void SMLParser::PowerSum() { smlOBISWh(SumWh); }

void SMLParser::readBuffer(const unsigned char* input, size_t size) {
    for (int i = 0; i < size; ++i) {
        readByte(input[i]);
    }
}

void SMLParser::printSMLValues()
{
    sprintf(buffer, "Msg..: %lu", counter);
    ESP_LOGI(SML_PARSER_LOG_TAG, "Message: %s", buffer);
    sprintf(buffer, "Manuf: %s", manuf);
    ESP_LOGI(SML_PARSER_LOG_TAG, "Manufacturer: %s", buffer);

    // dtostrf(T1Wh, 10, 1, floatBuffer);
    sprintf(buffer, "T1.: %f", T1Wh);
    ESP_LOGI(SML_PARSER_LOG_TAG, "T1.: %s", buffer);

    // dtostrf(SumWh, 10, 1, floatBuffer);
    sprintf(buffer, "Sum: %f", SumWh);
    ESP_LOGI(SML_PARSER_LOG_TAG, "Sum.: %s", buffer);
}

void SMLParser::sendSMLValues()
{
    auto size = sprintf(buffer, "%f", T1Wh);
    mqttClient->publish("t1", std::string_view(buffer, size));

    sprintf(buffer, "%f", SumWh);
    mqttClient->publish("sum", std::string_view(buffer, size));
}

void SMLParser::readByte(unsigned char inputChar)
{
  unsigned int iHandler = 0;
  currentState = smlState(inputChar);
  if (currentState == SML_START) {
    /* reset local vars */
    manuf[0] = 0;
    T1Wh = -3;
    SumWh = -3;
  }
  if (currentState == SML_LISTEND) {
    /* check handlers on last received list */
    for (iHandler = 0; OBISHandlers[iHandler].Handler != 0 &&
                       !(smlOBISCheck(OBISHandlers[iHandler].OBIS));
         iHandler++)
      ;
    if (OBISHandlers[iHandler].Handler != 0) {
      OBISHandlers[iHandler].Handler();
    }
  }
  if (currentState == SML_UNEXPECTED) {
    ESP_LOGE(SML_PARSER_LOG_TAG, "Unexpected byte");
  }
  if (currentState == SML_FINAL) {
    counter++;
    ESP_LOGI(SML_PARSER_LOG_TAG, "Successfully received a complete message");
    printSMLValues();
    sendSMLValues();
  }
}
