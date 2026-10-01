#include <Arduino.h>

/*
 * ============================================================
 *  SENAI – Internet das Coisas
 *  Técnico em Desenvolvimento de Sistemas
 *
 *  Projeto: Servidor Web no ESP32 (Modo Access Point)
 *           - Leitura de Sensores DHT11/DHT22
 *           - Controle de LED via Navegador
 *
 *  Integrantes: Camila, Maria Zaine, Sam
 *
 *  Bibliotecas necessárias (instalar no Library Manager):
 *    - DHT sensor library (Adafruit)
 *    - Adafruit Unified Sensor
 *
 * ============================================================
 */


// ── Escolha a placa: comente/descomente conforme necessário ──
#define PLACA_ESP32   // ESP32 (Wemos D1 R32)


// ── Includes condicionais ─────────────────────────────────────
#ifdef PLACA_ESP32
  #include <WiFi.h>
  #include <WebServer.h>
  WebServer server(80);
#endif




#include <DHT.h>


// ── Configurações da rede AP ──────────────────────────────────
const char* ssid     = "Grupo2_IoT";   // Nome da rede Wi-Fi
const char* password = "divas_e_simeon";   // Senha (mín. 8 caracteres)


// ── Pinos ─────────────────────────────────────────────────────
#define DHTPIN   4    // GPIO onde o DHT está conectado
#define DHTTYPE  DHT22  // DHT11 ou DHT22
#define LED_PIN  13   // GPIO do LED


// ── Objetos ──────────────────────────────────────────────────
DHT dht(DHTPIN, DHTTYPE);
bool ledState = false;
//  ESTILOS CSS COMUNS (embarcados como String)
const char* CSS = R"rawCSS(
  @import url('https://fonts.googleapis.com/css2?family=Orbitron:wght@700&family=Share+Tech+Mono&display=swap');
  :root {
    --bg: #0a0e1a;
    --card: #111827;
    --border: #1e3a5f;
    --accent: #00d4ff;
    --accent2: #ff6b35;
    --green: #00ff88;
    --red: #ff3366;
    --text: #c9d6e3;
    --heading: #ffffff;
  }
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body {
    background: var(--bg);
    color: var(--text);
    font-family: 'Share Tech Mono', monospace;
    min-height: 100vh;
    display: flex;
    flex-direction: column;
    align-items: center;
  }
  header {
    width: 100%;
    background: linear-gradient(135deg, #0d1b2a 0%, #1a2f4a 100%);
    border-bottom: 2px solid var(--accent);
    padding: 18px 24px;
    text-align: center;
    box-shadow: 0 0 30px rgba(0,212,255,0.15);
  }
  header h1 {
    font-family: 'Orbitron', sans-serif;
    font-size: 1.5rem;
    color: var(--accent);
    letter-spacing: 3px;
    text-shadow: 0 0 15px rgba(0,212,255,0.5);
  }
  header p {
    font-size: 0.75rem;
    color: #7a9bb5;
    margin-top: 4px;
    letter-spacing: 2px;
    text-transform: uppercase;
  }
  main {
    width: 100%;
    max-width: 700px;
    padding: 30px 16px;
    flex: 1;
  }
  .card {
    background: var(--card);
    border: 1px solid var(--border);
    border-radius: 8px;
    padding: 24px;
    margin-bottom: 20px;
    box-shadow: 0 0 20px rgba(0,0,0,0.4);
  }
  .card h2 {
    font-family: 'Orbitron', sans-serif;
    font-size: 1rem;
    color: var(--accent);
    margin-bottom: 16px;
    letter-spacing: 2px;
    border-bottom: 1px solid var(--border);
    padding-bottom: 10px;
  }
  .card p { line-height: 1.8; font-size: 0.9rem; }
  .nav {
    display: flex;
    gap: 12px;
    flex-wrap: wrap;
    margin-top: 20px;
  }
  a.btn {
    display: inline-block;
    padding: 10px 22px;
    border-radius: 4px;
    text-decoration: none;
    font-family: 'Orbitron', sans-serif;
    font-size: 0.75rem;
    letter-spacing: 1px;
    transition: all 0.2s;
  }
  a.btn-primary {
    background: transparent;
    border: 2px solid var(--accent);
    color: var(--accent);
    box-shadow: 0 0 10px rgba(0,212,255,0.2);
  }
  a.btn-primary:hover {
    background: var(--accent);
    color: var(--bg);
    box-shadow: 0 0 20px rgba(0,212,255,0.5);
  }
  a.btn-secondary {
    background: transparent;
    border: 2px solid var(--accent2);
    color: var(--accent2);
  }
  a.btn-secondary:hover {
    background: var(--accent2);
    color: var(--bg);
  }
  a.btn-back {
    border: 2px solid #3a5070;
    color: #7a9bb5;
  }
  a.btn-back:hover { border-color: var(--accent); color: var(--accent); }
  .sensor-grid {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 16px;
    margin-top: 8px;
  }
  .sensor-box {
    background: #0d1b2a;
    border: 1px solid var(--border);
    border-radius: 6px;
    padding: 20px;
    text-align: center;
  }
  .sensor-box .label {
    font-size: 0.7rem;
    letter-spacing: 2px;
    color: #7a9bb5;
    text-transform: uppercase;
    margin-bottom: 10px;
  }
  .sensor-box .value {
    font-family: 'Orbitron', sans-serif;
    font-size: 2rem;
    font-weight: 700;
  }
  .sensor-box .unit {
    font-size: 0.9rem;
    color: #7a9bb5;
    margin-left: 4px;
  }
  .temp-val { color: var(--accent2); text-shadow: 0 0 10px rgba(255,107,53,0.4); }
  .hum-val  { color: var(--accent);  text-shadow: 0 0 10px rgba(0,212,255,0.4); }
  .led-status {
    font-family: 'Orbitron', sans-serif;
    font-size: 1.4rem;
    letter-spacing: 2px;
    padding: 16px;
    border-radius: 6px;
    text-align: center;
    margin-bottom: 20px;
  }
  .led-on  { color: var(--green); border: 2px solid var(--green); background: rgba(0,255,136,0.06); text-shadow: 0 0 15px rgba(0,255,136,0.5); }
  .led-off { color: var(--red);   border: 2px solid var(--red);   background: rgba(255,51,102,0.06); text-shadow: 0 0 15px rgba(255,51,102,0.4); }
  .btn-led-on  { border-color: var(--green) !important; color: var(--green) !important; }
  .btn-led-on:hover  { background: var(--green) !important; color: var(--bg) !important; box-shadow: 0 0 20px rgba(0,255,136,0.4) !important; }
  .btn-led-off { border-color: var(--red) !important; color: var(--red) !important; }
  .btn-led-off:hover { background: var(--red) !important; color: var(--bg) !important; }
  .error { color: var(--red); font-size: 0.9rem; padding: 12px; border: 1px solid var(--red); border-radius: 4px; background: rgba(255,51,102,0.06); }
  .team-list { list-style: none; margin-top: 8px; }
  .team-list li { padding: 6px 0; border-bottom: 1px solid #1a2a3a; font-size: 0.9rem; }
  .team-list li:last-child { border: none; }
  .team-list li::before { content: "▸ "; color: var(--accent); }
  footer {
    width: 100%;
    text-align: center;
    padding: 14px;
    font-size: 0.7rem;
    color: #3a5070;
    border-top: 1px solid #1a2a3a;
    letter-spacing: 1px;
  }
)rawCSS";
//  PÁGINA PRINCIPAL  ( / )
void handleRoot() {
  String html = "<!DOCTYPE html><html lang='pt-BR'><head>";
  html += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>";
  html += "<title>SENAI IoT</title><style>" + String(CSS) + "</style></head><body>";


  // Header
  html += "<header><h1>SENAI – INTERNET DAS COISAS</h1>";
  html += "<p>Técnico em Desenvolvimento de Sistemas</p></header>";


  html += "<main>";


  // Card: Sobre o Projeto
  html += "<div class='card'><h2>◈ SOBRE O PROJETO</h2>";
  html += "<p>Servidor Web embarcado no ESP32 para leitura de sensores DHT e controle de atuadores (LED) via Wi-Fi, sem necessidade de roteador externo.</p></div>";


  // Card: Integrantes
  html += "<div class='card'><h2>◈ EQUIPE</h2>";
  html += "<ul class='team-list'>";
  html += "<li>Camila Paranhos</li>";
  html += "<li>Juliana Viana</li>";  
  html += "<li>Maria Zaine</li>";
  html += "<li>Sam Rodrigues</li>";
  html += "<li>Simeon Gomes</li>";
  html += "</ul></div>";


  // Card: Navegação
  html += "<div class='card'><h2>◈ NAVEGAÇÃO</h2>";
  html += "<div class='nav'>";
  html += "<a class='btn btn-primary' href='/sensores'>📡 Leitura dos Sensores</a>";
  html += "<a class='btn btn-secondary' href='/atuadores'>💡 Controle de Atuadores</a>";
  html += "</div></div>";


  html += "</main>";
  html += "<footer>SENAI – IoT PROJECT &nbsp;|&nbsp; ESP32 ACCESS POINT</footer>";
  html += "</body></html>";


  server.send(200, "text/html", html);
}
//  PÁGINA DE SENSORES  ( /sensores )
void handleSensores() {
  float temp = dht.readTemperature();
  float umid = dht.readHumidity();


  String html = "<!DOCTYPE html><html lang='pt-BR'><head>";
  html += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>";
  html += "<meta http-equiv='refresh' content='5'>";   // Atualiza a cada 5s
  html += "<title>Sensores – SENAI IoT</title><style>" + String(CSS) + "</style></head><body>";


  html += "<header><h1>SENAI – INTERNET DAS COISAS</h1>";
  html += "<p>Leitura dos Sensores</p></header>";


  html += "<main>";
  html += "<div class='card'><h2>◈ LEITURA DOS SENSORES</h2>";


  if (isnan(temp) || isnan(umid)) {
    html += "<p class='error'>⚠ Sensor não respondeu. Verifique a conexão do DHT.</p>";
  } else {
    html += "<div class='sensor-grid'>";


    html += "<div class='sensor-box'>";
    html += "<div class='label'>Temperatura</div>";
    html += "<div class='value temp-val'>" + String(temp, 1) + "<span class='unit'>°C</span></div>";
    html += "</div>";


    html += "<div class='sensor-box'>";
    html += "<div class='label'>Umidade</div>";
    html += "<div class='value hum-val'>" + String(umid, 1) + "<span class='unit'>%</span></div>";
    html += "</div>";


    html += "</div>";  // sensor-grid
    html += "<p style='margin-top:14px;font-size:0.75rem;color:#3a5070;'>↻ Atualização automática a cada 5 segundos</p>";
  }


  html += "</div>";  // card


  html += "<div class='nav'>";
  html += "<a class='btn btn-back' href='/'>← Voltar</a>";
  html += "<a class='btn btn-secondary' href='/atuadores'>💡 Atuadores</a>";
  html += "</div>";


  html += "</main>";
  html += "<footer>SENAI – IoT PROJECT &nbsp;|&nbsp; ESP32 ACCESS POINT</footer>";
  html += "</body></html>";


  server.send(200, "text/html", html);
}
//  PÁGINA DE ATUADORES  ( /atuadores )
void handleAtuadores() {
  // Verifica parâmetro ledstatus na URL
  if (server.hasArg("ledstatus")) {
    String val = server.arg("ledstatus");
    if (val == "1") {
      ledState = true;
      digitalWrite(LED_PIN, HIGH);
    } else if (val == "0") {
      ledState = false;
      digitalWrite(LED_PIN, LOW);
    }
  }


  String html = "<!DOCTYPE html><html lang='pt-BR'><head>";
  html += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>";
  html += "<title>Atuadores – SENAI IoT</title><style>" + String(CSS) + "</style></head><body>";


  html += "<header><h1>SENAI – INTERNET DAS COISAS</h1>";
  html += "<p>Controle de Atuadores</p></header>";


  html += "<main>";
  html += "<div class='card'><h2>◈ CONTROLE DO LED (GPIO 13)</h2>";


  if (ledState) {
    html += "<div class='led-status led-on'>● LED: ACESO</div>";
  } else {
    html += "<div class='led-status led-off'>○ LED: APAGADO</div>";
  }


  // Botões de controle
  html += "<div class='nav'>";
  html += "<a class='btn btn-led-on'  href='/atuadores?ledstatus=1'>▶ Acender LED</a>";
  html += "<a class='btn btn-led-off' href='/atuadores?ledstatus=0'>■ Apagar LED</a>";
  html += "</div>";
  html += "</div>";  // card


  html += "<div class='nav'>";
  html += "<a class='btn btn-back' href='/'>← Voltar</a>";
  html += "<a class='btn btn-primary' href='/sensores'>📡 Sensores</a>";
  html += "</div>";


  html += "</main>";
  html += "<footer>SENAI – IoT PROJECT &nbsp;|&nbsp; ESP32 ACCESS POINT</footer>";
  html += "</body></html>";


  server.send(200, "text/html", html);
}
//  404  – Página não encontrada
void handleNotFound() {
  String html = "<!DOCTYPE html><html lang='pt-BR'><head>";
  html += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>";
  html += "<title>404 – SENAI IoT</title><style>" + String(CSS) + "</style></head><body>";
  html += "<header><h1>SENAI – INTERNET DAS COISAS</h1></header>";
  html += "<main><div class='card'>";
  html += "<h2>404 – NÃO ENCONTRADO</h2>";
  html += "<p class='error'>⚠ Página não existe: " + server.uri() + "</p>";
  html += "<div class='nav'><a class='btn btn-back' href='/'>← Voltar ao Início</a></div>";
  html += "</div></main></body></html>";
  server.send(404, "text/html", html);
}


//  SETUP
void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.begin(115200);
  delay(500);


  // Configura pinos
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);


  // Inicia o sensor DHT
  dht.begin();
  Serial.println("\n[DHT] Sensor iniciado.");


  // Cria rede Access Point
  Serial.print("[WiFi] Criando Access Point: ");
  Serial.println(ssid);
  WiFi.softAP(ssid, password);


  IPAddress ip = WiFi.softAPIP();
  Serial.print("[WiFi] IP do Access Point: ");
  Serial.println(ip);
  Serial.println("[WiFi] Rede pronta! Conecte o celular/notebook e acesse o IP acima.");


  // Registra rotas
  server.on("/",          handleRoot);
  server.on("/sensores",  handleSensores);
  server.on("/atuadores", handleAtuadores);
  server.onNotFound(handleNotFound);


  // Inicia o servidor
  server.begin();
  Serial.println("[HTTP] Servidor iniciado na porta 80.");
  Serial.println("[HTTP] Rotas disponíveis:");
  Serial.println("       http://" + ip.toString() + "/           → Página Principal");
  Serial.println("       http://" + ip.toString() + "/sensores   → Leitura de Sensores");
  Serial.println("       http://" + ip.toString() + "/atuadores  → Controle do LED");
}


// ─────────────────────────────────────────────────────────────
//  LOOP
// ─────────────────────────────────────────────────────────────
void loop() {
  server.handleClient();   // Processa requisições HTTP
}


