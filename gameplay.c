#include <avr/io.h>
#include "gameplay.h"

void init_flags(Flags * _flags)
{
	_flags->boost_available = 1;
	_flags->boost_enabled = 0;

	/* config pins as inputs */
	CONFIG_DDR &= ~(CONFIG_MASK);
	/* enable pullups */
	CONFIG_PORT |= CONFIG_MASK;

	/* debug pins as outputs */
	DEBUG_PORT &= ~(DEBUG_MASK);
	DEBUG_DDR |= DEBUG_MASK;
}
/*
 * Boost timer, step one : boost is active for a short while
 * Short timer interrupt disables boost then launches a longer timer.
 * At the end of long period, boost is re-enabled
 */
#define BOOST_DURATION 3
#define INTERVAL_BETWEEN_BOOSTS	15
void call_booster(Flags * _flags)
{
	/* are we allowed to use the booster yet? */
	if (_flags->boost_available && !_flags->boost_enabled)
	{
		/* not available until timeout */
		_flags->boost_available = 0;
		/* give a few seconds of boost */
		_flags->boost_enabled = 1;
		/* start timer for deactivation */
		// TODO
	}
	/* all other cases : just wait for timer to time out */
	return;
}

/* function that gets called by booster timer overflow */
// TODO : rename this function
void overflow_callback(Flags * _flags)
{
	/* if booster is currently in use */
	if (_flags->boost_enabled)
	{
		_flags->boost_enabled = 0;
		/* start long timer to re-enable */
		// TODO
	}
	else
	{
		/* we've waited long enough : let's have some boost again! */
		_flags->boost_available = 1;
	}
}

/*
 * Read configuration from DIP switch
 */
void read_config(Flags * _flags)
{
	/* reverse steering or gas control */
	read_pin(CONFIG_REV_STEER,	reverse_steering);
	read_pin(CONFIG_REV_SPEED,	reverse_speed);

	/* emergency off button */
	read_pin(CONFIG_KILLSWITCH, 	killswitch_engaged);

	/* randomly change one of the ACTIVE reversals */
	read_pin(CONFIG_RANDOM, 	randomness);

	/* 
	 * now we must play... THE MOST DANGEROUS GAME 
	 * ( remove speed limits )
	 */
	read_pin(CONFIG_DANGEROUS, 	most_dangerous_game);

	DEBUG_PORT = ~((CONFIG_PINS << 4) | (CONFIG_PINS & 0x01)) & 0xe1;

}

