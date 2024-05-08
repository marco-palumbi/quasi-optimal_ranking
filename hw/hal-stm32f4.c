// =============================================
// ==    Classification: TII CONFIDENTIAL     ==
// =============================================

#include <errno.h>
#include <unistd.h>
#include "hal.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/rng.h>
#include <libopencm3/stm32/usart.h>

int _write(int file, char *ptr, int len);

uint64_t tick_per_sec;

/* 24 MHz */
const struct rcc_clock_scale benchmarkclock = {
  .pllm = 8,    // VCOin = HSE / PLLM = 1 MHz
  .plln = 192,  // VCOout = VCOin * PLLN = 192 MHz
  .pllp = 8,    // PLLCLK = VCOout / PLLP = 24 MHz (low to have 0WS)
  .pllq = 4,    // PLL48CLK = VCOout / PLLQ = 48 MHz (required for USB, RNG)
  .pllr = 0,
  .hpre = RCC_CFGR_HPRE_DIV_NONE,
  .ppre1 = RCC_CFGR_PPRE_DIV_2,
  .ppre2 = RCC_CFGR_PPRE_DIV_NONE,
  .pll_source = RCC_CFGR_PLLSRC_HSE_CLK,
  .voltage_scale = PWR_SCALE1,
  .flash_config = FLASH_ACR_DCEN | FLASH_ACR_ICEN | FLASH_ACR_LATENCY_0WS,
  .ahb_frequency = 24000000,
  .apb1_frequency = 12000000,
  .apb2_frequency = 24000000,
};

static void clock_setup(const enum clock_mode clock) {
  switch (clock) {
    case CLOCK_BENCHMARK:
      rcc_clock_setup_pll(&benchmarkclock);
      tick_per_sec = 24000000LLU;
      break;
    case CLOCK_QEMU:  // qemu seems not to support clock initialization
      break;
    case CLOCK_FAST:
    default:
      rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
      tick_per_sec = 168000000LLU;
      break;
  }

  rcc_periph_clock_enable(RCC_GPIOx);
  rcc_periph_clock_enable(RCC_USARTx);
  rcc_periph_clock_enable(RCC_DMA1);
  rcc_periph_clock_enable(RCC_RNG);

  flash_prefetch_enable();
}

static void gpio_setup(void) {
  gpio_mode_setup(GPIOx, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIOtx | GPIOrx);
  gpio_set_af(GPIOx, GPIO_AFx, GPIOtx | GPIOrx);
}

static void usart_setup(int baud) {
  usart_set_baudrate(USARTx, baud);
  usart_set_databits(USARTx, 8);
  usart_set_stopbits(USARTx, USART_STOPBITS_1);
  usart_set_mode(USARTx, USART_MODE_TX_RX);
  usart_set_parity(USARTx, USART_PARITY_NONE);
  usart_set_flow_control(USARTx, USART_FLOWCONTROL_NONE);

  usart_enable(USARTx);
}

static void systick_setup(void) {
  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
  systick_set_reload(16777215);
  systick_interrupt_enable();
  systick_counter_enable();
}

/*
static void send_USART_str(const char* in)
{
  int i;
  for(i = 0; in[i] != 0; i++) {
    usart_send_blocking(USARTx, *(uint8_t *)(in+i));
  }
  usart_send_blocking(USARTx, '\n');
}
*/

/**
 * Use USART_CONSOLE as a console.
 * This is a syscall for newlib
 * @param file
 * @param ptr
 * @param len
 * @return
 */
int _write(int file, char *ptr, int len) {
  int i;

  if (file == STDOUT_FILENO || file == STDERR_FILENO) {
    for (i = 0; i < len; i++) {
      // if (ptr[i] == '\n') {
      // 	usart_send_blocking(USARTx, '\r');
      // }
      usart_send_blocking(USARTx, ptr[i]);
    }
    return i;
  }
  errno = EIO;
  return -1;
}

int usleep(useconds_t usec) {
  uint64_t t0 = hal_get_time();
  while ((hal_get_time() - t0) < ((uint64_t)usec * tick_per_sec) / 1000000ULL)
    ;
  t0 = hal_get_time() - t0;
  return 0;
}

void hal_setup_m4(const enum clock_mode clock) {
#ifdef USE_QEMU
  (void)clock;
  clock_setup(CLOCK_QEMU);
#else
  clock_setup(clock);
#endif
  gpio_setup();
  usart_setup(115200);
  systick_setup();
#ifdef USE_QEMU
  hw_randombytes(NULL, 0);  // seed the shake128 based randombytes
#else
  rng_enable();
#endif
  // add 0.5 sec delay
  usleep(500000);
}

static volatile unsigned long long overflowcnt = 0;
void sys_tick_handler(void) {
  ++overflowcnt;
}

uint64_t hal_get_time() {
  while (true) {
    unsigned long long before = overflowcnt;
    unsigned long long result = (before + 1) * 16777216llu - systick_get_value();
    if (overflowcnt == before) {
      return result;
    }
  }
}
