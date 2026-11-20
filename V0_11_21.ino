// 실험 3

// --- 모터 드라이버 핀 설정 ---
#define ENA 6
#define EN1 7
#define EN2 3
#define EN3 4
#define EN4 2
#define ENB 5

// --- 속도 변수 설정 ---
int speed = 180;
int speedlow = 80;
int speedhigh = 255;

// --- 라인트레이서 센서 핀 설정 ---
#define LT_MODULE_L A1
#define LT_MODULE_F A2
#define LT_MODULE_R A0

// --- 차량 동작 상태 정의 ---
#define CAR_DIR_NONE 0
#define CAR_DIR_RF   1
#define CAR_DIR_ST   2
#define CAR_DIR_LF   3
#define CAR_DIR_FW   4

// 현재 차량 동작
int g_carDirection = CAR_DIR_ST;

// 마지막으로 라인이 검출된 좌우 방향
int g_lastLineDirection = CAR_DIR_NONE;

// --- 센서 감지 함수 ---
bool lt_isLeft() {
  int ret = analogRead(LT_MODULE_L);

  Serial.print("left: ");
  Serial.println(ret);

  return ret > 150;
}

bool lt_isRight() {
  int ret = analogRead(LT_MODULE_R);

  Serial.print("right: ");
  Serial.println(ret);

  return ret > 150;
}

bool lt_isForward() {
  int ret = analogRead(LT_MODULE_F);

  Serial.print("forward: ");
  Serial.println(ret);

  return ret > 50;
}
void stopmotors(){
  digitalWrite(EN1,LOW);
  digitalWrite(EN2,LOW);
  analogWrite(ENA,0);
  digitalWrite(EN3,LOW);
  digitalWrite(EN4,LOW);
  analogWrite(ENB,0);
  delay(20);
}
static void Goforward(){
    Serial.println("Forward");

    digitalWrite(EN1, HIGH);
    digitalWrite(EN2, LOW);
    analogWrite(ENA, speed);

    digitalWrite(EN3, HIGH);
    digitalWrite(EN4, LOW);
    analogWrite(ENB, speed);
    
    //delay(50);
    //stopmotors();
}
static void Goright(){
    Serial.println("RIGHT");

    digitalWrite(EN1, LOW);
    digitalWrite(EN2, HIGH);
    analogWrite(ENA, speedlow);

    digitalWrite(EN3, HIGH);
    digitalWrite(EN4, LOW);
    analogWrite(ENB, speedhigh);
    delay(1000);
}
static void Goleft(){
    Serial.println("LEFT");

    digitalWrite(EN1, HIGH);
    digitalWrite(EN2, LOW);
    analogWrite(ENA, speedhigh);

    digitalWrite(EN3, LOW);
    digitalWrite(EN4, HIGH);
    analogWrite(ENB, speedlow);
    delay(1000);
    
}

// --- 센서 입력에 따른 방향 갱신 ---
void lt_mode_update() {
  bool ll = lt_isLeft();
  bool ff = lt_isForward();
  bool rr = lt_isRight();

  // 111: 교차로 또는 굵은 라인
  if (ll && ff && rr) {
    g_carDirection = CAR_DIR_RF;
  }

  // 000: 라인을 완전히 잃음
  else if (!ll && !ff && !rr) {
    if (g_lastLineDirection == CAR_DIR_LF) {
      Serial.println("Line lost: search left");
      g_carDirection = CAR_DIR_LF;
    }
    else if (g_lastLineDirection == CAR_DIR_RF) {
      Serial.println("Line lost: search right");
      g_carDirection = CAR_DIR_RF;
    }
    else {
      // 프로그램 시작 후 아직 좌우 라인을
      // 한 번도 감지하지 못한 상태
      Serial.println("Line lost: no previous direction");
      g_carDirection = CAR_DIR_ST;
    }
  }

  // 010: 중앙
  else if (ff && !ll && !rr) {
    g_carDirection = CAR_DIR_FW;
  }

  // 100 또는 110: 라인이 왼쪽에 있음
  else if (ll && !rr) {
    g_carDirection = CAR_DIR_LF;

    // 라인 이탈 시 복구할 방향 저장
    g_lastLineDirection = CAR_DIR_LF;
  }

  // 001 또는 011: 라인이 오른쪽에 있음
  else if (rr && !ll) {
    g_carDirection = CAR_DIR_RF;

    // 라인 이탈 시 복구할 방향 저장
    g_lastLineDirection = CAR_DIR_RF;
  }

  // 101 등의 기타 패턴
  else {
    g_carDirection = CAR_DIR_FW;
  }
}

// --- 차량 구동 함수 ---
void car_update() {
  Serial.print("Car update: ");
  Serial.println(g_carDirection);

  if (g_carDirection == CAR_DIR_FW) {
    Goforward();
  }

  else if (g_carDirection == CAR_DIR_RF) {
    stopmotors();
    Goright();
    
    
  }

  else if (g_carDirection == CAR_DIR_LF) {
    stopmotors();
    Goleft();
    
    
  }

  else if (g_carDirection == CAR_DIR_ST) {
    Serial.println("Stop");

    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(ENA, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(EN3, OUTPUT);
  pinMode(EN4, OUTPUT);

  pinMode(LT_MODULE_L, INPUT);
  pinMode(LT_MODULE_F, INPUT);
  pinMode(LT_MODULE_R, INPUT);
}


void loop() {
  // 현재 센서값으로 방향을 먼저 판단
  lt_mode_update();

  // 판단 결과를 바로 모터에 반영
  car_update();
}