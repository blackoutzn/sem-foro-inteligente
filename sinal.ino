const int ledVerde = 9;
const int ledAmarelo = 8;
const int ledVermelho = 7;
const int botao = 2;
const int buzzer = 10;

int carros_detectados = 0;
unsigned long tempo_inicio = 0;
unsigned long duracao_verde = 6000;
unsigned long tempo_amarelo = 0;
unsigned long tempo_vermelho = 0;

bool em_espera = false;
bool ledsLigados = true;
bool ultimoEstadoBotao = HIGH;
bool bipesDadosAmarelo = false;
bool bipesDadosVermelho = false;
bool buzzerConectado = false;
bool bipesDadosVerde = false;

void apitarBuzzer(int vezes, int frequencia, int duracao, int pausa) {
  for (int i = 0; i < vezes; i++) {
    tone(buzzer, frequencia);
    delay(duracao);
    noTone(buzzer);
    delay(pausa);
  }
}

void setup() {
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(botao, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledAmarelo, LOW);
  digitalWrite(ledVermelho, HIGH);
}

void loop() {
  bool estadoAtualBotao = digitalRead(botao);
  if (estadoAtualBotao == LOW && ultimoEstadoBotao == HIGH) {
    ledsLigados = !ledsLigados;
    if (!ledsLigados) {
      digitalWrite(ledVerde, LOW);
      digitalWrite(ledAmarelo, LOW);
      digitalWrite(ledVermelho, LOW);
      noTone(buzzer);
    } else {
      digitalWrite(ledVermelho, HIGH);
    }
    delay(200);
  }
  ultimoEstadoBotao = estadoAtualBotao;

  if (!ledsLigados) return;

  if (Serial.available() > 0) {
    if (!buzzerConectado) {
      apitarBuzzer(2, 700, 150, 150);
      buzzerConectado = true;
    }
    carros_detectados = Serial.parseInt();
    if (carros_detectados == 1) duracao_verde = 6000;
    else if (carros_detectados == 2) duracao_verde = 12000;
    else if (carros_detectados == 3) duracao_verde = 18000;
  }

  if (digitalRead(ledVermelho) == HIGH && millis() - tempo_vermelho >= 4000) {
    em_espera = false;
    if (carros_detectados > 0) {
      digitalWrite(ledVermelho, LOW);
      digitalWrite(ledVerde, HIGH);
      tempo_inicio = millis();
      bipesDadosVerde = false;
    }
  }

  if (digitalRead(ledVerde) == HIGH) {
    if (!bipesDadosVerde) {
      apitarBuzzer(2, 1200, 200, 200);
      bipesDadosVerde = true;
    }
    if (millis() - tempo_inicio >= duracao_verde) {
      digitalWrite(ledVerde, LOW);
      digitalWrite(ledAmarelo, HIGH);
      tempo_amarelo = millis();
      bipesDadosAmarelo = false;
    }
  }

  if (digitalRead(ledAmarelo) == HIGH && !bipesDadosAmarelo) {
    apitarBuzzer(2, 1600, 200, 200);
    bipesDadosAmarelo = true;
  }

  if (digitalRead(ledAmarelo) == HIGH && millis() - tempo_amarelo >= 1000) {
    digitalWrite(ledAmarelo, LOW);
    digitalWrite(ledVermelho, HIGH);
    tempo_vermelho = millis();
    em_espera = true;
    bipesDadosVermelho = false;
  }

  if (em_espera && digitalRead(ledVermelho) == HIGH && millis() - tempo_vermelho >= 4000) {
    digitalWrite(ledVermelho, LOW);
    digitalWrite(ledVerde, HIGH);
    tempo_inicio = millis();
    em_espera = false;
    bipesDadosVerde = false;
  }

  if (digitalRead(ledVermelho) == HIGH && !bipesDadosVermelho) {
    tone(buzzer, 900);
    delay(300);
    noTone(buzzer);
    bipesDadosVermelho = true;
  }
}
