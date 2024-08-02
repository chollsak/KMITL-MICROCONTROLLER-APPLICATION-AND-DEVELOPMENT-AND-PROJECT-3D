/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private variables ---------------------------------------------------------*/
static uint8_t currentLED = 0;  // Index of the current LED
#define DEBOUNCE_DELAY 50  // Debounce delay in milliseconds
#define LED_COUNT 8       // Number of LEDs

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void handleButtonPC13Press(void);
void handleButtonPA0Press(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* Initialize all configured peripherals */
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();

    /* Infinite loop */
    while (1)
    {
    	 handleButtonPA0Press();
        handleButtonPC13Press();

    }
}

/**
  * @brief  Handle button PC13 press for LED control.
  */
void handleButtonPC13Press(void)
{
    static uint8_t lastButtonState = GPIO_PIN_SET; // Default to high
    static uint32_t lastButtonPressTime = 0;
    uint8_t buttonState = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);

    // Check for button state change and debounce
    if (buttonState != lastButtonState && (HAL_GetTick() - lastButtonPressTime) > DEBOUNCE_DELAY)
    {
        if (buttonState == GPIO_PIN_RESET) // Button pressed
        {
            // Turn off all LEDs
            HAL_GPIO_WritePin(GPIOD, 0xFF, GPIO_PIN_RESET); // 0xFF turns off all LEDs

            // Turn on the current LED
            HAL_GPIO_WritePin(GPIOD, (1 << currentLED), GPIO_PIN_SET);

            // Move to the next LED
            currentLED = (currentLED + 1) % LED_COUNT;

            // Wait for button release
            while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET);
        }

        lastButtonPressTime = HAL_GetTick(); // Update last press time
    }

    lastButtonState = buttonState;
}




/**
  * @brief  Handle button PA0 press for auto-cycling LEDs.
  */
void handleButtonPA0Press(void)
{
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
    {
        // LED auto-cycling from LED7 to LED0
        for (int8_t i = 7; i >= 0; i--)
        {
            HAL_GPIO_WritePin(GPIOD, (1 << i), GPIO_PIN_SET);
            HAL_Delay(500);
            HAL_GPIO_WritePin(GPIOD, (1 << i), GPIO_PIN_RESET);
        }

        // Wait until PA0 button is released
        while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
        {
            // Do nothing, just wait
        }
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Configure the main internal regulator output voltage */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    /* Initializes the RCC Oscillators according to the specified parameters in the RCC_OscInitTypeDef structure. */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /* Initializes the CPU, AHB and APB buses clocks */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();  // Enable clock for GPIOB
    __HAL_RCC_GPIOC_CLK_ENABLE();  // Enable clock for GPIOC
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /* Configure GPIO pins : PA0 */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Configure GPIO pin : PB1 */
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Configure GPIO pin : PC13 (Button) */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;  // Adjust if you have a different pull configuration
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Configure GPIO pins : PD0 PD1 PD2 PD3 PD4 PD5 PD6 PD7 */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                            |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}


void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}
