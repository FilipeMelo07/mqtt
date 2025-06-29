# Projeto de Controle de LED via MQTT com ESP32 e Broker Local

Este projeto implementa um cliente MQTT em um microcontrolador ESP32 utilizando o framework ESP-IDF. O dispositivo se conecta a um broker MQTT local (Mosquitto) para controlar o estado de seu LED embutido, respondendo a mensagens publicadas em um tópico específico.


---

## Descrição do Projeto

O objetivo deste projeto é demonstrar o uso do protocolo MQTT para comunicação M2M (Machine-to-Machine) em um sistema embarcado com um ecossistema totalmente local. O ESP32 atua como um cliente que:

1.  Conecta-se a uma rede Wi-Fi local pré-configurada.
2.  Conecta-se a um broker MQTT Mosquitto rodando na mesma rede local (no computador do usuário).
3.  Inscreve-se (subscribes) no tópico: `/ifpe/ads/embarcados/esp32/led`.
4.  Aguarda por mensagens nesse tópico e controla o estado do LED (GPIO 2) com base no conteúdo da mensagem recebida:
    * **Payload `1`**: Acende o LED.
    * **Payload `0`**: Apaga o LED.

## Hardware e Software Utilizados

### Hardware
* Placa ESP32.
* Cabo USB para alimentação e programação.

### Software e Ferramentas
* **Visual Studio Code** com a extensão oficial **ESP-IDF** da Espressif.
* **Git** para controle de versão.
* **Mosquitto MQTT Broker** para Windows.

---

## Como Executar o Projeto

Siga os passos abaixo para compilar e executar o projeto.

### 1. Preparação do Ambiente
* **Clone o repositório:**
    ```bash
    git clone [LINK_DO_SEU_REPOSITORIO_AQUI]
    cd [NOME_DA_PASTA_DO_PROJETO]
    ```
* **Instale o Mosquitto:** Baixe e instale o Mosquitto a partir do [site oficial](https://mosquitto.org/download/).

### 2. Inicie o Broker Mosquitto Local
É necessário ter um servidor MQTT rodando na sua rede para que o ESP32 possa se conectar.

* **Crie o arquivo de configuração:** Crie um arquivo de texto chamado `mosquitto.conf` em uma pasta de fácil acesso (ex: `C:\Users\SeuUsuario\Documentos`) com o seguinte conteúdo:
    ```ini
    listener 1883
    allow_anonymous true
    ```

* **Inicie o Broker:** Abra um terminal (CMD ou PowerShell) como Administrador e execute o seguinte comando, ajustando o caminho para o seu arquivo `.conf`:
    ```cmd
    "C:\Program Files\mosquitto\mosquitto.exe" -c "C:\Users\SeuUsuario\Documentos\mosquitto.conf" -v
    ```
    **Deixe esta janela do terminal aberta.** Ela é o seu servidor MQTT.

### 3. Configure e Grave o Firmware do ESP32

* **Descubra o IP do seu PC:** Abra um **novo** terminal e digite `ipconfig`. Anote o **"Endereço IPv4"** da sua conexão Wi-Fi (ex: `192.168.1.105`).

* **Configure o projeto no VSCode:**
    1.  Abra a pasta do projeto no VSCode.
    2.  Execute o `menuconfig` com o comando `idf.py menuconfig` ou pelo atalho do VSCode.
    3.  Navegue até `Example Connection Configuration` e configure o **SSID e a senha** da sua rede Wi-Fi.
    4.  Navegue até `Example Configuration` e configure o `(BROKER_URL) Broker URL` com o IP do seu PC. Exemplo: `mqtt://192.168.1.105`
    5.  Salve e saia.

* **Compile e Grave:** Use os comandos `flash` e `monitor` ou os botões do VSCode para gravar o firmware e iniciar o monitor serial. Você deverá ver o ESP32 se conectar ao Wi-Fi e ao seu broker Mosquitto local.

#### ESP32 se conectou ao broker

![](imgs/servidor.png)

#### monitor device

![](imgs/monitor.png)

### 4. Testando a Funcionalidade
Com tudo rodando, abra um **terceiro** terminal para enviar os comandos.

* Navegue até a pasta do Mosquitto:
    ```cmd
    cd "C:\Program Files\mosquitto"
    ```

* **Para LIGAR o LED**, publique a mensagem "1":
    ```cmd
    mosquitto_pub.exe -h 192.168.1.105 -t "/ifpe/ads/embarcados/esp32/led" -m "1"
    ```

    O comando está dizendo "Use a ferramenta de publicação do Mosquitto para enviar a mensagem '1' para o tópico '/ifpe/ads/embarcados/esp32/led', entregando-a ao servidor MQTT que está no endereço 192.168.1.105.

* **Para DESLIGAR o LED**, publique a mensagem "0":
    ```cmd
    mosquitto_pub.exe -h 192.168.1.105 -t "/ifpe/ads/embarcados/esp32/led" -m "0"
    ```
#### monitor device

![](imgs/monitor2.png)

Observe o LED físico na placa respondendo aos comandos e os logs nas janelas do Monitor Serial e do Broker Mosquitto.

---
