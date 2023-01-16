#ifndef SML_PARSER_HH
#define SML_PARSER_HH

#include "sml.h"
#include "mqtt.hh"
#include <cstddef>

const char* const SML_PARSER_LOG_TAG = "SML";

class SMLParser {

public:

    SMLParser(MQTTClient* mqttClient);

    void readByte(unsigned char inputChar);
    void readBuffer(const unsigned char* input, size_t size);
    void printSMLValues();
    void sendSMLValues();

    static void readBufferCallbackWrapper(void* sml_parser, const unsigned char* input, size_t size);

private:
    MQTTClient* mqttClient;
    sml_states_t currentState;
    unsigned long counter = 0;
    char buffer[50];
    char floatBuffer[20];

    static void Manufacturer();
    static void PowerT1();
    static void PowerSum();

    typedef struct {
      const unsigned char OBIS[6];
      void (*Handler)();
    } OBISHandler;

    // clang-format off
    OBISHandler OBISHandlers[4] = {
        {{0x81, 0x81, 0xc7, 0x82, 0x03, 0xff}, &SMLParser::Manufacturer}, /* 129-129:199.130.3*255 */
        {{0x01, 0x00, 0x01, 0x08, 0x01, 0xff}, &SMLParser::PowerT1},      /*   1-  0:  1.  8.1*255 (T1) */
        {{0x01, 0x00, 0x01, 0x08, 0x00, 0xff}, &SMLParser::PowerSum},     /*   1-  0:  1.  8.0*255 (T1 + T2) */
        {{0}, 0}
    };
    // clang-format on

};

#endif /* ifndef SML_PARSER_HH */
