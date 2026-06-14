#include <LiquidCrystal.h>

// LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// LEDs
#define LED_VERDE 10
#define LED_AMARELO 9 
#define LED_VERMELHO 8
#define LED_LARANJA 7

// Buzzer
#define BUZZER 13

// Limite de livros
#define MAX_LIVROS 20

// Livros fixos
#define LIVROS_FIXOS 5

// Estrutura
struct Livro {
  String nome;
  bool reservado;
};

Livro livros[MAX_LIVROS];
int totalLivros = 5;

// ======================
// Sons
// ======================

void somSucesso() {
  tone(BUZZER, 800, 150);
  delay(200);
  tone(BUZZER, 1000, 150);
  delay(200);
}

void somErro() {
  tone(BUZZER, 250, 300);
  delay(350);
}

// ======================
// Utilitários
// ======================

bool esperarEntrada(unsigned long tempo = 15000) {
  unsigned long inicio = millis();

  while (!Serial.available()) {
    if (millis() - inicio >= tempo) {
      Serial.println("Tempo esgotado!");
      return false;
    }
    delay(10);
  }
  return true;
}

void limparBuffer() {
  while (Serial.available()) {
    Serial.read();
  }
}

void mostrarLCD(String linha1, String linha2 = "") {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(linha1);

  lcd.setCursor(0, 1);

  if (linha2.length() > 16)
    linha2 = linha2.substring(0, 16);

  lcd.print(linha2);
}

// ======================
// Menu
// ======================

void menu() {
  Serial.println();
  Serial.println("===== BIBLIOTECA =====");
  Serial.println("1 - Cadastrar");
  Serial.println("2 - Listar");
  Serial.println("3 - Reservar");
  Serial.println("4 - Devolver");
  Serial.println("5 - Remover");
  Serial.println("6 - Estatisticas");
  Serial.println("======================");
  Serial.println("Escolha uma opcao:");
}

// ======================
// Cadastro
// ======================

void cadastrarLivro() {

  if (totalLivros >= MAX_LIVROS) {

    Serial.println("Limite atingido!");

    digitalWrite(LED_VERMELHO, HIGH);
    mostrarLCD("Limite cheio");
    somErro();
    digitalWrite(LED_VERMELHO, LOW);

    return;
  }

  Serial.println("Digite o nome do livro:");

  if (!esperarEntrada()) {
    menu();
    return;
  }

  String nome = Serial.readStringUntil('\n');
  nome.trim();

  limparBuffer();

  if (nome.length() == 0) {

    Serial.println("Nome invalido!");

    digitalWrite(LED_VERMELHO, HIGH);
    mostrarLCD("Nome invalido");
    somErro();
    digitalWrite(LED_VERMELHO, LOW);

    return;
  }

  for (int i = 0; i < totalLivros; i++) {
    if (nome.equalsIgnoreCase(livros[i].nome)) {

      Serial.println("Livro ja existe!");

      digitalWrite(LED_VERMELHO, HIGH);
      mostrarLCD("Ja existe");
      somErro();
      digitalWrite(LED_VERMELHO, LOW);

      return;
    }
  }

  livros[totalLivros].nome = nome;
  livros[totalLivros].reservado = false;

  totalLivros++;

  Serial.println("Livro cadastrado!");

  digitalWrite(LED_VERDE, HIGH);
  mostrarLCD("Cadastrado!", nome);
  somSucesso();
  digitalWrite(LED_VERDE, LOW);

  delay(1500);
}

// ======================
// Listagem (Atualizada)
// ======================

void listarLivros() {
  digitalWrite(LED_AMARELO, HIGH);

  Serial.println();
  Serial.println("===== LIVROS =====");

  if (totalLivros == 0) {
    Serial.println("Nenhum livro.");
    mostrarLCD("Sem livros");
    delay(2000);
    digitalWrite(LED_AMARELO, LOW);
    return;
  }

  for (int i = 0; i < totalLivros; i++) {
    String statusLivro = "";

    Serial.print("L");
    Serial.print(i + 1);
    Serial.print(" - ");
    Serial.print(livros[i].nome);
    Serial.print(" - ");

    if (livros[i].reservado) {
      Serial.println("Reservado");
      statusLivro = "Reservado";
    } else {
      Serial.println("Disponivel");
      statusLivro = "Disponivel";
    }

    mostrarLCD(livros[i].nome, statusLivro);
    
    delay(2000); 
  }

  mostrarLCD("Total livros", String(totalLivros));
  delay(2000);
  
  digitalWrite(LED_AMARELO, LOW);
}

// ======================
// Reserva
// ======================

void reservarLivro() {

  Serial.println("Digite o codigo:");
  Serial.println("Exemplo: 1");

  if (!esperarEntrada()) {
    menu();
    return;
  }

  int codigo = Serial.parseInt();
  limparBuffer();
  codigo--;

  if (codigo < 0 || codigo >= totalLivros) {

    Serial.println("Livro inexistente!");

    digitalWrite(LED_VERMELHO, HIGH);
    mostrarLCD("Livro nao existe");
    somErro();
    digitalWrite(LED_VERMELHO, LOW);

    return;
  }

  if (livros[codigo].reservado) {

    Serial.println("Livro indisponivel!");

    digitalWrite(LED_VERMELHO, HIGH);
    mostrarLCD("Indisponivel");
    somErro();
    digitalWrite(LED_VERMELHO, LOW);

    return;
  }

  livros[codigo].reservado = true;

  Serial.print("Reservado: ");
  Serial.println(livros[codigo].nome);

  digitalWrite(LED_VERDE, HIGH);
  mostrarLCD("Reservado!", livros[codigo].nome);
  somSucesso();
  digitalWrite(LED_VERDE, LOW);

  delay(1500);
}

// ======================
// Devolucao
// ======================

void devolverLivro() {

  Serial.println("Digite o codigo:");
  Serial.println("Exemplo: 1");

  if (!esperarEntrada()) {
    menu();
    return;
  }

  int codigo = Serial.parseInt();
  limparBuffer();
  codigo--;

  if (codigo < 0 || codigo >= totalLivros) {

    Serial.println("Livro inexistente!");

    digitalWrite(LED_VERMELHO, HIGH);
    mostrarLCD("Livro nao existe");
    somErro();
    digitalWrite(LED_VERMELHO, LOW);

    return;
  }

  if (!livros[codigo].reservado) {

    Serial.println("Livro nao reservado!");

    digitalWrite(LED_VERMELHO, HIGH);
    mostrarLCD("Livro nao", "reservado");
    somErro();
    digitalWrite(LED_VERMELHO, LOW);

    return;
  }

  livros[codigo].reservado = false;

  Serial.print("Devolvido: ");
  Serial.println(livros[codigo].nome);

  digitalWrite(LED_VERDE, HIGH);
  mostrarLCD("Devolvido!", livros[codigo].nome);
  somSucesso();
  digitalWrite(LED_VERDE, LOW);

  delay(1500);
}

// ======================
// Remover
// ======================

void removerLivro() {

  if (totalLivros <= LIVROS_FIXOS) {

    Serial.println("Nao ha livros removiveis!");

    digitalWrite(LED_LARANJA, HIGH);
    mostrarLCD("Apenas fixos");
    somErro();
    digitalWrite(LED_LARANJA, LOW);

    return;
  }

  Serial.println("Digite o codigo do livro:");

  if (!esperarEntrada()) {
    menu();
    return;
  }

  int codigo = Serial.parseInt();
  limparBuffer();
  codigo--;

  if (codigo < 0 || codigo >= totalLivros) {

    Serial.println("Livro inexistente!");

    digitalWrite(LED_VERMELHO, HIGH);
    mostrarLCD("Livro nao existe");
    somErro();
    digitalWrite(LED_VERMELHO, LOW);

    return;
  }

  if (codigo < LIVROS_FIXOS) {

    Serial.println("Nao pode remover livros fixos!");

    digitalWrite(LED_LARANJA, HIGH);
    mostrarLCD("Livro fixo");
    somErro();
    digitalWrite(LED_LARANJA, LOW);

    return;
  }

  Serial.print("Removido: ");
  Serial.println(livros[codigo].nome);

  for (int i = codigo; i < totalLivros - 1; i++) {
    livros[i] = livros[i + 1];
  }

  totalLivros--;

  digitalWrite(LED_LARANJA, HIGH);
  mostrarLCD("Removido!");
  somSucesso();
  digitalWrite(LED_LARANJA, LOW);

  delay(1500);
}

// ======================
// Estatisticas
// ======================

void mostrarEstatisticas() {

  if (totalLivros == 0) {
    Serial.println("Nenhum livro no sistema!");
    mostrarLCD("Sem dados");
    return;
  }

  int reservados = 0;

  for (int i = 0; i < totalLivros; i++) {
    if (livros[i].reservado) {
      reservados++;
    }
  }

  int disponiveis = totalLivros - reservados;

  float pReservados = (reservados * 100.0) / totalLivros;
  float pDisponiveis = (disponiveis * 100.0) / totalLivros;

  Serial.println();
  Serial.println("===== ESTATISTICAS =====");

  Serial.print("Total: ");
  Serial.println(totalLivros);

  Serial.print("Reservados: ");
  Serial.print(reservados);
  Serial.print(" (");
  Serial.print(pReservados, 1);
  Serial.println("%)");

  Serial.print("Disponiveis: ");
  Serial.print(disponiveis);
  Serial.print(" (");
  Serial.print(pDisponiveis, 1);
  Serial.println("%)");

  Serial.println("========================");

  mostrarLCD(
    "Reserv: " + String(reservados),
    "Dispon: " + String(disponiveis)
  );
  delay(4000);

  mostrarLCD(
    "Total de livros:",
    String(totalLivros)
  );
  delay(4000);
}

// ======================
// Setup
// ======================

void setup() {

  Serial.begin(9600);
  Serial.setTimeout(100);

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LED_LARANJA, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  lcd.begin(16, 2);

  livros[0].nome = "Dom Casmurro";
  livros[1].nome = "O Hobbit";
  livros[2].nome = "Harry Potter";
  livros[3].nome = "Percy Jackson";
  livros[4].nome = "1984";

  for (int i = 0; i < totalLivros; i++) {
    livros[i].reservado = false;
  }

  mostrarLCD("Bem vindo a", "biblioteca");

  digitalWrite(LED_VERDE, HIGH);
  delay(300);
  digitalWrite(LED_VERDE, LOW);

  Serial.println("Sistema iniciado!");

  menu();
}

// ======================
// Loop
// ======================

void loop() {

  if (Serial.available()) {

    int opcao = Serial.parseInt();
    limparBuffer();

    switch (opcao) {

      case 1:
        cadastrarLivro();
        break;

      case 2:
        listarLivros();
        break;

      case 3:
        reservarLivro();
        break;

      case 4:
        devolverLivro();
        break;

      case 5:
        removerLivro();
        break;

      case 6:
        mostrarEstatisticas();
        break;

      default:
        Serial.println("Opcao invalida!");
        break;
    }

    menu();
  }
}