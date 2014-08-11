#include <pebble.h>
#include "weather_layer.h"

void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    //Store incoming information
    char temperature_buffer[8];
    char conditions_buffer[32];
    char weather_layer_buffer[32];
    
    //Read first item
    Tuple *t = dict_read_first(iterator);
    
    //For all items
    while (t != NULL) {
        //Which key was received?
        switch (t->key) {
            case KEY_TEMPERATURE:
                snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", (int) t->value->int32);
                break;
            case KEY_CONDITIONS:
                snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
                break;
            default:
                APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int) t->key);
                break;
        }
        
        //Look for next item
        t = dict_read_next(iterator);
    }
    
    //Assemble full string and display
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", temperature_buffer, conditions_buffer);
    text_layer_set_text(s_weather_layer, weather_layer_buffer);
}

void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}