/* Basic example for the Push Button
 *  
 *  Wire an interrupt to Pin 11
 *  In main loop, LED lights up 
 *  when button is pressed
 *  
 */

#define LED_RED 5
#define LED_GREEN 3
#define LED_BLUE 2
#define BUTTON 11

static int button_state = 0;
static long button_state_length = 0;
static long button_tmp_time = 0;

void isr_button()
{
  if(digitalRead(BUTTON))
  {
    button_state = 1;
    button_tmp_time = millis();
  }
  else
  {
    button_state = 0;
  }
}

void setup()
{
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  /* Button is in pull down */
  pinMode(BUTTON, INPUT);

  /* Detect button event */
  attachInterrupt(BUTTON, isr_button, CHANGE);
}

void loop()
{
  /* Follow button_state with various LED depending on duration */
  if(button_state == 1)
  {
    button_state_length = millis() - button_tmp_time;

    if (button_state_length > 2000)
    {
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_BLUE, LOW);
      digitalWrite(LED_RED, HIGH);
    }
    else if (button_state_length > 500)
    {
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_BLUE, HIGH);
      digitalWrite(LED_RED, LOW);
    }
    else
    {
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_BLUE, LOW);
      digitalWrite(LED_RED, LOW);
    }

  }
  else
  {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_RED, LOW);
  }
}

