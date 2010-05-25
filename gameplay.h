/*
 * Gameplay.h
 *
 * Read and set game status flags and configuration
 *
 */

/*
 * Configuration port
 */

#define CONFIG_DDR	DDRA
#define CONFIG_PINS	PINA
#define CONFIG_PORT	PORTA
#define CONFIG_MASK	0x1F
#define CONFIG_DANGEROUS	PA0
#define CONFIG_RANDOM		PA1
#define CONFIG_REV_STEER	PA2
#define CONFIG_REV_SPEED	PA3
#define CONFIG_KILLSWITCH	PA4	/* main disable switch */

#define DEBUG_PORT	PORTB
#define DEBUG_DDR	DDRB
#define DEBUG_MASK	0xF0

/*
 * Flags store current game status
 */
typedef struct 
{
	int boost_available:1;
	int boost_enabled:1;
	int reverse_steering:1;
	int reverse_speed:1;
	int randomness:1;
	int most_dangerous_game:1;
	int killswitch_engaged:1;
} Flags;


/*
 * Functions
 */
void init_flags(Flags * _flags);
void read_config(Flags * _flags);
void overflow_callback(Flags * _flags);
void call_booster(Flags * _flags);

/*
 * Private macros
 */
#define read_pin(pin, var)	(_flags->var = bit_is_clear(CONFIG_PINS,pin) ? 1 : 0 )
