#include "main_sm.h"
#include "lib\sm\state_machine.h"
//#include <vld.h> // For memory leak testing

int main() {
	init_sm_framework();
	init_main_sm();

	Event e = NULL_EVENT;

	while (e != STARTUP_COMPLETE) {
		e = get_next_event();
		process_event(get_main_sm(), e);
	}

	system("pause");
}