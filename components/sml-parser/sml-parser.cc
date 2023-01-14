#include "sml-parser.hh"

#include "esp_log.h"


#define MAX_STR_MANUF 5
unsigned char manuf[MAX_STR_MANUF];
double T1Wh = -2, SumWh = -2;

void SMLParser::Manufacturer() { smlOBISManufacturer(manuf, MAX_STR_MANUF); }

void SMLParser::PowerT1() { smlOBISWh(T1Wh); }

void SMLParser::PowerSum() { smlOBISWh(SumWh); }

void SMLParser::readBuffer(const unsigned char* input, size_t size) {
    for (int i = 0; i < size; ++i) {
        readByte(input[i]);
    }
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
  }
}
