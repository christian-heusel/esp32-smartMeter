#include "sml-parser.hh"

#include "esp_log.h"
#include <stdio.h>

unsigned char manuf[5];
double T1Wh = -2, SumWh = -2, TotalW = -2, L1W = -2.0 , L2W = -2.0, L3W = -2.0;

SMLParser::SMLParser(MQTTClient* mqttClient)
:
    mqttClient(mqttClient)
{};

void SMLParser::Manufacturer() { smlOBISManufacturer(manuf, 5); }

void SMLParser::PowerT1() { smlOBISWh(T1Wh); }

void SMLParser::PowerSum() { smlOBISWh(SumWh); }

void SMLParser::PowerTotal() { smlOBISW(TotalW); }
void SMLParser::PowerL1() { smlOBISW(L1W); }
void SMLParser::PowerL2() { smlOBISW(L2W); }
void SMLParser::PowerL3() { smlOBISW(L3W); }

void SMLParser::readBufferCallbackWrapper(void* sml_parser, const unsigned char* input, size_t size) {
    auto self = static_cast<SMLParser*>(sml_parser);
    self->readBuffer(input, size);
}

void SMLParser::readBuffer(const unsigned char* input, size_t size) {
    for (int i = 0; i < size; ++i) {
        readByte(input[i]);
    }
}

void SMLParser::printSMLValues()
{
    sprintf(buffer, "Msg..: %lu", counter);
    ESP_LOGI(SML_PARSER_LOG_TAG, "Message: %s", buffer);
    sprintf(buffer, "%s", manuf);
    ESP_LOGI(SML_PARSER_LOG_TAG, "Manufacturer: %s", buffer);

    // dtostrf(T1Wh, 10, 1, floatBuffer);
    sprintf(buffer, "%f Wh", T1Wh);
    ESP_LOGI(SML_PARSER_LOG_TAG, "T1.: %s", buffer);

    // dtostrf(SumWh, 10, 1, floatBuffer);
    sprintf(buffer, "%f Wh", SumWh);
    ESP_LOGI(SML_PARSER_LOG_TAG, "Sum.: %s", buffer);

    sprintf(buffer, "%f W", TotalW);
    ESP_LOGI(SML_PARSER_LOG_TAG, "total.: %s", buffer);
}

void SMLParser::sendSMLValues()
{
    auto size = sprintf(buffer, "%f", T1Wh);
    ESP_LOGI(SML_PARSER_LOG_TAG, "%s,%d", buffer, size);
    auto str = std::string_view(buffer, size);
    mqttClient->publish("t1", str);

    size = sprintf(buffer, "%f", SumWh);
    str = std::string_view(buffer, size);
    ESP_LOGI(SML_PARSER_LOG_TAG, "%s,%d", buffer, size);
    mqttClient->publish("sum", str);

    size = sprintf(buffer, "%f", TotalW);
    str = std::string_view(buffer, size);
    ESP_LOGI(SML_PARSER_LOG_TAG, "%s,%d", buffer, size);
    mqttClient->publish("total", str);
}

void SMLParser::readByte(unsigned char inputChar)
{
  unsigned int iHandler = 0;
  currentState = smlState(inputChar);
  if (currentState == SML_START) {
    /* reset local vars */
    manuf[0] = 0;
    T1Wh = -3.0;
    SumWh = -3.0;
    TotalW =-3.0;
    L1W = -3.0;
    L2W = -3.0;
    L3W = -3.0;
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
