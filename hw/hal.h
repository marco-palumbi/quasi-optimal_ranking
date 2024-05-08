// =============================================
// ==    Classification: TII CONFIDENTIAL     ==
// =============================================

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef HAL_H
#define HAL_H

enum clock_mode { CLOCK_FAST, CLOCK_BENCHMARK, CLOCK_QEMU };

/* USART6
#define RCC_USARTx RCC_USART6
#define USARTx USART6
#define RCC_GPIOx RCC_GPIOC
#define GPIOx GPIOC
#define GPIO_AFx GPIO_AF8
#define GPIOtx GPIO6  // PC6
#define GPIOrx GPIO7  // PC7
*/
/* USART2 */
#define RCC_USARTx RCC_USART2
#define USARTx     USART2
#define RCC_GPIOx  RCC_GPIOA
#define GPIOx      GPIOA
#define GPIO_AFx   GPIO_AF7
#define GPIOtx     GPIO2  // PA2
#define GPIOrx     GPIO3  // PA3

void hal_setup_m4(enum clock_mode clock);
uint64_t hal_get_time(void);

int hw_randombytes(uint8_t *buf, size_t n);

// markers for automated testing
#define xstr(s) str(s)
#define str(s)  #s
#define hal_send_ERROR(str)       \
    printf("%s\n", "ERROR " str   \
                   " in "__FILE__ \
                   ":" xstr(__LINE__) "\n")

#define hal_start_MARKER() printf("%s\n", "\n\n==========================")
#define hal_end_MARKER()   printf("%s\n", "===*** END OF EXECUTION ***===")

#ifdef STM32F4
#define hal_exit_(error)       \
    do {                       \
        __asm volatile("NOP"); \
    } while (1)
#else
#define hal_exit_(error) exit(error)
#endif

#define hal_end_TEST(error)                   \
    do {                                      \
        printf("Errors found %d\n", (error)); \
        if ((error) == 0)                     \
            printf("[  PASSED  ]\n");         \
        else                                  \
            printf("[  FAILED  ]\n");         \
        hal_end_MARKER();                     \
        hal_exit_((error) == 0 ? 0 : 1);      \
    } while (0)

extern uint64_t tick_per_sec;

#ifdef STM32F4
#define PLATFORM_CLOCK()        hal_get_time()
#define PLATFORM_CLOCKS_PER_SEC tick_per_sec
#define hal_setup(clock)        hal_setup_m4(clock)
#elif defined USERDTSC
static inline uint64_t cpucycles(void) {
    uint64_t result;

    __asm__ volatile("rdtsc; shlq $32,%%rdx; orq %%rdx,%%rax" : "=a"(result) : : "%rdx");

    return result;
}
#define PLATFORM_CLOCK()        cpucycles()
#define PLATFORM_CLOCKS_PER_SEC 2000000000UL
#define hal_setup(clock)
#else
#include <time.h>
static inline uint64_t clock_usec(void) {
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);
    return (uint64_t)(spec.tv_sec) * 1000000000UL + spec.tv_nsec / 1;
}
#define PLATFORM_CLOCK()        clock_usec()
#define PLATFORM_CLOCKS_PER_SEC 1000000000UL
#define hal_setup(clock)
#endif

#endif
