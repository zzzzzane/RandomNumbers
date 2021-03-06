//standard header
#include "pebble.h"
#include "iota.h"

//app-specific data
Window *window; //obvious window is obvious
TextLayer *time_layer; //time layer

/*what the window does*/
static void fill_window(struct tm* tick_time) {
	
  	static char full_string[125]; //125 characters
  	static char time_hours[] = "00";
 	  static char time_minutes[] = "00";
  	static char time_month[] = "00";
  	static char time_day[] = "00";
	  int r; //init variables for number of lines and random numbers
	  //can't init 'r' inside switch-statement due to restrictions of C; boo-hiss
	
	/*each time-component size*/
  /*handle 24 hour time setting */
  if (clock_is_24h_style()) {
  	strftime(time_hours, sizeof("00"), "%H", tick_time);
  } else {    
  	strftime(time_hours, sizeof("00"), "%I", tick_time);
  }
  	strftime(time_minutes, sizeof("00"), "%M", tick_time);
  	strftime(time_month, sizeof("00"), "%m", tick_time);
  	strftime(time_day, sizeof("00"), "%d", tick_time);
	
  	srand(time(NULL)); //'NULL' tick event

  	//loop prints 40 number lines
  	for (int i = 1; i < 41; i++) {
	  	switch(i)
		{
			case 1 : //i == 1
		  	strcpy(full_string, time_hours); //hours to first row, first slot
			strcat(full_string, ".");
		  	continue;
			
		  	case 5 : //i == 5
		  	strcat(full_string, time_minutes); //minutes to first row, last slot
			strcat(full_string, "\n");
		  	continue;
			
		  	case 36 :
		  	strcat(full_string, time_month); //month to last row, last slot
			strcat(full_string, ".");
		  	continue;
			
		  	case 40 : //i == 40  
		  	strcat(full_string, time_day); //day to last row, last slot
			strcat(full_string, "\n");
			continue;
		  
		  	default : //if 'i' isn't equal to one of the values above... do this instead!
        if (tick_time->tm_min == 0) {
          			  strcat(full_string, time_hours);
        }
        else
        {
		  	r = rand() % 99 + 1; //random number, 1-99; this bypasses the whole check for zero

        if (r < 10)
				  strcat(full_string, "0");
                       
			  strcat(full_string, itoa(r));
        }
        
			  if ((i % 5) == 0)
				  strcat(full_string, "\n");
			  else
				  strcat(full_string, ".");
	  }
	}
  	text_layer_set_text(time_layer, full_string); //set the text
}
  
/*called once per minute*/
static void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) {

    static char full_string2[125];
  	static char time_hours[] = "00";
 	  static char time_minutes[] = "00";
  	static char time_month[] = "00";
  	static char time_day[] = "00";
    const char *StringPtr ;
  
  /*Random numbers change once per 10 minutes */ 
	if (((tick_time->tm_min % 10) == 0) || (tick_time->tm_min == 1)) 
  {
    fill_window(tick_time);
  }  else
  {

  /*handle 24 hour time setting */
  if (clock_is_24h_style()) {
  	strftime(time_hours, sizeof("00"), "%H", tick_time);
  } else {    
  	strftime(time_hours, sizeof("00"), "%I", tick_time);
  }
  	strftime(time_minutes, sizeof("00"), "%M", tick_time);
  	strftime(time_month, sizeof("00"), "%m", tick_time);
  	strftime(time_day, sizeof("00"), "%d", tick_time);
  
  /* replace the date and time only */
    StringPtr = text_layer_get_text(time_layer);
    strcpy(full_string2, StringPtr);
    full_string2[0] = time_hours[0];
    full_string2[1] = time_hours[1];
    full_string2[12] = time_minutes[0];
    full_string2[13] = time_minutes[1];
    full_string2[105] = time_month[0];
    full_string2[106] = time_month[1];
    full_string2[117] = time_day[0];
    full_string2[118] = time_day[1];
	  text_layer_set_text(time_layer, full_string2); //set the text
  }
  }

/*handle the start-up of the app*/
static void do_init(void) {
	
	//create the watchface's base window
  	window = window_create();
  	window_stack_push(window, true);
  	window_set_background_color(window, GColorBlack);

  	//init the text layer used to show the time
  	time_layer = text_layer_create(GRect(0, 0, 144, 168)); //format: GRect(0, 0, width, height)
  	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  	text_layer_set_text_color(time_layer, GColorWhite);
  	text_layer_set_background_color(time_layer, GColorClear);
  	text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));

  	/*ensures time is displayed immediately (will break if 'NULL' tick event accessed);
  	(this is why it's a good idea to have a separate routine to do the update itself)*/
  	time_t now = time(NULL);
  	struct tm *current_time = localtime(&now);
  	fill_window(current_time);
  	handle_minute_tick(current_time, MINUTE_UNIT);
  	tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);

  	layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
}

/*deinits the layers*/
static void do_deinit(void) {
   
	text_layer_destroy(time_layer);
  	window_destroy(window);
}

/*the main "run" loop for our app*/
int main() {
	
  	do_init();
  	app_event_loop();
  	do_deinit();
}
