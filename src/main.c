/*

   V2.0 First attempt

 */

// Standard includes
#include "pebble.h"
#include "iota.h"

// App-specific data
Window *window; // All apps must have at least one window
TextLayer *time_layer; // The clock

static void fill_window(struct tm* tick_time)
{
 
  
  static char full_string[125]; 
  int i;
  int r;
  static char time_hours[] = "00";
  static char time_minutes[] = "00";
  static char time_month[] = "00";
  static char time_day[] = "00";

  strftime(time_hours, sizeof("00"), "%I", tick_time);
  strftime(time_minutes, sizeof("00"), "%M", tick_time);
  strftime(time_month, sizeof("00"), "%m", tick_time);
  strftime(time_day, sizeof("00"), "%d", tick_time);
  
  srand(time(NULL));

  //set hours to first character
  //40 number lines
  
  for (i = 1; i < 41; i++)
  {
 
    if (i==1) {
        strcpy(full_string,time_hours);
        } else {
    if (i==5) { strcat(full_string,time_minutes); } else {
    if (i==36) {  strcat(full_string,time_month); } else {
    if (i==40 ) {  strcat(full_string,time_day);  } else {

              r = rand() % 100;
              if (r < 10 ) { strcat(full_string, "0"); }
              if (r == 0 ) { strcat(full_string, "0"); } 
              strcat(full_string, itoa(r));        
                       
          }}}}
  if  (i % 5 == 0) { strcat(full_string, "\n"); }
   else {            strcat(full_string, ":");  }
  }
  text_layer_set_text(time_layer, full_string);
}
  
  // Called once per second
static void handle_second_tick(struct tm* tick_time, TimeUnits units_changed) {

  if ((tick_time->tm_sec % 10) == 0  ) { fill_window(tick_time);}
}

// Handle the start-up of the app
static void do_init(void) {

  // Create our app's base window
  window = window_create();
  window_stack_push(window, true);
  window_set_background_color(window, GColorBlack);

  // Init the text layer used to show the time
  time_layer = text_layer_create(GRect(0, 0, 144 /* width */, 168 /* height */));
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter );
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  fill_window(current_time);
  handle_second_tick(current_time, SECOND_UNIT);
  tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
}

static void do_deinit(void) {
  text_layer_destroy(time_layer);
  window_destroy(window);
}

// The main event/run loop for our app
int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}