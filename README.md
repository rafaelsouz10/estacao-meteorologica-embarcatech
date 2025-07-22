# Estação Meteorológica Interativa EmbarcaTech

---

## 🎯 Descrição do Projeto
Projeto desenvolvido no contexto do EmbarcaTech, utilizando a placa BitDogLab com a Raspberry Pi Pico W para criar uma estação meteorológica embarcada. O sistema monitora em tempo real a **temperatura**, **umidade** e **pressão atmosférica** com sensores I2C, exibindo os dados localmente em um **display OLED** e remotamente em uma **interface web responsiva**, com **gráficos dinâmicos**, **ajustes de calibração** e **alertas visuais e sonoros** para valores fora dos limites definidos pelo usuário.

---

## ⚙️ Requisitos

- **Microcontrolador**: Raspberry Pi Pico W
- **Editor de Código**: Visual Studio Code (VS Code)
- **SDK do Raspberry Pi Pico** devidamente configurado
- **Ferramentas de build**: CMake e Ninja

---

## Instruções de Uso

### 1. Clone o Repositório
```bash
git clone https://github.com/rafaelsouz10/estacao-meteorologica-embarcatech.git
cd estacao-meteorologica-embarcatech
code .
```

---

### 2. Instale as Dependências

### 2.1 Certifique-se de que o SDK do Raspberry Pi Pico está configurado corretamente no VS Code. As extensões recomendadas são:

- **C/C++** (Microsoft).
- **CMake Tools**.
- **Raspberry Pi Pico**.


### 3. Configure o VS Code

Abra o projeto no Visual Studio Code e siga as etapas abaixo:

1. Certifique-se de que as extensões mencionadas anteriormente estão instaladas.
2. No terminal do VS Code, compile o código clicando em "Compile Project" na interface da extensão do Raspberry Pi Pico.
3. O processo gerará o arquivo `.uf2` necessário para a execução no hardware real.

---

### 4. Teste no Hardware Real

#### Utilizando a Placa de Desenvolvimento BitDogLab com Raspberry Pi Pico W:

1. Conecte a placa ao computador no modo BOTSEEL:
   - Pressione o botão **BOOTSEL** (localizado na parte de trás da placa de desenvolvimento) e, em seguida, o botão **RESET** (localizado na frente da placa).
   - Após alguns segundos, solte o botão **RESET** e, logo depois, solte o botão **BOOTSEL**.
   - A placa entrará no modo de programação.

2. Compile o projeto no VS Code utilizando a extensão do [Raspberry Pi Pico W](https://marketplace.visualstudio.com/items?itemName=raspberry-pi.raspberry-pi-pico):
   - Clique em **Compile Project**.

3. Execute o projeto clicando em **Run Project USB**, localizado abaixo do botão **Compile Project**.

---

### 🔌 5. Conexões e Esquemático
Abaixo está o mapeamento de conexões entre os componentes e a Raspberry Pi Pico W:

| **Componentes**        | **Pino Conectado (GPIO)** |
|------------------------|---------------------------|
| Display SSD1306 (SDA1) | GPIO 14                   |
| Display SSD1306 (SCL1) | GPIO 15                   |
| LED_VERDE              | GPIO 16                   |
| LED VERMELHO           | GPIO 17                   |
| Buzzer                 | GPIO 21                   |
| Botão A                | GPIO 5                    |
| Botão B                | GPIO 6                    |
| MATRIZ DE LEDS RGB     | GPIO 7                    |
| Sensor AHT20 (SDA0)    | GPIO 0                    |
| Sensor AHT20 (SCL0)    | GPIO 1                    |
| Sensor BMP280 (SDA0)   | GPIO 0                    |
| Sensor BMP280 (SCL0)   | GPIO 1                    |

#### 🛠️ Demais Hardwares Utilizado
- **Microcontrolador Raspberry Pi Pico W**
- **Wi-Fi (CYW43439)**

---

### 📌 6. Funcionamento do Sistema

#### 🌐 Interface Web
A **interface web** é servida localmente pela placa via Wi-Fi e oferece:

- 📊 Gráficos em tempo real para **temperatura**, **umidade** e **pressão** (Chart.js).

- 🧪 Formulário de calibração com **offsets ajustáveis**.

- 📏 **Limites configuráveis** para disparo de alerta.

- 🔔 Estado do sistema (**alerta visual/sonoro, sensores, buzzer**).

- 🌐 **Atualização automática** com fetch() e JSON via AJAX.

#### 🧠 Lógica do Sistema
🟢 Leitura de Sensores

- **AHT20**: Temperatura e umidade.
- **BMP280**: Pressão atmosférica e temperatura auxiliar.

🔔 Alertas
- LED RGB: Alerta visual (mudança de cor).
- Buzzer: Sinal sonoro para leituras críticas.
- Matriz 5x5: Animação de “gota de chuva” indicando alerta.

🖥️ Webserver (TCP via lwIP)
- Servidor TCP embutido envia HTML e responde com JSON.
- Rota /estado retorna JSON com leituras e histórico.
- Rota /ajuste recebe os dados de calibração.

---

### 8. Vídeos Demonstrativo

**Click [AQUI](COLAR LINK AQUI) para acessar o link do Vídeo Ensaio**
