#include <iostream>
#include <memory>
#include <thread>
#include <string>

#include <stdio.h>
#include <unistd.h>

#include "InfoLed.h"

void test_led_on(std::shared_ptr<InfoLed> infoled, int color) {
    infoled->leds_multi_all_on(color);
}

void test_led_scroll(std::shared_ptr<InfoLed> infoled, uint64_t bitmap_color, uint32_t color, uint32_t  bg_color, uint32_t shift, uint32_t delay_ms) {
    infoled->leds_multi_set_scroll(bitmap_color, color, bg_color, shift, delay_ms);
}

void test_led_off(std::shared_ptr<InfoLed> infoled) {
    infoled->leds_multi_all_off();
}

int main(int argc, char** argv)
{
    int ledNumber;
    auto infoled = std::make_shared<InfoLed>();

    /**
     *
     * enum {
     *     LED_MULTI_PURE_COLOR_GREEN = 0,
     *     LED_MULTI_PURE_COLOR_RED,
     *     LED_MULTI_PURE_COLOR_BLUE,
     *     LED_MULTI_PURE_COLOR_WHITE,
     *     LED_MULTI_PURE_COLOR_BLACK,
     *     LED_MULTI_PURE_COLOR_NON_GREEN,
     *     LED_MULTI_PURE_COLOR_NON_RED,
     *     LED_MULTI_PURE_COLOR_NON_BLUE,
     *     LED_MULTI_PURE_COLOR_MAX,
     * };
     *
     */
    ledNumber = infoled->leds_multi_init();
    if(ledNumber < 0) {
        printf("led init failed\n");
        return -1;
    }

    printf("Led on...\n");
    test_led_on(infoled, LED_MULTI_PURE_COLOR_RED);

    sleep(3);

    printf("Led start scrolling...\n");
    int m_scrollLedNum = 3;
    int bitmap = (1 << m_scrollLedNum) -1;
    test_led_scroll(infoled, bitmap, LED_MULTI_PURE_COLOR_WHITE, LED_MULTI_PURE_COLOR_BLUE, 3, 80);
    sleep(8);

    printf("Led off...\n");
    test_led_off(infoled);

    for (;;){
    }

    return 0;
}
