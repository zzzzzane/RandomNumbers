#include "pebble.h"
#include "buff.h"

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
	int r; //init variable for rand numbers
	
	/*each time-component size*/
  	strftime(time_hours, sizeof("00"), "%I", tick_time);
  	strftime(time_minutes, sizeof("00"), "%M", tick_time);
  	strftime(time_month, sizeof("00"), "%m", tick_time);
  	strftime(time_day, sizeof("00"), "%d", tick_time);
	
  	srand(time(NULL)); //'NULL' tick event

  	//loop prints 36 number lines
  	for (int i = 1; i < 37; i++) {
	  	switch(i)
		{
			case 1 :
		  	strcpy(full_string, time_hours);
			strcat(full_string, ".");
		  	continue;
			
		  	case 6 :
		  	strcat(full_string, time_minutes);
			strcat(full_string, "\n");
		  	continue;
			
		  	case 31 :
		  	strcat(full_string, time_month);
			strcat(full_string, ".");
		  	continue;
			
		  	case 36 :
		  	strcat(full_string, time_day);
			strcat(full_string, "\n");
			continue;
		  
		  	default :
		  	r = rand() % 99 + 1; //random number, 1-99
		  	if (r < 10)
				strcat(full_string, "0");
			strcat(full_string, buff(r));
			if ((i % 6) == 0)
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
        const char *StringPtr;
	
	/*random numbers change once per 10 minutes*/ 
	if ((tick_time->tm_min % 10) == 0) {
    	fill_window(tick_time);
	} else {
  		/*handle 24 hour time setting */
  		if (clock_is_24h_style())
  			strftime(time_hours, sizeof("00"), "%H", tick_time);
		else
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
    full_string2[15] = time_minutes[0];
    full_string2[16] = time_minutes[1];
    full_string2[105] = time_month[0];
    full_string2[106] = time_month[1];
    full_string2[105] = time_day[0];
    full_string2[106] = time_day[1];
	
    text_layer_set_text(time_layer, full_string2); //set the text
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
  	text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));

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
	
	tick_timer_service_unsubscribe();
	text_layer_destroy(time_layer);
  	window_destroy(window);
}

/*the main "run" loop for our app*/
int main() {
	
  	do_init();
  	app_event_loop();
  	do_deinit();
}
