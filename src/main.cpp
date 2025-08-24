#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "esp_camera.h"
#include "esp_http_server.h"
#include "motor_control.h"
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "time.h" // Para NTP

// --- WiFi ---
const char* ssid = "carrito";
const char* password = "12345678";

// --- Firebase ---
#define API_KEY "AIzaSyD91HecuKw1XtfAkWFmhpou7iSDztE2RRI"
#define DATABASE_URL "https://prueba-5b785-default-rtdb.firebaseio.com/"


// --- Servidor HTTP ---
httpd_handle_t server = NULL;

esp_err_t inicio_handler(httpd_req_t *req) {
    Serial.println("Evento INICIO recibido de la maqueta");
    // Aquí llamas a tu función de Firebase
    Firebase.RTDB.setString(&fbdo, "carrera/estado", "iniciada");
    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t fin_handler(httpd_req_t *req) {
    Serial.println("Evento FIN recibido de la maqueta");
    // Aquí llamas a tu función de Firebase
    Firebase.RTDB.setString(&fbdo, "carrera/estado", "finalizada");
    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

void startServer() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t inicio_uri = { .uri="/inicio", .method=HTTP_GET, .handler=inicio_handler };
        httpd_register_uri_handler(server, &inicio_uri);

        httpd_uri_t fin_uri = { .uri="/fin", .method=HTTP_GET, .handler=fin_handler };
        httpd_register_uri_handler(server, &fin_uri);
    }
}


FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// --- Camera pins (AI Thinker) ---
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// HTTP server handle
httpd_handle_t stream_httpd = NULL;

// --- Función segura para convertir a int ---
int toInt(const String &s) {
  String t = s;
  t.trim();
  if (t.length() == 0) return 0;
  return t.toInt();
}

// MJPEG stream handler
static esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t * fb = NULL;
  char part_buf[64];
  const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=frame";
  const char* _STREAM_BOUNDARY = "\r\n--frame\r\n";
  const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

  httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);

  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Error al capturar frame");
      httpd_resp_send_500(req);
      return ESP_FAIL;
    }
    httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    size_t hlen = snprintf(part_buf, 64, _STREAM_PART, fb->len);
    httpd_resp_send_chunk(req, part_buf, hlen);
    httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
    esp_camera_fb_return(fb);
    vTaskDelay(pdMS_TO_TICKS(1));
  }
  return ESP_OK;
}

static esp_err_t index_handler(httpd_req_t *req) {
  const char* resp_str = "<html><body style='margin:0;padding:0;'><img src='/stream' style='width:100%;'></body></html>";
  httpd_resp_send(req, resp_str, strlen(resp_str));
  return ESP_OK;
}

void startCameraServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  httpd_uri_t index_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t stream_uri = {
    .uri       = "/stream",
    .method    = HTTP_GET,
    .handler   = stream_handler,
    .user_ctx  = NULL
  };

  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &index_uri);
    httpd_register_uri_handler(stream_httpd, &stream_uri);
  } else {
    Serial.println("Error al iniciar httpd");
  }
}

void initCamera() {
  camera_config_t configCam;
  configCam.ledc_channel = LEDC_CHANNEL_0;
  configCam.ledc_timer = LEDC_TIMER_0;
  configCam.pin_d0 = Y2_GPIO_NUM;
  configCam.pin_d1 = Y3_GPIO_NUM;
  configCam.pin_d2 = Y4_GPIO_NUM;
  configCam.pin_d3 = Y5_GPIO_NUM;
  configCam.pin_d4 = Y6_GPIO_NUM;
  configCam.pin_d5 = Y7_GPIO_NUM;
  configCam.pin_d6 = Y8_GPIO_NUM;
  configCam.pin_d7 = Y9_GPIO_NUM;
  configCam.pin_xclk = XCLK_GPIO_NUM;
  configCam.pin_pclk = PCLK_GPIO_NUM;
  configCam.pin_vsync = VSYNC_GPIO_NUM;
  configCam.pin_href = HREF_GPIO_NUM;
  configCam.pin_sscb_sda = SIOD_GPIO_NUM;
  configCam.pin_sscb_scl = SIOC_GPIO_NUM;
  configCam.pin_pwdn = PWDN_GPIO_NUM;
  configCam.pin_reset = RESET_GPIO_NUM;
  configCam.xclk_freq_hz = 20000000;
  configCam.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    configCam.frame_size = FRAMESIZE_VGA;
    configCam.jpeg_quality = 10;
    configCam.fb_count = 2;
  } else {
    configCam.frame_size = FRAMESIZE_SVGA;
    configCam.jpeg_quality = 12;
    configCam.fb_count = 1;
  }

  if (esp_camera_init(&configCam) != ESP_OK) {
    Serial.println("Error inicializando cámara");
    while (true) delay(1000);
  }
}

// Base de la ruta en tu DB
static const char* PATH_BASE = "/Prueba";
static const char* KEYS[] = {"Arriba", "Abajo", "Izquierda", "Derecha", "Central"};
static const size_t NUM_KEYS = sizeof(KEYS)/sizeof(KEYS[0]);

void setup() {
  Serial.begin(115200);
  delay(500);

  // 1) Conexión WiFi
  Serial.print("Conectando a WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - startAttempt) < 15000) {
    delay(300);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n[ERROR] No se pudo conectar a WiFi (timeout).");
    return;
  }
  Serial.println();
  Serial.print("WiFi conectado, IP: ");
  Serial.println(WiFi.localIP());

  // 2) Inicializar Firebase
  Serial.println("Inicializando Firebase...");
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = "leoleohero00@gmail.com";
  auth.user.password = "Humanoith1941";
  config.token_status_callback = tokenStatusCallback;
  Firebase.reconnectWiFi(true);
  Firebase.begin(&config, &auth);

  if (Firebase.ready()) {
    for (size_t i = 0; i < NUM_KEYS; ++i) {
      char path[64];
      snprintf(path, sizeof(path), "%s/%s", PATH_BASE, KEYS[i]);
      if (Firebase.RTDB.getInt(&fbdo, path)) {
        if (fbdo.dataType() == "int") {
          Serial.printf("Lectura inicial %s: %d\n", path, fbdo.intData());
        } else if (fbdo.dataType() == "string") {
          Serial.printf("Lectura inicial %s (string): %d\n", path, toInt(fbdo.stringData()));
        }
      }
    }
    startServer();
  }

  // 3) Motores, cámara y servidor
  Motor::begin();
  Serial.println("Inicializando cámara...");
  initCamera();
  startCameraServer();
  Serial.println("Camera server started");
}

void loop() {
  if (Firebase.ready()) {
    for (size_t i = 0; i < NUM_KEYS; ++i) {
      char path[64];
      snprintf(path, sizeof(path), "%s/%s", PATH_BASE, KEYS[i]);

      int value = -1;
      bool gotValue = false;

      if (Firebase.RTDB.getInt(&fbdo, path)) {
        if (fbdo.dataType() == "int") {
          value = fbdo.intData();
          gotValue = true;
        } else if (fbdo.dataType() == "string") {
          value = toInt(fbdo.stringData());
          gotValue = true;
        }
      } else {
        if (Firebase.RTDB.getString(&fbdo, path)) {
          if (fbdo.dataType() == "string") {
            value = toInt(fbdo.stringData());
            gotValue = true;
          }
        } else {
          Serial.printf("Error leyendo %s: %s\n", path, fbdo.errorReason().c_str());
        }
      }

      if (gotValue && value != -1) {
        Serial.printf("Ejecutando %s con valor %d\n", path, value);

        switch (value) {
          case 0: Motor::forward(); break; // Arriba
          case 1: Motor::back();    break; // Abajo
          case 2: Motor::left();    break; // Izquierda
          case 3: Motor::right();   break; // Derecha
          case 4: Motor::stop();    break; // Central
          default:
            Serial.println("Valor no reconocido");
            break;
        }

        // Después de ejecutar, marcar como "sin comando"
        if (!Firebase.RTDB.setInt(&fbdo, path, -1)) {
          Serial.printf("Error al resetear %s: %s\n", path, fbdo.errorReason().c_str());
        }
      }
      }
    }
}