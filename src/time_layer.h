#pragma once

TextLayer *s_time_layer;

GFont *s_time_font;

void update_time();

void tick_handler(struct tm *tick_time, TimeUnits units_changed);