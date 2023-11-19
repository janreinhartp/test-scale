#include <Arduino.h>
#include "U8g2lib.h"

#include "HX711.h"

#define DOUT 3
#define CLK 2

HX711 scale;

float calibration_factor = -7050; //-7050 worked for my 440lb max scale setup
float currentWeight = 0;

#define BUTTON_UP_PIN 4
#define BUTTON_SELECT_PIN 3
#define BUTTON_DOWN_PIN 2

int button_up_clicked = 0;
int button_select_clicked = 0;
int button_down_clicked = 0;

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

const unsigned char epd_bitmap_gram_icon[] PROGMEM = {
	0x00, 0x3f, 0x00, 0x00, 0x21, 0x00, 0xc0, 0xff, 0x00, 0xe0, 0xff, 0x01, 0xf0, 0xff, 0x03, 0x70,
	0x80, 0x03, 0x38, 0x00, 0x07, 0x38, 0xff, 0x07, 0x3c, 0xff, 0x0f, 0x3c, 0xff, 0x0f, 0x3e, 0x0f,
	0x1f, 0x3e, 0x0f, 0x1f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x00, 0x3f, 0x7e, 0x80, 0x1f, 0xfc, 0xff, 0x0f, 0xf8,
	0xff, 0x07};

void setup()
{
    Serial.begin(9600);
    Serial.println("HX711 calibration sketch");
    Serial.println("Remove all weight from scale");
    Serial.println("After readings begin, place known weight on scale");
    Serial.println("Press + or a to increase calibration factor");
    Serial.println("Press - or z to decrease calibration factor");

    scale.begin(DOUT, CLK);
    scale.set_scale();
    scale.tare(); // Reset the scale to 0

    long zero_factor = scale.read_average(); // Get a baseline reading
    Serial.print("Zero factor: ");           // This can be used to remove the need to tare the scale. Useful in permanent scale projects.
    Serial.println(zero_factor);
}

void loop()
{
    scale.set_scale(calibration_factor); // Adjust to this calibration factor
    currentWeight = scale.get_units();
    Serial.print("Reading: ");
    Serial.print(currentWeight, 1);
    Serial.print(" kg"); // Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
    Serial.print(" calibration_factor: ");
    Serial.print(calibration_factor);
    Serial.println();

    printCalibration(scale.get_units(), calibration_factor);

    if ((digitalRead(BUTTON_UP_PIN) == LOW) && (button_up_clicked == 0))
    {
        button_up_clicked = 1;
        calibration_factor += 10;
    }
    else if ((digitalRead(BUTTON_DOWN_PIN) == LOW) && (button_down_clicked == 0))
    {
        button_down_clicked = 1;
        calibration_factor -= 10;
    }
    
    if ((digitalRead(BUTTON_UP_PIN) == HIGH) && (button_up_clicked == 1))
    {
        button_up_clicked = 0;
    }
    if ((digitalRead(BUTTON_DOWN_PIN) == HIGH) && (button_down_clicked == 1))
    {
        button_down_clicked = 0;
    }
}

void printCalibration(float currentWeight, float currentCalibration){
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_t0_14b_tf);
    u8g2.drawStr(getXCoordinateCenter("Calibrattion"), 15, "Calibrattion");

    char *jobArr;
	itoa(currentCalibration,jobArr,10);
	u8g2.setFont(u8g2_font_t0_14b_tf);
	u8g2.drawStr(15, 15 + 15, jobArr);

    u8g2.drawXBMP(4, 38, 22, 22, epd_bitmap_gram_icon);
    char cstr[10];
	itoa(currentWeight, cstr, 10);
	String g = " G";
	strcat(cstr, g.c_str());
    u8g2.setFont(u8g2_font_logisoso22_tr);
	u8g2.drawStr(4 + 22 + 4 + 10, 38 + 22, cstr);

    u8g2.sendBuffer();
}

int getXCoordinateCenter(String text)
{
	char *char_array = new char[text.length() + 1];
	strcpy(char_array, text.c_str());
	return (128 - u8g2.getStrWidth(char_array)) / 2;
}