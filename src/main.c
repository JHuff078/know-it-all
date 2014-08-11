#include <pebble.h>
    
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1

Window *s_main_window;

TextLayer *s_time_layer;
TextLayer *s_weather_layer;

GFont *s_time_font;
GFont *s_weather_font;

void update_time() {
    //Get a tm structure
    time_t temp = time(NULL);
    struct tm *time_tick = localtime(&temp);
    
    //Create a long-lived buffer
    static char buffer[] = "00:00";
    
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

void main_window_load(Window *window) {
    //Set the windows background color
    window_set_background_color(s_main_window, GColorBlack);
    
    //Create GFont
    s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_48));
    s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_20));
    
    //Create time TextLayer
    s_time_layer = text_layer_create(GRect(2, 110, 144, 50));
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorWhite);
    
    //Improve the layout to be more like a watchface
    text_layer_set_font(s_time_layer, s_time_font);
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    
    //Add it as a child layer to the Window's root layer
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
    
    //Create weather TextLayer
    s_weather_layer = text_layer_create(GRect(2, 90, 144, 50));
    text_layer_set_background_color(s_weather_layer, GColorClear);
    text_layer_set_text_color(s_weather_layer, GColorWhite);
    text_layer_set_font(s_weather_layer, s_weather_font);
    text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
    text_layer_set_text(s_weather_layer, "Loading...");
    
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
}

void main_window_unload(Window *window) {
    //Destroy time elements
    text_layer_destroy(s_time_layer);
    fonts_unload_custom_font(s_time_font);
    
    //Destroy weather elements
    text_layer_destroy(s_weather_layer);
    fonts_unload_custom_font(s_weather_font);
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

void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    //Store incoming information
    static char temperature_buffer[8];
    static char conditions_buffer[32];
    static char weather_layer_buffer[32];
    
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

void init() {
    //Create the main window and assign to the pointer
    s_main_window = window_create();
    
    //Set the handlers to manage the elements inside the window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });
    
    //Show the window on the watch with animated = true
    window_stack_push(s_main_window, true);
    
    //Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    
    //Make sure the time is displayed from the start
    update_time();
    
    //Register callbacks
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    
    //Open AppMessage
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void deinit() {
    //Destroy the window
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}