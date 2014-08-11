#include <pebble.h>
#include "time_layer.h"

void update_time() {
    //Get a tm structure
    time_t temp = time(NULL);
    struct tm *time_tick = localtime(&temp);
    
    //Create a long-lived buffer
    char buffer[] = "00:00";
    
    //Write the current hours and minutes into the buffer
    if(clock_is_24h_style() == true) {
        //Use 24h format
        strftime(buffer, sizeof("00:00"), "%H:%M", time_tick);
    } else {
        //User 12H format
        strftime(buffer, sizeof("00:00"), "%I:%M", time_tick);
    }
    
    //Display this time on the TextLayer
    text_layer_set_text(s_time_layer, buffer);
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
    
    //Get weather update every 30 mintues
    if (tick_time->tm_min % 30 == 0) {
        //Begin dictionary
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        
        //Add a key-value pair
        dict_write_uint8(iter, 0, 0);
        
        //Send the message
        app_message_outbox_send();
    }
}