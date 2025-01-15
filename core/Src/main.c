
#include "init.h"

/* Simple delay function */
void DelayLoop(volatile uint32_t cycleCount)
{
    while (cycleCount--)
    {
        /* Intentional empty loop for delay */
    }
}

static uint8_t s_ledQuantity    = 1;    /* Indicates how many LEDs will be lit on Button2 action */
static uint8_t s_activeLedIndex = 0;    /* Tracks which LED is currently lit */
static uint32_t s_ledMapping[]  = { LED1_ON, LED2_ON, LED3_ON };

static uint8_t s_toggleMode     = 0;    /* When in 3-LED mode, this variable toggles the LED state */

static uint8_t s_button1State;
static uint8_t s_button2State;

/* Forward declaration for LED handling */
static void HandleLEDs(uint8_t ledIndex, uint8_t ledCount);

int main(void)
{
    InitializeGPIOSelf();
    InitializeGPIOCMSIS();

    uint8_t button1PreviouslyPressed = 0;
    uint8_t button2PreviouslyPressed = 0;

    while (1)
    {
        /* Refresh button states (for demonstration, though not strictly necessary) */
        s_button1State = !button1PreviouslyPressed;
        s_button2State = !button2PreviouslyPressed;

        /*******************************************************************
         * Button 1 logic
         * - If 3 LEDs are active, toggle them all on or off.
         * - Otherwise, rotate which LED is lit.
         ******************************************************************/
        if ( !(READ_BIT_SELF(GPIOC_IDR, GPIO_PIN_13) != 0) )
        {
            if (!button1PreviouslyPressed)
            {
                button1PreviouslyPressed = 1;

                if (s_ledQuantity == 3)
                {
                    /* Toggle all LEDs if we are in the 3-LED mode */
                    s_toggleMode = !s_toggleMode;
                    HandleLEDs(s_activeLedIndex, s_ledQuantity);
                }
                else
                {
                    /* Cycle to the next LED if fewer than 3 are on */
                    s_activeLedIndex = (s_activeLedIndex + 1) % 3;
                    HandleLEDs(s_activeLedIndex, s_ledQuantity);
                }
            }
        }
        else
        {
            button1PreviouslyPressed = 0;
        }

        /*******************************************************************
         * Button 2 logic
         * - Cycles through LED counts: 1 -> 2 -> 3 -> 1 ...
         * - Invokes HandleLEDs to update lighting.
         ******************************************************************/
        if ( !(READ_BIT(GPIOC->IDR, GPIO_IDR_ID6) != 0) )
        {
            if (!button2PreviouslyPressed)
            {
                button2PreviouslyPressed = 1;
                s_ledQuantity = (s_ledQuantity % 3) + 1; 
                HandleLEDs(s_activeLedIndex, s_ledQuantity);
            }
        }
        else
        {
            button2PreviouslyPressed = 0;
        }

        /* Debounce delay */
        DelayLoop(1000);
    }
}

/****************************************************************************
 *  @brief   Updates LED states based on index and count
 ****************************************************************************/
static void HandleLEDs(uint8_t ledIndex, uint8_t ledCount)
{
    /* Turn off all LEDs first */
    GPIOB->BSRR = (LED1_ON | LED2_ON | LED3_ON) << 16;

    /* If the count is 3 and toggleMode is set, turn them all on */
    if (ledCount == 3)
    {
        if (s_toggleMode)
        {
            GPIOB->BSRR = LED1_ON | LED2_ON | LED3_ON;
        }
    }
    else
    {
        /* Light up the specified number of consecutive LEDs */
        for (uint8_t i = 0; i < ledCount; i++)
        {
            uint8_t idx = (ledIndex + i) % 3;
            GPIOB->BSRR = s_ledMapping[idx];
        }
    }
}
