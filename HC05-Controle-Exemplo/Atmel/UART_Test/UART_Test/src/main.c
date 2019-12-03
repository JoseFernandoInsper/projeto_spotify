#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <asf.h>
#include <string.h>
#include "io_sam.h"

// Descomente o define abaixo, para desabilitar o Bluetooth e utilizar modo Serial via Cabo
//#define DEBUG_SERIAL

#define STRING_EOL    "\r"
#define STRING_HEADER "-- AFEC Temperature Sensor Example --\r\n" \
"-- "BOARD_NAME" --\r\n" \
"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

/** Reference voltage for AFEC,in mv. */
#define VOLT_REF        (3300)

/** The maximal digital value */
/** 2^12 - 1                  */
#define MAX_DIGITAL     (4095)

/** The conversion data is done flag */
volatile bool g_is_conversion_done = false;

/** The conversion data value */
volatile uint32_t g_ul_value = 0;

volatile bool g_att_temp = true;

/* Canal do sensor de temperatura */
#define AFEC_CHANNEL_TEMP_SENSOR 6


#ifdef DEBUG_SERIAL
#define UART_COMM USART1
#else
#define UART_COMM USART0
#endif

volatile long g_systimer = 0;

volatile Bool but1_flag;
volatile Bool but2_flag;
volatile Bool but3_flag;
volatile Bool but4_flag;
volatile Bool but5_flag;
volatile unsigned int but_status;
#define BUT1_ID '0'
#define BUT2_ID '1'
#define BUT3_ID '2'
#define BUT4_ID '3'
#define BUT5_ID '4'

void but1_callback(void)
{
	but1_flag = true;
	but_status = !pio_get(BUT1_PIO, PIO_INPUT, BUT1_IDX_MASK);
}
void but2_callback(void)
{
	but2_flag = true;
	but_status = !pio_get(BUT2_PIO, PIO_INPUT, BUT2_IDX_MASK);
}
void but3_callback(void)
{
	but3_flag = true;
	but_status = !pio_get(BUT3_PIO, PIO_INPUT, BUT3_IDX_MASK);
}
void but4_callback(void)
{
	but4_flag = true;
	but_status = !pio_get(BUT4_PIO, PIO_INPUT, BUT4_IDX_MASK);
}
void but5_callback(void)
{
	but5_flag = true;
	but_status = !pio_get(BUT5_PIO, PIO_INPUT, BUT5_IDX_MASK);
}

void SysTick_Handler() {
	g_systimer++;
}

static void AFEC_Temp_callback(void)
{
	g_ul_value = afec_channel_get_value(AFEC1, AFEC_CHANNEL_TEMP_SENSOR);
	g_is_conversion_done = true;
}


static void configure_console(void)
{

	const usart_serial_options_t uart_serial_options = {
		.baudrate   = CONF_UART_BAUDRATE,
		.charlength = 8,
		.paritytype = CONF_UART_PARITY,
		.stopbits   = 1,
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}



void usart_put_string(Usart *usart, char str[]) {
	usart_serial_write_packet(usart, str, strlen(str));
}

int usart_get_string(Usart *usart, char buffer[], int bufferlen, int timeout_ms) {
	long timestart = g_systimer;
	uint32_t rx;
	uint32_t counter = 0;
	
	while(g_systimer - timestart < timeout_ms && counter < bufferlen - 1) {
		if(usart_read(usart, &rx) == 0) {
			//timestart = g_systimer; // reset timeout
			buffer[counter++] = rx;
		}
	}
	buffer[counter] = 0x00;
	return counter;
}

void usart_send_command(Usart *usart, char buffer_rx[], int bufferlen, char buffer_tx[], int timeout) {
	usart_put_string(usart, buffer_tx);
	usart_get_string(usart, buffer_rx, bufferlen, timeout);
}

void usart_log(char* name, char* log) {
	usart_put_string(USART1, "[");
	usart_put_string(USART1, name);
	usart_put_string(USART1, "] ");
	usart_put_string(USART1, log);
	usart_put_string(USART1, "\r\n");
}

void hc05_config_server(void) {
	usart_serial_options_t config;
	config.baudrate = 9600;
	config.charlength = US_MR_CHRL_8_BIT;
	config.paritytype = US_MR_PAR_NO;
	config.stopbits = false;
	usart_serial_init(USART0, &config);
	usart_enable_tx(USART0);
	usart_enable_rx(USART0);
	
	 // RX - PB0  TX - PB1 
	 pio_configure(PIOB, PIO_PERIPH_C, (1 << 0), PIO_DEFAULT);
	 pio_configure(PIOB, PIO_PERIPH_C, (1 << 1), PIO_DEFAULT);
}

int hc05_server_init(void) {
	char buffer_rx[128];
	usart_send_command(USART0, buffer_rx, 1000, "AT", 1000);
	usart_send_command(USART0, buffer_rx, 1000, "AT", 1000);	
	usart_send_command(USART0, buffer_rx, 1000, "AT+NAMEServer", 1000);
	usart_log("hc05_server_init", buffer_rx);
	usart_send_command(USART0, buffer_rx, 1000, "AT", 1000);
	usart_send_command(USART0, buffer_rx, 1000, "AT+PIN0000", 1000);
	usart_log("hc05_server_init", buffer_rx);
}

void serial_but(char id, char status, char eof) {
	while(!usart_is_tx_ready(UART_COMM));
	usart_write(UART_COMM, id);
	
	while(!usart_is_tx_ready(UART_COMM));
	usart_write(UART_COMM, status);
	
	while(!usart_is_tx_ready(UART_COMM));
	usart_write(UART_COMM, eof);
	
}

static int32_t convert_adc_to_temp(int32_t ADC_value){

  int32_t ul_vol;
  int32_t ul_temp;
	ul_vol = ADC_value * VOLT_REF / (float) MAX_DIGITAL;
  ul_temp = (int)((float)(10000*ul_vol)/4096.0);
  return(ul_temp);
}

static void config_ADC_TEMP(void){

	afec_enable(AFEC1);

	/* struct de configuracao do AFEC */
	struct afec_config afec_cfg;

	/* Carrega parametros padrao */
	afec_get_config_defaults(&afec_cfg);

	/* Configura AFEC */
	afec_init(AFEC1, &afec_cfg);

	/* Configura trigger por software */
	afec_set_trigger(AFEC1, AFEC_TRIG_SW);

	/* configura call back */
	afec_set_callback(AFEC1, AFEC_INTERRUPT_EOC_6,	AFEC_Temp_callback, 1);

	/*** Configuracao específica do canal AFEC ***/
	struct afec_ch_config afec_ch_cfg;
	afec_ch_get_config_defaults(&afec_ch_cfg);
	afec_ch_cfg.gain = AFEC_GAINVALUE_0;
	afec_ch_set_config(AFEC1, AFEC_CHANNEL_TEMP_SENSOR, &afec_ch_cfg);
	afec_channel_set_analog_offset(AFEC1, AFEC_CHANNEL_TEMP_SENSOR, 0x200);

	struct afec_temp_sensor_config afec_temp_sensor_cfg;

	afec_temp_sensor_get_config_defaults(&afec_temp_sensor_cfg);
	afec_temp_sensor_set_config(AFEC1, &afec_temp_sensor_cfg);

	/* Selecina canal e inicializa conversão */
	afec_channel_enable(AFEC1, AFEC_CHANNEL_TEMP_SENSOR);
}

void TC0_Handler(void){
	volatile uint32_t ul_dummy;

	ul_dummy = tc_get_status(TC0, 0);

	UNUSED(ul_dummy);
	
	afec_start_software_conversion(AFEC1);

}

void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq){
	uint32_t ul_div;
	uint32_t ul_tcclks;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();

	uint32_t channel = 1;
	pmc_enable_periph_clk(ID_TC);

	tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
	tc_init(TC, TC_CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
	tc_write_rc(TC, TC_CHANNEL, (ul_sysclk / ul_div) / freq);

	NVIC_EnableIRQ((IRQn_Type) ID_TC);
	tc_enable_interrupt(TC, TC_CHANNEL, TC_IER_CPCS);

	tc_start(TC, TC_CHANNEL);
}

void interrupt_init() {
	pio_handler_set(BUT1_PIO, BUT1_PIO_ID, BUT1_IDX_MASK, PIO_IT_EDGE, but1_callback);
	pio_enable_interrupt(BUT1_PIO, BUT1_IDX_MASK);
	NVIC_EnableIRQ(BUT1_PIO_ID);
	NVIC_SetPriority(BUT1_PIO_ID, 4);
	
	pio_handler_set(BUT2_PIO, BUT2_PIO_ID, BUT2_IDX_MASK, PIO_IT_EDGE, but2_callback);
	pio_enable_interrupt(BUT2_PIO, BUT2_IDX_MASK);
	NVIC_EnableIRQ(BUT2_PIO_ID);
	NVIC_SetPriority(BUT2_PIO_ID, 4);
	
	pio_handler_set(BUT3_PIO, BUT3_PIO_ID, BUT3_IDX_MASK, PIO_IT_EDGE, but3_callback);
	pio_enable_interrupt(BUT3_PIO, BUT3_IDX_MASK);
	NVIC_EnableIRQ(BUT3_PIO_ID);
	NVIC_SetPriority(BUT3_PIO_ID, 4);
	
	pio_handler_set(BUT4_PIO, BUT4_PIO_ID, BUT4_IDX_MASK, PIO_IT_EDGE, but4_callback);
	pio_enable_interrupt(BUT4_PIO, BUT4_IDX_MASK);
	NVIC_EnableIRQ(BUT4_PIO_ID);
	NVIC_SetPriority(BUT4_PIO_ID, 4);
	
	pio_handler_set(BUT5_PIO, BUT5_PIO_ID, BUT5_IDX_MASK, PIO_IT_EDGE, but5_callback);
	pio_enable_interrupt(BUT5_PIO, BUT5_IDX_MASK);
	NVIC_EnableIRQ(BUT5_PIO_ID);
	NVIC_SetPriority(BUT5_PIO_ID, 4);
}

int main (void)
{
	board_init();
	ioport_init();
	sysclk_init();
	configure_console();
	puts(STRING_HEADER);	
	
	/////led///////
	pmc_enable_periph_clk(LED_PIO_ID);
	pmc_enable_periph_clk(LED2_PIO_ID);
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pmc_enable_periph_clk(BUT3_PIO_ID);
	pmc_enable_periph_clk(BUT4_PIO_ID);
	pmc_enable_periph_clk(BUT5_PIO_ID);
	pio_set_output(LED_PIO, LED_PIN_MASK, 1, 0, 0);
	pio_set_output(LED2_PIO, LED2_PIN_MASK, 1, 0, 0);
	pio_set_input(BUT1_PIO, BUT1_IDX_MASK, PIO_PULLUP);
	pio_set_input(BUT2_PIO, BUT2_IDX_MASK, PIO_PULLUP);
	pio_set_input(BUT3_PIO, BUT3_IDX_MASK, PIO_PULLUP);
	pio_set_input(BUT4_PIO, BUT4_IDX_MASK, PIO_PULLUP);
	pio_set_input(BUT5_PIO, BUT5_IDX_MASK, PIO_PULLUP);
	pio_clear(LED2_PIO, LED2_PIN_MASK);
	
	interrupt_init();

	TC_init(TC0, ID_TC0, 0, 1);
	
	SysTick_Config(sysclk_get_cpu_hz() / 1000); // 1 ms
	
	config_ADC_TEMP();
		/* incializa conversão ADC */
	afec_start_software_conversion(AFEC1);
	
	#ifndef DEBUG_SERIAL
	usart_put_string(USART1, "Inicializando...\r\n");
	usart_put_string(USART1, "Config HC05 Server...\r\n");
	hc05_config_server();
	hc05_server_init();
	#endif
	char button1 = '0';
	char button2 = '0';
	char eof = 'X';
	char buffer[1024];
	
	int range, rangeOld ;
	
	while(1) {
	     if(g_is_conversion_done == true) {
			 
				if(g_ul_value < 408 )
					range = 0;
				else if (408 < g_ul_value && g_ul_value < 816 )
					range = 1;
				else if (816 < g_ul_value && g_ul_value < 1228 )
					range = 2;
				else if (1228 < g_ul_value && g_ul_value < 1636 )
					range = 3;
				else if (1636 < g_ul_value && g_ul_value < 2048 )
					range = 4;
				else if (2048 < g_ul_value && g_ul_value < 2456 )
					range = 5;
				else if (2456 < g_ul_value && g_ul_value < 2864 )
					range = 6;
				else if (2864 < g_ul_value && g_ul_value < 3272 )
					range = 7;
				else if (3272 < g_ul_value && g_ul_value < 3680 )
					range = 8;
				else if (3680 < g_ul_value && g_ul_value < 4096 )
				range = 9;
					
				if(range != rangeOld){
					serial_but('A' , range+0x30, eof);
					rangeOld = range;
				}
			
			g_is_conversion_done = false;
			
		

		}
		if (but1_flag) {
			serial_but(BUT1_ID , but_status+0x30, eof);
			but1_flag = false;
		}
		if (but2_flag) {
			serial_but(BUT2_ID , but_status+0x30, eof);
			but2_flag = false;
		}
		if (but3_flag) {
			serial_but(BUT3_ID , but_status+0x30, eof);
			but3_flag = false;
		}
		if (but4_flag) {
			serial_but(BUT4_ID , but_status+0x30, eof);
			but4_flag = false;
		}
		if (but5_flag) {
			serial_but(BUT5_ID , but_status+0x30, eof);
			but5_flag = false;
			pio_set(LED2_PIO, LED2_PIN_MASK);
		}
	}
}
