	/*
 * io.h
 *
 * Created: 18/10/2019 10:48:18
 *  Author: 55129
 */ 

#define LED_PIO           PIOC
#define LED_PIO_ID        ID_PIOC
#define LED_PIO_IDX       8u
#define LED_PIN_MASK  (1u << LED_PIO_IDX)

#define LED1_PIO_ID	   ID_PIOC
#define LED1_PIO        PIOC
#define LED1_PIN		   19
#define LED1_PIN_MASK   (1<<LED1_PIN)

#define LED2_PIO_ID	    ID_PIOD
#define LED2_PIO        PIOD
#define LED2_PIN		26
#define LED2_PIN_MASK   (1<<LED2_PIN)

#define BUT1_PIO      PIOA
#define BUT1_PIO_ID   ID_PIOA
#define BUT1_IDX      2
#define BUT1_IDX_MASK (1 << BUT1_IDX)

#define BUT2_PIO      PIOD
#define BUT2_PIO_ID   ID_PIOD
#define BUT2_IDX      30
#define BUT2_IDX_MASK (1 << BUT2_IDX)

#define BUT3_PIO      PIOC
#define BUT3_PIO_ID   ID_PIOC
#define BUT3_IDX      13
#define BUT3_IDX_MASK (1 << BUT3_IDX)

#define BUT4_PIO		PIOA
#define BUT4_PIO_ID		ID_PIOA
#define BUT4_IDX		24
#define BUT4_IDX_MASK	(1u << BUT4_IDX)

#define BUT5_PIO		PIOA
#define BUT5_PIO_ID		ID_PIOA
#define BUT5_IDX		4
#define BUT5_IDX_MASK	(1u << BUT5_IDX)