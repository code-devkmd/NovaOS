#ifndef ISR_H
#define ISR_H

void isr_init(void);
void isr_handler(unsigned int number, unsigned int error_code);

#endif
