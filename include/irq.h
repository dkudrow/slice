/* include/irq.h -- TODO */

#ifndef IRQ_H
#define IRQ_H

typedef void (*irq_service_routine_t)(void);

void irq_init();
void irq_register_service_routine(irq_service_routine_t isr, int pos);

#endif /* IRQ_H */

