#include <driver/i2s.h>
#include <soc/syscon_reg.h>
#include <driver/adc.h>
#include "driver/ledc.h"

#define ICG_PIN 25
#define CLK_PIN 26
#define SH_PIN 27
#define INPUT_PIN 32

#define I2S_SAMPLE_RATE 200000
#define N_SAMPLES 1200 //3648
#define ADC_INPUT ADC1_CHANNEL_4 //pin 32
#define OUTPUT_PIN 27
#define OUTPUT_VALUE 3800
#define READ_DELAY 800 

uint16_t adc_reading;
uint8_t sampleFlag = 0;

uint32_t integration_time=200; //in µs
volatile uint32_t timebase=10; // in µs (100)
volatile uint32_t SH_ticks;
volatile uint32_t SH_ticks_L=1; // equals SH pulse width (in 'ticks')
volatile uint32_t SH_ticks_H=integration_time/timebase; // equals SH period (in 'ticks')
volatile bool SH_is_high=false;
volatile uint32_t ICG_ticks;
volatile uint32_t ICG_ticks_L=50*SH_ticks_H; // equals sampling period (in 'ticks'), must be multiple of SH_ticks_H and above 5ms/timebase (which is the minimum readout duration)
volatile uint32_t ICG_ticks_H=2*SH_ticks_L; // equals integration clear duration (in 'ticks', uncritical and typically 2*SH_ticks_L)
volatile bool ICG_is_high=true;

volatile uint8_t dataBuff[N_SAMPLES];

hw_timer_t * signal_timer = NULL; // our timer
portMUX_TYPE DRAM_ATTR timerMux = portMUX_INITIALIZER_UNLOCKED; 

ledc_timer_config_t clock_timer;
ledc_channel_config_t clock_pwm_config;

void IRAM_ATTR onTimer() {
//Serial.println("tick");
  portENTER_CRITICAL_ISR(&timerMux);

  SH_ticks++;
  if (SH_ticks>SH_ticks_H && !SH_is_high) {
    GPIO.out_w1ts = (1 << SH_PIN);
    SH_ticks=0;
    SH_is_high=true;
  }
  else if (SH_ticks>SH_ticks_L && SH_is_high) {
    GPIO.out_w1tc = (1 << SH_PIN);
    SH_is_high=false;
  }
  
  ICG_ticks++;
  if (ICG_ticks>ICG_ticks_L && ICG_is_high) {
    GPIO.out_w1tc = (1 << ICG_PIN);
    ICG_ticks=0;
    ICG_is_high=false;
  }
  else if (ICG_ticks>ICG_ticks_H && !ICG_is_high) {
    GPIO.out_w1ts = (1 << ICG_PIN);
    // here I want to trigger DMA ADC readout
    sampleFlag = 1;
    
//    i2s_adc_disable(I2S_NUM_0);
//    i2s_adc_enable(I2S_NUM_0);
//    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    ICG_is_high=true;
  }
  
  portEXIT_CRITICAL_ISR(&timerMux);
//Serial.println("tock");
}

void i2sInit()
{
   i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
    .sample_rate =  I2S_SAMPLE_RATE,              // The format of the signal using ADC_BUILT_IN
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 2,
    .dma_buf_len = 1000,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
   };
   adc1_config_width(ADC_WIDTH_12Bit);
   adc1_config_channel_atten(ADC_INPUT, ADC_ATTEN_11db);
   SET_PERI_REG_MASK(SYSCON_SARADC_CTRL2_REG, SYSCON_SARADC_SAR1_INV);
  
   i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
   i2s_set_adc_mode(ADC_UNIT_1, ADC_INPUT);
   i2s_adc_enable(I2S_NUM_0);

   Serial.println("I2S-ADC started");
}

void driving_signal_init() {
  pinMode(ICG_PIN,OUTPUT);
  pinMode(CLK_PIN,OUTPUT);
  pinMode(SH_PIN,OUTPUT);
  digitalWrite(ICG_PIN,HIGH);
  digitalWrite(CLK_PIN,HIGH);
  digitalWrite(SH_PIN,LOW);
  
  signal_timer = timerBegin(1, 80, true); // 80 MHz / 8000 = 10 kHz hardware (minimum integration time 100µs)
  timerAttachInterrupt(signal_timer, &onTimer, false); // Attaches the handler function to the timer 
  timerAlarmWrite(signal_timer, timebase, true); // Interrupts when counter == 45, i.e. 22.222 times a second
  timerAlarmEnable(signal_timer);
  
  ledcSetup(2, 3200000, 1);
  ledcAttachPin(CLK_PIN, 2);
  ledcWrite(2, 1);
}

void reader(void *pvParameter) {
  uint32_t read_counter = 0;
  uint64_t read_sum = 0;
// The 4 high bits are the channel, and the data is inverted
  uint16_t offset = (int)ADC_INPUT * 0x1000 + 0xFFF;
  size_t bytes_read;
  int i = 0;
  
   while(1){
       if (*((uint8_t*)pvParameter)!=0){ // check sample flag
        uint16_t buffer[2] = {0};
        i2s_read(I2S_NUM_0, &buffer, sizeof(buffer), &bytes_read, 15);
        if (bytes_read == sizeof(buffer)) {
          read_sum += offset - buffer[0];
          read_sum += offset - buffer[1];
          if (i < N_SAMPLES){
            dataBuff[i++] = ((offset - buffer[0]) >> 4); // convert to 8 bit
          }
          read_counter++;
        } else {
          Serial.println("buffer empty");
        }
        if (read_counter == N_SAMPLES) {
          adc_reading = read_sum / N_SAMPLES / 2;
          read_counter = 0;
          i = 0;
          read_sum = 0;
          sampleFlag = 0;
        }
      }else{
        yield();
      }
    }
}

void setup() {
  Serial.begin(500000);
  // Put a signal out on pin 
  uint32_t freq = ledcSetup(0, I2S_SAMPLE_RATE, 10);
  Serial.printf("Output frequency: %d\n", freq);
  ledcWrite(0, OUTPUT_VALUE/4);
  ledcAttachPin(OUTPUT_PIN, 0);
  // Initialize the I2S peripheral
  i2sInit();
  driving_signal_init();
  // Create a task that will read the data
  xTaskCreatePinnedToCore(reader, "ADC_reader", 4096, (void*)&sampleFlag, 1, NULL, 1);
  sampleFlag = 0;
}

void loop() {
  delay(900);
  for(int j = 0; j < 500;j++) // serial plotter can only display 500 pixels at a time
  {
    Serial.println(dataBuff[j]);
  }
}
