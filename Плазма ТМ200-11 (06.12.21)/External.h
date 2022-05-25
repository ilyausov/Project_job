// адреса внутренних устроств (плат ISO)
int
    base_813        = 0x0220,
    base_DA16       = 0x0230,
    base_P32C32_1   = 0x0210,
	base_P32C32_2   = 0x0280,
    base_7225_1     = 0x0260,
    base_7225_2     = 0x0250;

Word
    isoP32C32err1   = 1,
	isoP32C32err2   = 1,
    isoDA16err      = 1,
    iso813err       = 1,
    iso7225err1     = 1,
    iso7225err2     = 1,
	iso7250err      = 1;
	
#define VALUE_COUNT 7
#define AIK_813_COUNT 24
unsigned int
    // время полного опроса устрйоств
    timeOprosISO  = 0,
    avrgAikValues[AIK_813_COUNT][VALUE_COUNT] = {0};

