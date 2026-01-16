/* replace ? and add code in ???*/
#include <Arduino.h>
#include "NewPing.h"
#include "pitches.h" 

// 引脚定义
#define BUZZER_PIN 35
#define TRIG_PIN 4                  
#define ECHO_PIN 5  
#define LED 1  // 内置LED引脚

// 超声波最大检测距离（厘米）
#define MAX_DISTANCE 400

// 初始化超声波传感器
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

// 1. 高音版《Jingle Bells》旋律
int melody_high[] = {
  NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_G4, NOTE_C4, NOTE_D4, NOTE_E4,
  NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_G4
};
// 高音音符时长
int noteDurations_high[] = {
  8, 8, 4, 8, 8, 4, 8, 8, 8, 8, 2, 8, 8, 4, 8, 8, 4, 8, 8, 8, 4, 8, 8, 8, 2, 4
};

// 2. 低音版《Jingle Bells》旋律（降低一个八度）
int melody_low[] = {
  NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_G3, NOTE_C3, NOTE_D3, NOTE_E3,
  NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_D3, NOTE_D3, NOTE_E3, NOTE_D3, NOTE_G3
};
// 低音音符时长（与高音版一致）
int noteDurations_low[] = {
  8, 8, 4, 8, 8, 4, 8, 8, 8, 8, 2, 8, 8, 4, 8, 8, 4, 8, 8, 8, 4, 8, 8, 8, 2, 4
};

// 预计算总音符数（全局常量，避免函数内失效）
const int TOTAL_NOTES_HIGH = sizeof(melody_high) / sizeof(melody_high[0]);
const int TOTAL_NOTES_LOW = sizeof(melody_low) / sizeof(melody_low[0]);

// 播放音乐的通用函数（新增总音符数参数，解决数组长度计算失效问题）
void playJingleBells(int melody[], int noteDurations[], int totalNotes, String toneType) {
  Serial.print("Playing ");
  Serial.print(toneType);
  Serial.println("-tone Jingle Bells...");
  
  // 循环次数由传入的totalNotes指定，不再依赖函数内sizeof
  for (int i = 0; i < totalNotes; i++) {
    int noteDuration = 1000 / noteDurations[i];
    // 播放单个音符：tone(蜂鸣器引脚, 音符频率, 音符时长)
    tone(BUZZER_PIN, melody[i], noteDuration);
    // 音符间隔：1.3倍时长避免粘连，确保旋律清晰
    delay(noteDuration * 1.3);
    // 停止当前音符发声，为下一个音符做准备
    noTone(BUZZER_PIN);
  }
  delay(3000); // 播放完成后延迟3秒，避免重复播放过快
}

void setup() {
  Serial.begin(9600);
  // 初始化引脚为输出模式
  pinMode(LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  // 读取超声波距离（排除0值，避免无效读取干扰逻辑）
  unsigned int distance = sonar.ping_cm();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // 流程图逻辑判断（确保LED和音乐逻辑正确）
  if (distance < 10 && distance != 0) { // 距离 < 10cm（有效读取）
    digitalWrite(LED, LOW); // 关闭LED
    
    if (distance < 5) { // 距离 < 5cm：播放低音版
      playJingleBells(melody_low, noteDurations_low, TOTAL_NOTES_LOW, "low");
    } else { // 5cm ≤ 距离 < 10cm：播放高音版
      playJingleBells(melody_high, noteDurations_high, TOTAL_NOTES_HIGH, "high");
    }
  } else { // 距离 ≥ 10cm：打开LED，不播放音乐
    digitalWrite(LED, HIGH);
  }

  delay(500); // 延迟0.5秒，避免超声波读取过快
}
