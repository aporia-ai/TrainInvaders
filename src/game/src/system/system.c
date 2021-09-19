
#include "system.h"

void system_init(SystemConfiguration *configuration) {
	system_base_init();
	system_video_init(configuration);
	system_input_init();
}
