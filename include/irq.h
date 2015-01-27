/* include/irq.h -- TODO */

#ifndef IRQ_H
#define IRQ_H

typedef void (*irq_handler_t)(void);

void irq_init();
void irq_register_handler(irq_handler_t handler, int pos);

#endif /* IRQ_H */

