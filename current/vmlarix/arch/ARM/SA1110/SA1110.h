#ifndef SA_1110_h
#define SA_1110_h


#define UART_BASE (char *)0x80010000


/* Interrupt controller IRQ pending register */
#define ICIP (unsigned *)0x90050000
/* Interrupt controller mask register */
#define ICMR (unsigned *)0x90050004
/* Interrupt controller level register */
#define ICLR (unsigned *)0x90050008
/* Interrupt controller control register */
#define ICCR (unsigned *)0x9005000C
/* Interrupt controller FIQ pending register */
#define ICFP (unsigned *)0x90050010
/* Interrupt controller pending register */
#define ICPR (unsigned *)0x90050020

/* real time clock alarm register */
#define RTAR (unsigned *)0x90010000
/* real time clock count register */
#define RCNR (unsigned *)0x90010004
/* real time clock timer trim register */
#define RTTR (unsigned *)0x90010008
/* real time clock status register */
#define RTSR (unsigned *)0x90010010


/* OS timer match registers */
#define OSMR0 (unsigned *)0x90000000
#define OSMR1 (unsigned *)0x90000004
#define OSMR2 (unsigned *)0x90000008
#define OSMR3 (unsigned *)0x9000000C
/* OS timer counter register */
#define OSCR (unsigned *)0x90000010
/* OS timer status register */
#define OSSR (unsigned *)0x90000014
/* OS timer watchdog enable register */
#define OWER (unsigned *)0x90000018
/* OS timer interrupt enable register */
#define OIER (unsigned *)0x9000001C

/* interrupt bit for the OS timer match register 0 */
#define OSMR0_BIT 0x04000000

/*#define TIMER_INTERVAL_100MS (0x0005A000) */
#define TIMER_INTERVAL_100MS (32767)
#define TIMER_INTERVAL_1000MS (327670)

void start_timer();

int read_CPSR();

void write_CPSR(int v);

__inline void enable_IRQ(void);
__inline void disable_IRQ(void);
__inline void enable_FIQ(void);
__inline void disable_FIQ(void);

void SA1110_dcache_flush();
void SA1110_tlb_flush();
void SA1110_tlb_flush_addr(void *addr);

int SA1110_start_timer();
int SA1110_low_level_init();

#endif
