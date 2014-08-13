#include <pebble.h>
    
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1

#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168
#define PADDING_X 1
#define PADDING_Y 1

#define BATTERY_ICON_SIZE 24
#define BATTERY_ICON_POS_X SCREEN_WIDTH - BATTERY_ICON_SIZE - PADDING_X
#define BATTERY_ICON_POS_Y 0

#define BLUETOOTH_ICON_SIZE 24
#define BLUETOOTH_ICON_POS_X BATTERY_ICON_POS_X - BLUETOOTH_ICON_SIZE - PADDING_X
#define BLUETOOTH_ICON_POS_Y 0

#define TIME_FONT_SIZE 49
#define TIME_LAYER_WIDTH SCREEN_WIDTH
#define TIME_LAYER_HEIGHT 50
#define TIME_LAYER_POS_X 0
#define TIME_LAYER_POS_Y BATTERY_ICON_POS_Y + BATTERY_ICON_SIZE + PADDING_Y

#define WEATHER_FONT_SIZE 21
#define WEATHER_LAYER_WIDTH SCREEN_WIDTH
#define WEATHER_LAYER_HEIGHT 25
#define WEATHER_LAYER_POS_X 0
#define WEATHER_LAYER_POS_Y SCREEN_HEIGHT - WEATHER_FONT_SIZE - PADDING_Y


Window *main_window;

TextLayer *time_layer;

TextLayer *weather_layer;

BitmapLayer *battery_icon_layer;
GBitmap *battery_icon[4];

BitmapLayer *bluetooth_icon_layer;
GBitmap *bluetooth_icon;


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
    text_layer_set_text(time_layer, buffer);
}

void update_battery_icon(BatteryChargeState charge_state) {
    if (charge_state.is_charging) {
        bitmap_layer_set_bitmap(battery_icon_layer, battery_icon[3]);
    } else if (charge_state.charge_percent > 80) {
        bitmap_layer_set_bitmap(battery_icon_layer, battery_icon[2]);
    } else if (charge_state.charge_percent > 20) {
        bitmap_layer_set_bitmap(battery_icon_layer, battery_icon[1]);
    } else {
        bitmap_layer_set_bitmap(battery_icon_layer, battery_icon[0]);
    }
}

void main_window_load(Window *window) {
    //Set the windows background color
    window_set_background_color(main_window, GColorBlack);
    
    /*
     * Time Layer
     */
    //Create time TextLayer
    time_layer = text_layer_create(GRect(TIME_LAYER_POS_X, TIME_LAYER_POS_Y, TIME_LAYER_WIDTH, TIME_LAYER_HEIGHT));
    text_layer_set_background_color(time_layer, GColorClear);
    text_layer_set_text_color(time_layer, GColorWhite);
    text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
    text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
    
    //Add it as a child layer to the Window's root layer
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
    
    /*
     * Weather Layer
     */
    //Create weather TextLayer
    weather_layer = text_layer_create(GRect(WEATHER_LAYER_POS_X, WEATHER_LAYER_POS_Y, WEATHER_LAYER_WIDTH, WEATHER_LAYER_HEIGHT));
    text_layer_set_background_color(weather_layer, GColorClear);
    text_layer_set_text_color(weather_layer, GColorWhite);
    text_layer_set_text_alignment(weather_layer, GTextAlignmentCenter);
    text_layer_set_font(weather_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
    text_layer_set_text(weather_layer, "Loading...");
    
    //Add it as a child layer to the Window's root layer
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(weather_layer));

    /*
     * Battery Icon Layer
     */
    //Create GBitmap, then set it to created BitmapLayer
    battery_icon[0] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_EMPTY_ICON);
    battery_icon[1] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_HALF_ICON);
    battery_icon[2] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_FULL_ICON);
    battery_icon[3] = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_CHARGING_ICON);
    battery_icon_layer = bitmap_layer_create(GRect(BATTERY_ICON_POS_X, BATTERY_ICON_POS_Y, BATTERY_ICON_SIZE, BATTERY_ICON_SIZE));
    update_battery_icon(battery_state_service_peek());
    bitmap_layer_set_alignment(battery_icon_layer, GAlignCenter);
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(battery_icon_layer));

    /*
     * Bluetooth Icon Layer
     */
    //Create GBitmap, then set it to created BitmapLayer
    bluetooth_icon = gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH_ICON);
    bluetooth_icon_layer = bitmap_layer_create(GRect(BLUETOOTH_ICON_POS_X, BLUETOOTH_ICON_POS_Y, BLUETOOTH_ICON_SIZE, BLUETOOTH_ICON_SIZE));
    bitmap_layer_set_bitmap(bluetooth_icon_layer, bluetooth_icon);
    bitmap_layer_set_alignment(bluetooth_icon_layer, GAlignCenter);
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bluetooth_icon_layer));
   
    if (!bluetooth_connection_service_peek()) {
        layer_set_hidden(bitmap_layer_get_layer(bluetooth_icon_layer), true);
    }
}

void main_window_unload(Window *window) {
    //Destroy time elements
    text_layer_destroy(time_layer);
    
    //Destroy weather elements
    text_layer_destroy(weather_layer);

    //Destroy Battery Icon elements
    GBitmap *ptr = battery_icon[0];
    while (!ptr) {
        gbitmap_destroy(ptr);
        ++ptr;
    }
    bitmap_layer_destroy(battery_icon_layer);
    
    //Destroy Bluetooth Icon elements
    gbitmap_destroy(bluetooth_icon);
    bitmap_layer_destroy(bluetooth_icon_layer);
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

void bluetooth_handler(bool connected) {
    //Display the Bluetooth icon
    if (connected) {
        layer_set_hidden(bitmap_layer_get_layer(bluetooth_icon_layer), false);
    } else {
        layer_set_hidden(bitmap_layer_get_layer(bluetooth_icon_layer), true);
    }
}

void battery_handler(BatteryChargeState charge_state) {
    //Check the status of the battery and display the corresponding bitmap
    update_battery_icon(charge_state);
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
    text_layer_set_text(weather_layer, weather_layer_buffer);
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
    main_window = window_create();
    
    //Set the handlers to manage the elements inside the window
    window_set_window_handlers(main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });
    
    //Show the window on the watch with animated = true
    window_stack_push(main_window, true);
    
    //Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

    //Register with BluetoothConnectionService
    bluetooth_connection_service_subscribe(bluetooth_handler);
    
    //Register with BatteryStateService
    battery_state_service_subscribe(battery_handler);

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
    window_destroy(main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}