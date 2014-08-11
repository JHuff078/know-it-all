#pragma once

#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1

TextLayer *s_weather_layer;

GFont *s_weather_font;

void inbox_received_callback(DictionaryIterator *iterator, void *context);

void inbox_dropped_callback(AppMessageResult reason, void *context);

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context);

void outbox_sent_callback(DictionaryIterator *iterator, void *context);