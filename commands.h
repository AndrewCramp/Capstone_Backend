#define	GET_ANALOG_VALUE   "A" 
#define	ANALOG_CONFIGURE   "AC" 
#define	BOOTLOADER   "BL" 
#define	CONFIGURE   "C" 
#define	CLEAR_NODE_COUNT   "CN" 
#define	CHECK_INPUT   "CK" 
#define	CONFIGURE_OPTIONS   "CU" 
#define	CLEAR_STEP_POSITION   "CS" 
#define	ENABLE_MOTORS   "EM" 
#define	E_STOP   "ES" 
#define	HOME   "HM" 
#define	INPUT   "I" 
#define	LOW_LEVEL_MOVE   "LM" 
#define	LOW_LEVEL_MMOVE_T   "LT" 
#define	MEMORY_READ   "MR" 
#define	MEMORY_WRITE   "MW" 
#define	DECREMENT_NODE_COUNT   "ND" 
#define	INCREMENT_NODE_COUNT   "NI" 
#define	OUTPUT   "O" 
#define	PULSE_CONFIGURE   "PC" 
#define	PIN_DIRECTION   "PD" 
#define	PULSE_GO   "PG" 
#define	PIN_INPUT   "PI" 
#define	PIN_OUTPUT   "PO" 
#define	QUERY_BUTTON   "QB" 
#define	QUERY_CURRENT   "QC" 
#define	QUERY_ENABLE   "QE" 
#define	QUERY_GENERAL   "QG" 
#define	QUERY_LAYER   "QL" 
#define	QUERY_MOTORS   "QM" 
#define	QUERY_NODE_COUNT   "QN" 
#define	QUERY_PEN   "QP" 
#define	QUERY_RC   "QR" 
#define	QUERY_STEP   "QS" 
#define	QUERY_NAME   "QT" 
#define	REBOOT   "RB" 
#define	RESET   "R" 
#define	SERVO_OUTPUT   "S2" 
#define	STEPPER_SERVO_CONFIGURE   "SC" 
#define	SET_ENGRAVER   "SE" 
#define	SET_LAYER   "SL" 
#define	STEPPER_MOVE   "SM" 
#define	SET_NODE_COUNT   "SN" 
#define	SET_PEN   "SP" 
#define	SET_RC_TIMEOUT   "SR" 
#define	SET_NAME   "ST" 
#define	TIMED_READ   "T" 
#define	VERSION_QUERY   "V" 
#define	STEPPER_MOVE_MIXED   "XM"

void generateCommand(char* command, char* prefix, char* parameters){
	int last = 0;
	memset(command, '\0', sizeof(command));
	memcpy(command, prefix,  strlen(prefix));
	last+= strlen(prefix); 
	if(parameters != ""){
		command[strlen(command)] = ',';
		memcpy(&command[strlen(command)], parameters, strlen(parameters));
		last+= strlen(parameters)+1; 
	}
	command[last] = '\n';
	command[last+1] = '\0';
}


