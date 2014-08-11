#include <pebble.h>
#include "time_layer.h"
#include "weather_layer.h"    

Window *s_main_window;

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

static void init() {
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

static void deinit() {
    //Destroy the window
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}