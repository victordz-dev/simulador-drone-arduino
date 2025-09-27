//Incluindo as bibliotecas
  #include <Adafruit_SSD1306.h> //inclui a biblioteca do display
  #include <Adafruit_GFX.h> //inclui a biblioteca do display
  #include <Stepper.h> //inclui a biblioteca do motor
  #include <DHT.h> //inclui a biblioteca do sensor de temperatura
//----------------------------------------------------------------------//

//setando parâmetros do display
  #define width 128 //define a largura do display
  #define height 64 // define a altura do display
  #define reset -1 // define o pino de reset, que nesse caso é a mesma do arduino
//----------------------------------------------------------------------//

//definindo os pinos dos leds
  #define ledG 13
  #define ledR 12
  #define ledW 7
//----------------------------------------------------------------------//

//Definindo os pinos do joyStick e potenciômetro
  #define stickX A1 //eixo horizontal
  #define stickY A0 //eixo vertical
  #define stickBtn 2
  #define pot A2
//----------------------------------------------------------------------//

//Definindo os pinos dos botões de controle
  #define up 4
  #define down 5
  #define exit 3
  #define enter 6
//----------------------------------------------------------------------//

//Definindo os objetos
  #define passosMotor 200
  Adafruit_SSD1306 OLED = Adafruit_SSD1306(width, height, &Wire, reset);
  Stepper motor(passosMotor, 8, 9, 10, 11);
  DHT sensor(A3, DHT22);
//----------------------------------------------------------------------//

//variaveis utilizadas nas funções
  int selecionado = 0;
  int preenchido = -1;
  int btnClick = 0;
  int btnLiberado = 0;
  int passoAtual = 0;
//----------------------------------------------------------------------//

//função para ligar a lanterna
  void lanterna(){
    if(digitalRead(stickBtn) == 0){ //bloco que verifica se o botão foi clicado
      btnClick = 1;
      btnLiberado = 0;
    }else{
      btnLiberado = 1;
    }

    if((btnClick == 1) && (btnLiberado == 1)){ //bloco que verifica se o led esta ligado ou desligado
      btnClick = 0;
      btnLiberado = 0;
    if(digitalRead(ledW) == HIGH){
      digitalWrite(ledW, LOW);
    }else{
      digitalWrite(ledW, HIGH);
    }
    }
  }
//----------------------------------------------------------------------//

//função que mostra condições de alerta
  void alerta(){
    digitalWrite(ledR, HIGH);
    digitalWrite(ledG, LOW);
  }
//----------------------------------------------------------------------//

//função que mostra condições normais
  void normal(){
    digitalWrite(ledR, LOW);
    digitalWrite(ledG, HIGH);
  }
//----------------------------------------------------------------------//

//função para apagar os led depois de sair do menu
  void desligaLeds(){
    digitalWrite(ledR, LOW);
    digitalWrite(ledG, LOW);
  }
//----------------------------------------------------------------------//

//função que chama o sensor de temperatura
  void temperatura(){
    int temp = sensor.readTemperature();

    OLED.clearDisplay();
    OLED.setTextSize(1);
    OLED.setCursor(25,0);
    OLED.print("Temperatura");
    OLED.setTextSize(2);
    OLED.setCursor(30,25);
    OLED.print(temp);
    OLED.setCursor(80,25);
    OLED.print("c");
    OLED.drawCircle(70,25,2,WHITE);
    OLED.display();

    if (temp > 40 || temp < -5){
      alerta();
    }else{
      normal();
    }
  }
//----------------------------------------------------------------------//

//função que simula o sensor de salinidade
  void salinidade(){
    int potValue = analogRead(pot);

    float voltagem = potValue * (5.0/1023.0);  //converte a leitura do potenciômetro para voltagem
    float minSal = 0.0;
    float maxSal = 70.0;
    float salinidadeAgua = minSal + (voltagem * ((maxSal - minSal) / 5.0)); //converte a voltagem para salinidade nos padrões de agua do mar

    OLED.clearDisplay(); //configura o display
    OLED.setTextSize(1);
    OLED.setCursor(35,0);
    OLED.print("Salinidade");
    OLED.setTextSize(2);
    OLED.setCursor(30,25);
    OLED.print((int)salinidadeAgua);
    OLED.setCursor(60,25);
    OLED.print("PPT");
    OLED.display();

    if (salinidadeAgua > 45 || salinidadeAgua < 10){ //bloco que verifica se a salinidade esta fora do padrão
      alerta();
    }else{
      normal();
    }
  }
//----------------------------------------------------------------------//

// função que simula o sensor de ph
  void ph(){
    int potValue = analogRead(pot);

    float voltagem = potValue * (5.0/1023.0); //converte a leitura do potenciômetro para voltagem
    float minPh = 0.0;
    float maxPh = 14.0;
    float ph = minPh + (voltagem * ((maxPh - minPh) / 5.0)); //converte a voltagem para ph nos padrões de agua do mar

    OLED.clearDisplay();  //configura o display
    OLED.setTextColor(SSD1306_WHITE);
    OLED.setTextSize(1);
    OLED.setCursor(50,0);
    OLED.print("PH");
    OLED.setTextSize(2);
    OLED.setCursor(30,25);
    OLED.print(ph);
    OLED.display();

    if (ph > 10.0 || ph < 7.5){ //bloco que verifica se o ph esta fora do padrão
      alerta();
    }else{
      normal();
    }
  }
//----------------------------------------------------------------------//

// função que chama o sensor de umidade
  void umidade(){
    int umid = sensor.readHumidity();

    OLED.clearDisplay();
    OLED.setTextColor(SSD1306_WHITE);
    OLED.setTextSize(1);
    OLED.setCursor(40,0);
    OLED.print("Umidade");
    OLED.setTextSize(2);
    OLED.setCursor(40,25);
    OLED.print(umid);
    OLED.setCursor(80,25);
    OLED.print("%");
    OLED.display();

      if (umid >= 90){
        normal();
      }else{
        alerta();
      }
  }
//função que permite navegar para observar qual dado deseja
  void menu(){
    if (digitalRead(down) == LOW){ //bloco que verifica se o botão foi clicado
      selecionado = selecionado + 1;  //bloco que permite navegar entre as opções
      delay(250); //bloco que impede de clicar varias vezes no botão
    }

    if (digitalRead(up) == LOW){ 
      selecionado = selecionado - 1; 
      delay(250);
    }

    if (digitalRead(enter) == LOW){ 
      preenchido = selecionado; //preenchido recebe o valor de selecionado e entra na opção desejada
      delay(250);
    }
    
    if (digitalRead(exit) == LOW){
      preenchido = -1; //preenchido volta a ser -1 e sai da opção desejada
      desligaLeds();
      delay(250);
    }

    const char *opcoes[4] = { //opções do menu
      "Temperatura",
      "Salinidade",
      "PH",
      "Umidade"
    };

    if (preenchido == -1){ //configura o display
      OLED.clearDisplay();
      OLED.setTextSize(1);
      OLED.setTextColor(SSD1306_WHITE);
      OLED.setCursor(50, 0);
      OLED.println(F("MENU"));
      OLED.println("");

      for (int i = 0; i < 4; i++){ //bloco que mostra as seleções do menu
        if (selecionado < 0){ //bloco que impede de selecionar uma opção que não existe
          selecionado = 0;
        } 
        
        if (selecionado > 3){ //bloco que impede de selecionar uma opção que não existe
          selecionado = 3;
        }

        if (i == selecionado){ //bloco que mostra a seleção atual
          OLED.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          OLED.print(">");
          OLED.println(opcoes[i]);
        }else{
          OLED.setTextColor(SSD1306_WHITE); 
          OLED.println(opcoes[i]); 
        }
      }
    }

    if (preenchido == 0){ //bloco que chama a função de temperatura
      temperatura();
    }

    if (preenchido == 1){ //bloco que chama a função de salinidade
      salinidade();
    }

    if (preenchido == 2){ //bloco que chama a função de ph
      ph();
    }

    if (preenchido == 3){ //bloco que chama a função de ph
      umidade();
    }

    OLED.display(); 
  }
//----------------------------------------------------------------------//

//função controle que permite guiar o drone para onde deseja
  void controle(){
    int x = analogRead(stickX); 
    int y = analogRead(stickY);

    y = 1023 - y; //inverte o eixo y

    float angulo = atan2(x - 512, y - 512) * 180 / PI; //converte a posição do joyStick para angulo

    if (angulo < 0) { //bloco que impede o angulo de ser negativo
      angulo += 360;
    }

    int passoAlvo = map(angulo, 0, 360, 0, passosMotor); //converte o angulo para passos
    int difPassos = passoAlvo - passoAtual; //calcula a diferença de passos

    motor.step(difPassos);  //faz o motor de passos girar
    passoAtual = passoAlvo; //atualiza o passo atual
  }
//----------------------------------------------------------------------//

void setup(){
  //definindo os tipos de pino
    pinMode(ledR, OUTPUT); 
    pinMode(ledG, OUTPUT);
    pinMode(ledW, OUTPUT);
    pinMode(stickX, INPUT);
    pinMode(stickY, INPUT);
    pinMode(stickBtn, INPUT_PULLUP);
    pinMode(up, INPUT_PULLUP);
    pinMode(down, INPUT_PULLUP);
    pinMode(exit, INPUT_PULLUP);
    pinMode(enter, INPUT_PULLUP);
    pinMode(pot, INPUT);
  //inicia o display
    OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C); //endereço i2c
    OLED.display(); //video de entrada
    delay(1000);
    OLED.clearDisplay();//limpa o display
  motor.setSpeed(100); //seta a velocidade do motor de passos
  sensor.begin(); //inicia o sensor de temperatura e umidade
}

void loop(){
  controle(); //chama a função para controlar o drone
  menu(); //chama a função para cotrolar o menu
  lanterna(); //chama a função que controla a lanterna do drone
}