#include "sm/main_sm.h"
#include "sm/state_machine.h"

int main() {
	init_sm_framework();
	init_main_sm();

	Event e = NULL_EVENT;

	while (true) {
		e = get_next_event();
		process_event(get_main_sm(), e);
	}
}
