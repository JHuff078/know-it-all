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

void update_battery_icon(BatteryChargeState);

void update_bluetooth_visibility(bool);

void main_window_load(Window *);

void main_window_unload(Window *);

void tick_handler(struct tm *, TimeUnits);

void bluetooth_handler(bool);

void battery_handler(BatteryChargeState);

void inbox_received_callback(DictionaryIterator *, void *);

void inbox_dropped_callback(AppMessageResult, void *);

void outbox_failed_callback(DictionaryIterator *, AppMessageResult, void *);

void outbox_sent_callback(DictionaryIterator *, void *);

void init();

void deinit();