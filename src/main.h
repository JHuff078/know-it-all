/*
 * Definitions
 */
//JavaScript Constants
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1

//Window UI Constants
#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168
#define PADDING_X 1
#define PADDING_Y 1

//Battery Layer Constants
#define BATTERY_ICON_SIZE 24
#define BATTERY_ICON_POS_X SCREEN_WIDTH - BATTERY_ICON_SIZE - PADDING_X
#define BATTERY_ICON_POS_Y 0

//Bluetooth Layer Constants
#define BLUETOOTH_ICON_SIZE 24
#define BLUETOOTH_ICON_POS_X BATTERY_ICON_POS_X - BLUETOOTH_ICON_SIZE - PADDING_X
#define BLUETOOTH_ICON_POS_Y 0

//Time Layer Constants
#define TIME_FONT_SIZE 49
#define TIME_LAYER_WIDTH SCREEN_WIDTH
#define TIME_LAYER_HEIGHT 50
#define TIME_LAYER_POS_X 0
#define TIME_LAYER_POS_Y BATTERY_ICON_POS_Y + BATTERY_ICON_SIZE + PADDING_Y

//Weather Layer Constants
#define WEATHER_FONT_SIZE 21
#define WEATHER_LAYER_WIDTH SCREEN_WIDTH
#define WEATHER_LAYER_HEIGHT 25
#define WEATHER_LAYER_POS_X 0
#define WEATHER_LAYER_POS_Y SCREEN_HEIGHT - WEATHER_FONT_SIZE - PADDING_Y

/*
 * Functions
 */
void update_time();

void update_battery_icon(BatteryChargeState charge_state);

void main_window_load(Window *window);

void main_window_unload(Window *window);

void tick_handler(struct tm *tick_time, TimeUnits units_changed);

void bluetooth_handler(bool connected);

void battery_handler(BatteryChargeState charge_state);

void inbox_received_callback(DictionaryIterator *iterator, void *context);

void inbox_dropped_callback(AppMessageResult reason, void *context);

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context);

void outbox_sent_callback(DictionaryIterator *iterator, void *context);

void init();

void deinit();