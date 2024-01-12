


#include "blood.h"
#include "max30100.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "lcd.h"
#include "esp_log.h"

#include "lv_port_disp_template.h"
#include "lvgl.h"
//#include "lvgl/demos/lv_demos.h"


static const char* TAG = "MAIN";

void blood_task(void)
{

    max30100_init();
    blood_Setup();
    while (1)
    {
        //app init 
	   
        blood_Loop();
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    
	
}


void lcd_init_now(void)
{
    //ESP_LOGI(TAG,"Lcd_Init in");
    lcd_config();
    //ESP_LOGI(TAG,"Lcd_Init out");
    
    //ESP_LOGI(TAG,"Lcd_Clear out");
    set_background(WHITE);
    while(1)
    {
        static int x = 0;
        static int y = 0;
        static int t=0;
        //ESP_LOGI(TAG,"XXX");
        //
        //set_background(RED);
        set_background(WHITE);
        set_sin(40,1,RED,t);
        set_line(X_CENTER,0,X_CENTER,127,GREEN);
        vTaskDelay(pdMS_TO_TICKS(1));
        x++;y++;
        t++;
    }

}
static void lvgl_init( void ) 
{
    lv_init();
    lv_port_disp_init();        // 显示器初始化
    // lv_port_indev_init();       // 输入设备初始化
    // lv_port_fs_init();          // 文件系统设备初始化
}
lv_obj_t * label2 = NULL;
static void lv_ex_label_1(void)
{
    label2 = lv_label_create(lv_scr_act());
    //lv_label_set_recolor(label2, true);
    //lv_label_set_long_mode(label2, LV_LABEL_LONG_SROLL_CIRC); /*Circular scroll*/
    lv_obj_set_width(label2, 160);
    lv_obj_set_height(label2, 160);
    // Hello world ! Trisuborn.
    lv_label_set_text(label2, "#ff0000 Hello# #00ff00 world ! Trisuborn.#");
    lv_obj_align(label2, LV_ALIGN_CENTER, 0, 0);
}

void lv_task(void)
{
    lvgl_init();
    set_background(BLUE);
    lv_ex_label_1();
    int x = 0,y=0;
	while(1) {
        ESP_LOGI(TAG,"XXX %d:%d",x,y);
		// // 先调用 lv_tick_inc 再调用 lv_task_handler
		 //lv_tick_inc(5);
		 lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(5));	// 可以省略，lvgl并不是OS的真正任务
        lv_obj_align(label2, LV_ALIGN_CENTER, x, y);
        x++;
        y++;
	}
}
void app_main(void)
{
   
    xTaskCreatePinnedToCore(lcd_init_now, "lcd_init_now", 4096, NULL, 0, NULL, 0);
    xTaskCreate(blood_task, "blood_task", 4096, NULL, 0, NULL);
    //xTaskCreate(lv_task, "lv_task", 4096, NULL, 0, NULL);

    
}