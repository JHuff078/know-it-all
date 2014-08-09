#include <pebble.h>

static Window *s_main_window;    
static TextLayer *s_time_layer;
static GFont *s_time_font;

static void update_time() {
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

static void main_window_load(Window *window) {
    //Create GFont
    s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_48));
    
    //Create time TextLayer
    s_time_layer = text_layer_create(GRect(5, 52, 139, 50));
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorBlack);
    
    //Improve the layout to be more like a watchface
    text_layer_set_font(s_time_layer, s_time_font);
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    
    //Add it as a child layer to the Window's root layer
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
    //Destroy TextLayer
    text_layer_destroy(s_time_layer);
    
    //Unload GFont
    fonts_unload_custom_font(s_time_font);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
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